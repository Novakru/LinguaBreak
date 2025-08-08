#include "ScalarEvolution.h"
#include <iostream>

// #define scev_debug

#ifdef scev_debug
#define SCEV_DEBUG_PRINT(x) do { x; } while(0)
#else
#define SCEV_DEBUG_PRINT(x) do {} while(0)
#endif

void SCEVConstant::print(std::ostream &OS) const {
    OS << getValue()->GetIntImmVal();
}

void SCEVUnknown::print(std::ostream &OS) const {
	if(isLoopInvariant){
		OS << getValue() << " (I)"; // invariant
	} else {
		OS << getValue() << " (U)"; // unknown
	}
}

void SCEVCommutativeExpr::print(std::ostream &OS) const {
    OS << "(";
    for (size_t i = 0; i < getOperands().size(); ++i) {
        if (i > 0) {
            OS << (getType() == scAddExpr ? " + " : " * ");
        }
        getOperands()[i]->print(OS);
    }
    OS << ")";
}

void SCEVAddRecExpr::print(std::ostream &OS) const {
    OS << "{" ;
    getStart()->print(OS);
    OS << ",+,";
    getStep()->print(OS);
    OS << "}";
}

void SCEV::printType(std::ostream &OS) const {
	switch(Type){
		case scConstant:
			OS << "Constant ";
			break;
		case scUnknown:
			OS << "Unknown ";
			break;
		case scAddExpr:
			OS << "AddExpr ";
			break;
		case scMulExpr:
			OS << "MulExpr ";
			break;
		case scAddRecExpr:
			OS << "AddRecExpr ";
			break;
	}	
}

Instruction ScalarEvolution::getDef(Operand V) const {
    if (V->GetOperandType() != BasicOperand::REG) {
        return nullptr;
    }
    int regNo = ((RegOperand*)V)->GetRegNo();
    if (C->def_map.count(regNo)) {
        return C->def_map.at(regNo);
    }
    return nullptr;
}

ScalarEvolution::ScalarEvolution(CFG* C, LoopInfo *LI, DominatorTree *DT)
    : C(C), LI(LI), DT(DT) {
    for (auto& loop_pair : LI->getTopLevelLoops()) {
        analyzeLoop(loop_pair);
    }
}

void ScalarEvolution::analyzeLoop(Loop *L) {
    for (LLVMBlock BB : L->getBlocks()) {
        for (Instruction Inst : BB->Instruction_list) {
            if (Inst->GetResult() != nullptr) {
                getSCEV(Inst->GetResult(), L);
            }
        }
    }
    for (auto& subloop_pair : L->getSubLoops()) {
        analyzeLoop(subloop_pair);
    }
}

bool ScalarEvolution::isLoopInvariant(Operand V, Loop *L) const {
	// std::cout << "[isLoopInvariant] 分析变量: " << V << " in LoopHeader: " << L->getHeader()->block_id << std::endl;
    auto key = std::make_pair(V, L);
    auto it = invariantCache.find(key);
    if (it != invariantCache.end()) return it->second;

    if (V->GetOperandType() == BasicOperand::IMMI32 || V->GetOperandType() == BasicOperand::GLOBAL) {
        return invariantCache[key] = true;
    }
    Instruction Def = getDef(V);
    if (!Def) {
        return invariantCache[key] = true;
    }
	// 如果定义是PHI，并且定义在循环头，则是归纳变量，不是循环不变量
	// if(Def->GetOpcode() == BasicInstruction::PHI && L->getHeader()->block_id == Def->GetBlockID()){
	// 	return invariantCache[key] = false;
	// }
	LLVMBlock DefBlock = C->GetBlockWithId(Def->GetBlockID());
	if(Def->GetOpcode() == BasicInstruction::PHI && L->contains(DefBlock)){ // 嵌套循环，包含自循环头
		return invariantCache[key] = false;
	}
    if (!L->contains(DefBlock)) {
        return invariantCache[key] = true;
    }
    for (Operand op : Def->GetNonResultOperands()) {
        if (!isLoopInvariant(op, L)) {
            return invariantCache[key] = false;
        }
    }
    return invariantCache[key] = true;
}

SCEV *ScalarEvolution::getSCEV(Operand V, Loop *L) {
    SCEV_DEBUG_PRINT(std::cout << "[getSCEV] 分析变量: " << V << " in LoopHeader: " << L->getHeader()->block_id << std::endl);
    if (LoopToSCEVMap.count(L) && LoopToSCEVMap.at(L).count(V)) {
        return LoopToSCEVMap.at(L).at(V);
    }
    SCEV *S = createSCEV(V, L);
    LoopToSCEVMap[L][V] = S;
    return S;
}

SCEV *ScalarEvolution::getSimpleSCEV(Operand V, Loop *L) {
    if (LoopToSCEVMap.count(L) && LoopToSCEVMap.at(L).count(V)) {
        auto scev = LoopToSCEVMap.at(L).at(V);
		if (SimplifiedSCEVCache.count(scev)) {
			return SimplifiedSCEVCache.at(scev);
		} else {
			SCEV* simplifiedSCEV = simplify(scev);
			SimplifiedSCEVCache[scev] = simplifiedSCEV;
			return simplifiedSCEV;
		}
    }

	// 如果查表没有，则创建一个，并简化
    SCEV* createdSCEV = getSCEV(V, L);
	SCEV* simplifiedSCEV = simplify(createdSCEV);
	SimplifiedSCEVCache[createdSCEV] = simplifiedSCEV;
	return simplifiedSCEV;
}

SCEV *ScalarEvolution::createSCEV(Operand V, Loop *L) {
    if (isLoopInvariant(V, L)) {
        SCEV_DEBUG_PRINT(std::cout << "[createSCEV] " << V << " 是循环不变量" << std::endl);
        if (V->GetOperandType() == BasicOperand::IMMI32) {
            return getSCEVConstant(static_cast<ImmI32Operand*>(V));
        }
        return getSCEVUnknown(V, L);
    }

    Instruction I = getDef(V);
    if (!I) {
        SCEV_DEBUG_PRINT(std::cout << "[createSCEV] " << V << " 没有定义指令，返回Unknown" << std::endl);
        return getSCEVUnknown(V, L);
    }

    if (auto *GEP = dynamic_cast<GetElementptrInstruction*>(I)) {
        SCEV_DEBUG_PRINT(std::cout << "[createSCEV] " << V << " 的定义是GEP指令" << std::endl);

        auto Ptr = GEP->GetPtrVal();
        auto Dims = GEP->GetDims();
        auto Indexes = GEP->GetIndexes();
        
        SCEV* BaseSCEV = getSCEV(Ptr, L);
        SCEV* TotalOffsetSCEV = getSCEVConstant(new ImmI32Operand(0));
    
        for (size_t i = 0; i < Indexes.size(); ++i) {
            SCEV* IndexSCEV = getSCEV(Indexes[i], L);
            if (auto C = dynamic_cast<SCEVConstant*>(IndexSCEV); C && C->getValue()->GetIntImmVal() == 0) {
                continue;
            }
            SCEV* ScaledIndex;
            if (i == 0) { // 第一个index，乘以整个数组大小
                 SCEV* SizeOfArray = getSCEVConstant(new ImmI32Operand(1));
                 for(int d : Dims) SizeOfArray = getMulExpr({SizeOfArray, getSCEVConstant(new ImmI32Operand(d))});
                 ScaledIndex = getMulExpr({IndexSCEV, SizeOfArray});
            } else { // 后续的index
                 SCEV* SizeOfSubArray = getSCEVConstant(new ImmI32Operand(1));
                 for(size_t j = i; j < Dims.size(); ++j) {
                     SizeOfSubArray = getMulExpr({SizeOfSubArray, getSCEVConstant(new ImmI32Operand(Dims[j]))});
                 }
                 ScaledIndex = getMulExpr({IndexSCEV, SizeOfSubArray});
            }

            TotalOffsetSCEV = getAddExpr({TotalOffsetSCEV, ScaledIndex});
        }

        return getAddExpr({BaseSCEV, TotalOffsetSCEV});
    }

    if (auto *PN = dynamic_cast<PhiInstruction*>(I)) {
        SCEV_DEBUG_PRINT(std::cout << "[createSCEV] " << V << " 的定义是PHI, block_id=" << PN->GetBlockID() << std::endl);
        if (L->getHeader()->block_id == PN->GetBlockID()) {
            return createAddRecFromPHI(PN, L);
        } else {
            return getSCEVUnknown(V, L);
        }
    }

    if (auto *ArithI = dynamic_cast<ArithmeticInstruction*>(I)) {
        SCEV_DEBUG_PRINT(std::cout << "[createSCEV] " << V << " 的定义是算术指令, opcode=" << ArithI->GetOpcode() << std::endl);
        return createSCEVForArithmetic(ArithI, L);
    }

    SCEV_DEBUG_PRINT(std::cout << "[createSCEV] " << V << " 的定义类型未知，返回Unknown" << std::endl);
    return getSCEVUnknown(V, L);
}

SCEV *ScalarEvolution::createAddRecFromPHI(PhiInstruction *PN, Loop *L) {
    LLVMBlock preheader = L->getPreheader();
    if (!preheader) {
        SCEV_DEBUG_PRINT(std::cout << "[createAddRecFromPHI] 没有preheader, 返回Unknown" << std::endl);
        return getSCEVUnknown(PN->GetResult(), L);
    }
    
    Operand startValue = nullptr;
    Operand stepInstOperand = nullptr;

    for (const auto& pair : PN->GetPhiList()) {
        LLVMBlock incomingBlock = C->GetBlockWithId(((LabelOperand*)pair.first)->GetLabelNo());
        SCEV_DEBUG_PRINT(std::cout << "[createAddRecFromPHI] PHI分支: label=" << ((LabelOperand*)pair.first)->GetLabelNo()
                  << " value=" << pair.second << std::endl);
        if (incomingBlock == preheader) {
            startValue = pair.second;
        } else if (L->contains(incomingBlock)) { // From a latch
            stepInstOperand = pair.second;
        }
    }

    if (!startValue || !stepInstOperand) {
        SCEV_DEBUG_PRINT(std::cout << "[createAddRecFromPHI] start/step未识别, 返回Unknown" << std::endl);
        return getSCEVUnknown(PN->GetResult(), L);
    }
    
    Instruction stepInstruction = getDef(stepInstOperand);
    auto* ArithI = dynamic_cast<ArithmeticInstruction*>(stepInstruction);

    if (!ArithI || (ArithI->GetOpcode() != BasicInstruction::ADD && ArithI->GetOpcode() != BasicInstruction::SUB)) {
        SCEV_DEBUG_PRINT(std::cout << "[createAddRecFromPHI] step不是加法或减法, 返回Unknown" << std::endl);
        return getSCEVUnknown(PN->GetResult(), L);
    }

    Operand recurrentOperand = nullptr;
    Operand stepValue = nullptr;
    bool isSubtraction = (ArithI->GetOpcode() == BasicInstruction::SUB);
    
    if (ArithI->GetOperand1() == PN->GetResult()) {
        recurrentOperand = ArithI->GetOperand1();
        stepValue = ArithI->GetOperand2();
    } else if (ArithI->GetOperand2() == PN->GetResult()) {
        recurrentOperand = ArithI->GetOperand2();
        stepValue = ArithI->GetOperand1();
    } else {
        SCEV_DEBUG_PRINT(std::cout << "[createAddRecFromPHI] step算术指令没有归纳变量, 返回Unknown" << std::endl);
        return getSCEVUnknown(PN->GetResult(), L);
    }
    
    SCEV_DEBUG_PRINT(std::cout << "[createAddRecFromPHI] start=" << startValue << " step=" << stepValue << " isSubtraction=" << isSubtraction << std::endl);
    SCEV* StartSCEV = getSCEV(startValue, L);
    SCEV* StepSCEV = getSCEV(stepValue, L);
    
    // 如果是减法，需要将step取负
    if (isSubtraction) {
        StepSCEV = getMulExpr({StepSCEV, getSCEVConstant(new ImmI32Operand(-1))});
    }

    return getAddRecExpr(StartSCEV, StepSCEV, L);
}

SCEV* ScalarEvolution::simplify(SCEV* S) const {
    SCEV* prev = S->clone();
    while (true) {
        SCEV* next = simplifyOnce(prev);
        if (SCEVStructurallyEqual(next, prev)) break;
        prev = next;
    }
    return SimplifiedSCEVCache[S] = prev;
}

SCEV *ScalarEvolution::simplifyOnce(SCEV *S) const {
    if (SimplifiedSCEVCache.count(S)) {
        return SimplifiedSCEVCache.at(S);
    }
    switch (S->getType()) {
        case scConstant:
        case scUnknown:
            return S;
        case scAddExpr: {
            auto *Add = static_cast<SCEVAddExpr*>(S);
            std::vector<SCEV *> flatOps;
            int constSum = 0;
            for (auto *Op : Add->getOperands()) {
                SCEV* simplifiedOp = simplify(Op);
                if (auto *C = dynamic_cast<SCEVConstant*>(simplifiedOp)) {
                    constSum += C->getValue()->GetIntImmVal();
                } else if (auto *NestedAdd = dynamic_cast<SCEVAddExpr*>(simplifiedOp)) {
                    for (auto *subOp : NestedAdd->getOperands()) {
                        flatOps.push_back(subOp);
                    }
                } else {
                    flatOps.push_back(simplifiedOp);
                }
            }
            if (constSum != 0) {
                flatOps.push_back(getSCEVConstant(new ImmI32Operand(constSum)));
            }
            // 合并AddRec和常量
            for (size_t i = 0; i < flatOps.size(); ++i) {
                for (size_t j = i + 1; j < flatOps.size(); ) {
                    auto *A = dynamic_cast<SCEVAddRecExpr*>(flatOps[i]);
                    auto *B = dynamic_cast<SCEVAddRecExpr*>(flatOps[j]);
                    if (A && B && A->getLoop() == B->getLoop()) {
                        SCEV *newStart = simplify(getAddExpr({A->getStart(), B->getStart()}));
                        SCEV *newStep = simplify(getAddExpr({A->getStep(), B->getStep()}));
                        flatOps[i] = getAddRecExpr(newStart, newStep, const_cast<Loop*>(A->getLoop()));
                        flatOps.erase(flatOps.begin() + j);
                    } else {
                        ++j;
                    }
                }
            }
            // AddRec + 常量 合并
            for (size_t i = 0; i < flatOps.size(); ++i) {
                if (auto *A = dynamic_cast<SCEVAddRecExpr*>(flatOps[i])) {
                    for (size_t j = 0; j < flatOps.size(); ++j) {
                        if (i == j) continue;
                        if (auto *C = dynamic_cast<SCEVConstant*>(flatOps[j])) {
                            SCEV *newStart = simplify(getAddExpr({A->getStart(), C}));
                            flatOps[i] = getAddRecExpr(newStart, A->getStep(), const_cast<Loop*>(A->getLoop()));
                            flatOps.erase(flatOps.begin() + j);
                            if (j < i) --i;
                            break;
                        }
                    }
                }
            }
            // 判断是否有变化
            if (flatOps.size() == Add->getOperands().size()) {
                bool allSame = true;
                for (size_t i = 0; i < flatOps.size(); ++i) {
                    if (flatOps[i] != Add->getOperands()[i]) {
                        allSame = false;
                        break;
                    }
                }
                if (allSame) return S;
            }
            if (flatOps.size() == 0) return getSCEVConstant(new ImmI32Operand(0));
            if (flatOps.size() == 1) return flatOps[0];
            return getAddExpr(flatOps);
        }
        case scMulExpr: {
            auto *Mul = static_cast<SCEVMulExpr*>(S);
            std::vector<SCEV *> flatOps;
            int constProd = 1;
            for (auto *Op : Mul->getOperands()) {
                SCEV* simplifiedOp = simplify(Op);
                if (auto *C = dynamic_cast<SCEVConstant*>(simplifiedOp)) {
                    constProd *= C->getValue()->GetIntImmVal();
                } else if (auto *NestedMul = dynamic_cast<SCEVMulExpr*>(simplifiedOp)) {
                    for (auto *subOp : NestedMul->getOperands()) {
                        flatOps.push_back(subOp);
                    }
                } else {
                    flatOps.push_back(simplifiedOp);
                }
            }
            if (constProd == 0) {
                return getSCEVConstant(new ImmI32Operand(0));
            }
            // 只要有且仅有一个AddRec和常量，直接分配律展开
            if (flatOps.size() == 1 && dynamic_cast<SCEVAddRecExpr*>(flatOps[0]) && constProd != 1) {
                auto *addRec = static_cast<SCEVAddRecExpr*>(flatOps[0]);
                SCEV* newStart = simplify(getMulExpr({addRec->getStart(), getSCEVConstant(new ImmI32Operand(constProd))}));
                SCEV* newStep = simplify(getMulExpr({addRec->getStep(), getSCEVConstant(new ImmI32Operand(constProd))}));
                return getAddRecExpr(newStart, newStep, const_cast<Loop*>(addRec->getLoop()));
            }
            if (constProd != 1) flatOps.push_back(getSCEVConstant(new ImmI32Operand(constProd)));
            if (flatOps.size() == 0) return getSCEVConstant(new ImmI32Operand(1));
            if (flatOps.size() == 1) return flatOps[0];
            return getMulExpr(flatOps);
        }
        case scAddRecExpr: {
            auto *AddRec = static_cast<SCEVAddRecExpr*>(S);
            SCEV *Start = simplify(AddRec->getStart());
            SCEV *Step = simplify(AddRec->getStep());
            if (auto *StepC = dynamic_cast<SCEVConstant*>(Step); StepC && StepC->getValue()->GetIntImmVal() == 0) {
                return Start;
            }
            if (Start == AddRec->getStart() && Step == AddRec->getStep()) return S;
            return getAddRecExpr(Start, Step, const_cast<Loop*>(AddRec->getLoop()));
        }
    }
    return S;
}

SCEV *ScalarEvolution::createSCEVForArithmetic(ArithmeticInstruction *I, Loop *L) {
    SCEV *Op1 = getSCEV(I->GetOperand1(), L);
    SCEV *Op2 = getSCEV(I->GetOperand2(), L);
    
    if (I->GetOpcode() == BasicInstruction::ADD) {
        return getAddExpr({Op1, Op2});
    }
    if (I->GetOpcode() == BasicInstruction::SUB) {
        // SUB: a - b == a + (-b)
        SCEV* negOp2 = getMulExpr({Op2, getSCEVConstant(new ImmI32Operand(-1))});
        return getAddExpr({Op1, negOp2});
    }
    if (I->GetOpcode() == BasicInstruction::MUL) {
        return getMulExpr({Op1, Op2});
    }
    
    return getSCEVUnknown(I->GetResult(), L);
}


SCEV *ScalarEvolution::getSCEVUnknown(Operand V, Loop *L) const {
    SCEV *S = new SCEVUnknown(V, isLoopInvariant(V, L));
    Allocations.push_back(S);
    return S;
}

SCEV *ScalarEvolution::getSCEVConstant(ImmI32Operand *C) const {
    SCEV *S = new SCEVConstant(C);
    Allocations.push_back(S);
    return S;
}

SCEV *ScalarEvolution::getAddExpr(const std::vector<SCEV *> &Ops) const {
    SCEV *S = new SCEVAddExpr(Ops);
    Allocations.push_back(S);
    return S;
}

SCEV *ScalarEvolution::getMulExpr(const std::vector<SCEV *> &Ops) const {
    SCEV *S = new SCEVMulExpr(Ops);
    Allocations.push_back(S);
    return S;
}

SCEV *ScalarEvolution::getAddRecExpr(SCEV *Start, SCEV *Step, Loop *L) const {
    SCEV *S = new SCEVAddRecExpr(Start, Step, L);
    Allocations.push_back(S);
    return S;
}

void ScalarEvolution::print(std::ostream &OS) const {
    OS << "ScalarEvolution Analysis for function " << C->function_def->GetFunctionName() << "\n";
    for (auto const& [L, ScevMap] : LoopToSCEVMap) {
        OS << "Loop at depth " << L->getLoopDepth() << " with header " << L->getHeader()->block_id << ":\n";
        for (auto const& [V, S] : ScevMap) {
            OS << "  " << V << " => ";
			OS << "Type: ";
			S->printType(OS);
			OS << " Expr: ";
            S->print(OS);
            if (SimplifiedSCEVCache.count(S)) {
                OS << " simplifiedExpr: ";
                SimplifiedSCEVCache.at(S)->print(OS);
                OS << "\n";
            } else {
                OS << "\n";
            }
        }
    }
} 

void SCEVPass::Execute() {
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		// SCEV_DEBUG_PRINT(std::cerr << "ScalarEvolution Analysis for function " << cfg->function_def->GetFunctionName() << "\n");
		cfg->reSetBlockID();
		ScalarEvolution* SE = new ScalarEvolution(cfg, cfg->getLoopInfo(), cfg->getDomTree());
		cfg->SCEVInfo = SE;
		simplifyAllSCEVExpr();
		// fixAllSCEVExpr();
		SE->print(std::cout);
    }
}

void SCEVPass::simplifyAllSCEVExpr() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        if(cfg->SCEVInfo) {
            ScalarEvolution* SE = cfg->getSCEVInfo();
            for (auto& [L, ScevMap] : SE->LoopToSCEVMap) {
                for (auto& [V, S] : ScevMap) {
                    SE->simplify(S);
                }
            }
        }
    }
}

SCEV* SCEVConstant::clone() const { return new SCEVConstant(*this); }
SCEV* SCEVUnknown::clone() const { return new SCEVUnknown(*this); }
SCEV* SCEVAddExpr::clone() const {
    std::vector<SCEV*> newOps;
    for (auto* op : Operands) {
        newOps.push_back(op->clone());
    }
    return new SCEVAddExpr(newOps);
}
SCEV* SCEVMulExpr::clone() const {
    std::vector<SCEV*> newOps;
    for (auto* op : Operands) {
        newOps.push_back(op->clone());
    }
    return new SCEVMulExpr(newOps);
}
SCEV* SCEVAddRecExpr::clone() const {
    return new SCEVAddRecExpr(Start->clone(), Step->clone(), L);
}

bool ScalarEvolution::SCEVStructurallyEqual(const SCEV* A, const SCEV* B) {
    if (A == B) return true;
    if (!A || !B) return false;
    if (A->getType() != B->getType()) return false;
    switch (A->getType()) {
        case scConstant: {
            auto* CA = static_cast<const SCEVConstant*>(A);
            auto* CB = static_cast<const SCEVConstant*>(B);
            return CA->getValue()->GetIntImmVal() == CB->getValue()->GetIntImmVal();
        }
        case scUnknown: {
            auto* UA = static_cast<const SCEVUnknown*>(A);
            auto* UB = static_cast<const SCEVUnknown*>(B);
            return UA->getValue() == UB->getValue();
        }
        case scAddExpr:
        case scMulExpr: {
            auto* EA = static_cast<const SCEVCommutativeExpr*>(A);
            auto* EB = static_cast<const SCEVCommutativeExpr*>(B);
            if (EA->getOperands().size() != EB->getOperands().size()) return false;
            for (size_t i = 0; i < EA->getOperands().size(); ++i) {
                if (!SCEVStructurallyEqual(EA->getOperands()[i], EB->getOperands()[i])) return false;
            }
            return true;
        }
        case scAddRecExpr: {
            auto* RA = static_cast<const SCEVAddRecExpr*>(A);
            auto* RB = static_cast<const SCEVAddRecExpr*>(B);
            return SCEVStructurallyEqual(RA->getStart(), RB->getStart()) &&
                   SCEVStructurallyEqual(RA->getStep(), RB->getStep()) &&
                   RA->getLoop() == RB->getLoop();
        }
    }
    return false;
}

void SCEVPass::fixAllSCEVExpr() {
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		if(cfg->SCEVInfo) {
			ScalarEvolution* SE = cfg->getSCEVInfo();
			for (auto& [L, ScevMap] : SE->LoopToSCEVMap) {
				for (auto& [V, S] : ScevMap) {
					SCEV* simplifiedS = SE->SimplifiedSCEVCache.at(S);
					SCEV* fixedS = SE->fixLoopInvariantUnknowns(simplifiedS, L);
					fixedS = SE->simplify(fixedS);
					SE->SimplifiedSCEVCache[S] = fixedS;
				}
			}
		}
	}
}

// 递归修正：将循环内但本质循环不变量的SCEVUnknown递归还原为真实表达式
SCEV* ScalarEvolution::fixLoopInvariantUnknowns(SCEV* scev, Loop* L) {
    if (auto* unk = dynamic_cast<SCEVUnknown*>(scev)) {
        Operand V = unk->getValue();
        Instruction Def = getDef(V);
        LLVMBlock DefBlock = C->GetBlockWithId(Def->GetBlockID());
        if (Def && DefBlock && L->contains(DefBlock) && isLoopInvariant(V, L)) {
            // 如果是算术指令，递归还原为表达式
            if (auto* arith = dynamic_cast<ArithmeticInstruction*>(Def)) {
                SCEV* op1 = fixLoopInvariantUnknowns(createSCEV(arith->GetOperand1(), L), L);
                SCEV* op2 = fixLoopInvariantUnknowns(createSCEV(arith->GetOperand2(), L), L);
                if (arith->GetOpcode() == BasicInstruction::ADD) {
                    return getAddExpr({op1, op2});
                }
                if (arith->GetOpcode() == BasicInstruction::SUB) {
                    // SUB: a - b == a + (-b)
                    SCEV* negOp2 = getMulExpr({op2, getSCEVConstant(new ImmI32Operand(-1))});
                    return getAddExpr({op1, negOp2});
                }
                if (arith->GetOpcode() == BasicInstruction::MUL) {
                    return getMulExpr({op1, op2});
                }
                // 其它算术类型可按需扩展
            }
            // 其它类型递归还原
            SCEV* real = createSCEV(V, L);
            if (dynamic_cast<SCEVUnknown*>(real) == nullptr) {
                return fixLoopInvariantUnknowns(real, L);
            }
        }
        return scev;
    }
	if (auto* add = dynamic_cast<SCEVAddExpr*>(scev)) {
        std::vector<SCEV*> newOps;
        bool changed = false;
        for (auto* op : add->getOperands()) {
            SCEV* fixed = fixLoopInvariantUnknowns(op, L);
            if (fixed != op) changed = true;
            newOps.push_back(fixed);
        }
        if (!changed) return scev;
        return getAddExpr(newOps);
    }
	if (auto* mul = dynamic_cast<SCEVMulExpr*>(scev)) {
        std::vector<SCEV*> newOps;
        bool changed = false;
        for (auto* op : mul->getOperands()) {
            SCEV* fixed = fixLoopInvariantUnknowns(op, L);
            if (fixed != op) changed = true;
            newOps.push_back(fixed);
        }
        if (!changed) return scev;
        return getMulExpr(newOps);
    }
	if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(scev)) {
		SCEV* newStart = fixLoopInvariantUnknowns(addrec->getStart(), L);
        SCEV* newStep = fixLoopInvariantUnknowns(addrec->getStep(), L);
        if (newStart == addrec->getStart() && newStep == addrec->getStep()) return scev;
        return getAddRecExpr(newStart, newStep, const_cast<Loop*>(addrec->getLoop()));
	}
    return scev;
}