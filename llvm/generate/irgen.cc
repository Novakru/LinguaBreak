#include "../../include/ast.h"
#include "IRgen.h"
#include "../include/ir.h"
#include "semant.h"
int max_reg = -1;
int max_label = -1;

extern SemantTable semant_table;    // 也许你会需要一些语义分析的信息

IRgenTable irgen_table;    // 中间代码生成的辅助变量
LLVMIR llvmIR;             // 我们需要在这个变量中生成中间代码

std::map<Symbol,Operand> ptrmap;
Operand currentptr;
Operand currentop;
int currentint;
float currentfloat;
Symbol currentname;
FunctionDefineInstruction* funcdefI;

enum operand_type { I32 = 1, FLOAT32 = 2, BOOL = 3, I32_PTR = 4, FLOAT32_PTR = 5};
std::map<int,operand_type> optype_map;

// 判断左值还是右值，左值的上层为AssignmentLval的左部，其余都为右值

bool isLeftlval;
bool isRightlval;

// 判断是否为函数参数

bool isParam;

// 处理break continue

std::deque<LLVMBlock> continuebbque;
// 存储需要跳转到entrybb (continue) 的基本块
std::deque<LLVMBlock> breakbbque;
// 存储需要跳转到exitbb (break) 的基本块

// 处理短路求值

std::deque<LLVMBlock> truebbque;
std::deque<LLVMBlock> falsebbque;
BasicBlock* genbb;

/*************************************************************************************
 *					                数组初值处理 	
 *************************************************************************************
 * 	ISO_IEC_9899  p117 example9  section 6.7.9
 *	
 * 	除了最外围的{}以外，每次遇到嵌套的{}都会进入一个子数组(subArray)
 * 	subArray.size() 取决于括号所处的位置，
 * 	如q[3][5][2], As subArray begin at pos
 * 	if(pos % (2 * 5) == 0) 开辟 q[]     这样的数组, subArray.size() = 10;
 * 	if(pos % (2) == 0)     开辟 q[][]   这样的数组, subArray.size() = 2;
 * 	else                   开辟 q[][][] 这样的数组, subArray.size() = 1; (线性填充)
 * 	
 * 	如果开辟的数组大小没有{}内的元素多，就会顺序填充，例如 e 数组
 * 	
 * 
 * 	我们想要实现如下的转换，即将{}嵌套的数组初始化，转换为等效的std::vector<int> initValint
 * 	c[3][5][2] = {{1}, 2, {3}, 4, 5, {3}, 4, 5, {6}, {7}};
 * 	<=> c[3][5][2] = {1,0,0,0,0,0,0,0,0,0,2,3,4,5,3,0,4,5,6,0,7,0,0,0,0,0,0,0,0,0}
 * 	
 * 	d[3][5][2] = {{1}, 2, {3, 5}, 4};
 * 	<=> d[3][5][2] = {1,0,0,0,0,0,0,0,0,0,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
 * 	
 * 	e[3][5][2] = {{1}, {2}, {3, 5, 6, 7}};
 * 	<=> e[3][5][2] = {1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,3,5,6,7,0,0,0,0,0,0}
 * 	
 * 	f[3][5][2] = {{1}, {2, 3, {4}, 5}, 6}
 * 	<=> e[3][5][2] = {1,0,0,0,0,0,0,0,0,0,2,3,4,0,5,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0} 
 * 
 * 	具体数据维护可以看 handouts 中的数组初始化文档
 *  */

bool dimcount; 
// dimconut为true时表示目前的intcount对应数组定义的下标，不需要生成立即数相加的指令
std::set<int> paramptr; 
// 判断一个寄存器是否是函数参数和指针
// 函数参数的指针可以直接当数组指针作为getelementptr中的ptr使用
// 函数参数里的其他非指针变量需要先在entrybb里alloca，并把变量store进alloca的新指针
std::vector<int> initdim;
// 用于描述数组定义中各维度的数值
std::vector<int> initarrayint;
// 用于描述数组定义中的初值
std::vector<float> initarrayfloat;
// 用于描述数组定义中的初值

int head;
// 数组当前初始化的起始位置
bool isGobal;
bool isTopVarInitVal;
// 1 - 表示可以分配最大为数组去掉第 1 维的大小
// 2 - 表示可以分配最大为数组去掉第 2 维的大小
// ......
int subArraydim;
Operand initarrayReg;

// 根据数组当前初始化起始位置和维度信息，确定当前初始化的末尾 tail
int getTail(){
	int allocaSpace = 1, product = 1;

	// for(int i = initdim.size() - 1; i >= 1; i--) {
	// 	product *= initdim[i];
	// 	if(head % product == 0) {
	// 		allocaSpace = product;
	// 	}
	// 	else break;
	// }
	// std::cerr << "subArraydim : " << subArraydim << std::endl; 

	int subArraydim_copy = subArraydim;
	for(int i = initdim.size() - 1; i >= subArraydim_copy; i--) {
		product *= initdim[i];
		if(head % product == 0) {
			allocaSpace = product;
			subArraydim = i + 1;
		}
		else break;
	}

	return head + allocaSpace - 1;
}


// 线性偏移转换为数组各个维度的索引
std::vector<Operand> offset_to_indexs(int offset, const std::vector<int>& initdim) {
    int dims = initdim.size();
    std::vector<Operand> indexs(dims + 1, 0); 

	indexs[0] = new ImmI32Operand(0);
    for (int i = dims; i >= 1; --i) {
        indexs[i] = new ImmI32Operand(offset % initdim[i - 1]);
        offset /= initdim[i - 1];
    }
	
    return indexs;
}

void AddLibFunctionDeclare();

// 在基本块B末尾生成一条新指令
void IRgenArithmeticI32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticF32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticI32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int reg2, int result_reg);
void IRgenArithmeticF32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, int reg2,
                               int result_reg);
void IRgenArithmeticI32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int val2, int result_reg);
void IRgenArithmeticF32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, float val2,
                              int result_reg);

void IRgenIcmp(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenFcmp(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenIcmpImmRight(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int val2, int result_reg);
void IRgenFcmpImmRight(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, float val2, int result_reg);

void IRgenFptosi(LLVMBlock B, int src, int dst);
void IRgenSitofp(LLVMBlock B, int src, int dst);
void IRgenZextI1toI32(LLVMBlock B, int src, int dst);

void IRgenGetElementptrIndexI32(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                        std::vector<int> dims, std::vector<Operand> indexs);

void IRgenGetElementptrIndexI64(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                        std::vector<int> dims, std::vector<Operand> indexs);

void IRgenLoad(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, int value_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, Operand value, Operand ptr);

void IRgenCall(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg,
               std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);
void IRgenCallVoid(LLVMBlock B, BasicInstruction::LLVMType type,
                   std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);

void IRgenCallNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, std::string name);
void IRgenCallVoidNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, std::string name);

void IRgenRetReg(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenRetImmInt(LLVMBlock B, BasicInstruction::LLVMType type, int val);
void IRgenRetImmFloat(LLVMBlock B, BasicInstruction::LLVMType type, float val);
void IRgenRetVoid(LLVMBlock B);

void IRgenBRUnCond(LLVMBlock B, int dst_label);
void IRgenBrCond(LLVMBlock B, int cond_reg, int true_label, int false_label);

void IRgenAlloca(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenAllocaArray(LLVMBlock B, BasicInstruction::LLVMType type, int reg, std::vector<int> dims);

void IRgenTypeConverse(LLVMBlock B, BuiltinType::BuiltinKind type_src, BuiltinType::BuiltinKind type_dst, int src);
void IRgenGlobalVarDefineArray(std::string name, BasicInstruction::LLVMType type, VarAttribute v) {
    auto newI = new GlobalVarDefineInstruction(name, type, v);
    llvmIR.global_def.push_back(newI);
    
}
void IRgenGlobalVarDefine(std::string name, BasicInstruction::LLVMType type, Operand init_val) {
    auto newI = new GlobalVarDefineInstruction(name, type, init_val);
    llvmIR.global_def.push_back(newI);
}
RegOperand *GetNewRegOperand(int RegNo);

// generate TypeConverse Instructions from type_src to type_dst
// eg. you can use fptosi instruction to converse float to int
// eg. you can use zext instruction to converse bool to int
// LLVMType to_type, Operand result_receiver, LLVMType from_type, Operand value_for_cast
void IRgenTypeConverse(LLVMBlock B, BuiltinType::BuiltinKind type_src, BuiltinType::BuiltinKind type_dst, int src, int dst) {
    auto newresultop = GetNewRegOperand(dst);
    if(type_src == BuiltinType::BuiltinKind::Bool && type_dst == BuiltinType::BuiltinKind::Int){
        B->InsertInstruction(1, new ZextInstruction(BasicInstruction::LLVMType::I32, GetNewRegOperand(src), BasicInstruction::LLVMType::I1, newresultop));
        optype_map[dst] = operand_type::I32;
        currentop = newresultop;
    }else if(type_src == BuiltinType::BuiltinKind::Int && type_dst == BuiltinType::BuiltinKind::Bool){
        B->InsertInstruction(1, new ZextInstruction(BasicInstruction::LLVMType::I1, GetNewRegOperand(src), BasicInstruction::LLVMType::I32, newresultop));
        optype_map[dst] = operand_type::BOOL;
        currentop = newresultop;
    }else if(type_src == BuiltinType::BuiltinKind::Int && type_dst == BuiltinType::BuiltinKind::Float){
        B->InsertInstruction(1, new SitofpInstruction(GetNewRegOperand(src), newresultop));
        optype_map[dst] = operand_type::FLOAT32;
        currentop = newresultop;
    }else if(type_src == BuiltinType::BuiltinKind::Float && type_dst == BuiltinType::BuiltinKind::Int){
        B->InsertInstruction(1, new FptosiInstruction(GetNewRegOperand(src), newresultop));
        optype_map[dst] = operand_type::I32;
        currentop = newresultop;
    }
    
    // DONE("IRgenTypeConverse. Implement it if you need it");
}

void BasicBlock::InsertInstruction(int pos, Instruction Ins) {
    assert(pos == 0 || pos == 1);
    if (pos == 0) {
        Instruction_list.push_front(Ins);
    } else if (pos == 1) {
        Instruction_list.push_back(Ins);
    }
}

LLVMBlock& GetCurrentBlock(){
    return genbb == nullptr ? llvmIR.function_block_map[funcdefI][max_label] : genbb;
}

// 二元运算符需要保证两个的寄存器都非Bool，否则需要转换
void BinaryBoolCheckandConverse(LLVMBlock B, Operand &conversereg){
    if(B->Instruction_list.back()->GetOpcode() == BasicInstruction::ICMP || B->Instruction_list.back()->GetOpcode() == BasicInstruction::FCMP){
        auto newresultreg_zext = GetNewRegOperand(++max_reg);
        IRgenTypeConverse(B,
                BuiltinType::BuiltinKind::Bool,
                BuiltinType::BuiltinKind::Int,
                max_reg,
                ((RegOperand*)conversereg)->GetRegNo());
        optype_map[max_reg] = operand_type::I32;
        conversereg = newresultreg_zext;
    }else{
        conversereg = currentop;
    }
}

void BinaryFloattoIntConverse(LLVMBlock B, Operand &conversereg){
    auto newresultreg_zext = GetNewRegOperand(++max_reg);
    
    IRgenTypeConverse(B,
            BuiltinType::BuiltinKind::Float,
            BuiltinType::BuiltinKind::Int,
            max_reg,
            ((RegOperand*)conversereg)->GetRegNo());
    optype_map[max_reg] = operand_type::I32;
    conversereg = newresultreg_zext;
}

void BinaryInttoFloatConverse(LLVMBlock B, Operand &conversereg){
    auto newresultreg_zext = GetNewRegOperand(++max_reg);
    IRgenTypeConverse(B,
            BuiltinType::BuiltinKind::Int,
            BuiltinType::BuiltinKind::Float,
            max_reg,
            ((RegOperand*)conversereg)->GetRegNo());
    optype_map[max_reg] = operand_type::FLOAT32;
    conversereg = newresultreg_zext;
}



void GenArithmeticBinaryIR(NodeAttribute attribute, OpType::Op opKind, ExprBase lhs, ExprBase rhs) {
    currentop = nullptr;
    currentint = 0;
    currentfloat = 0;

    if (dimcount && attribute.ConstTag) {
        currentint = attribute.val.IntVal;
        currentfloat = attribute.val.FloatVal;
        return;
    }

    lhs->codeIR();
    auto leftop = currentop;
    BinaryBoolCheckandConverse(GetCurrentBlock(), leftop);

    rhs->codeIR();
    auto rightop = currentop;
    BinaryBoolCheckandConverse(GetCurrentBlock(), rightop);

    auto &entrybb = GetCurrentBlock();
    auto leftregno = ((RegOperand*)leftop)->GetRegNo();
    auto rightregno = ((RegOperand*)rightop)->GetRegNo();
    auto leftop_type = optype_map[leftregno];
    auto rightop_type = optype_map[rightregno];

    auto promoteToFloat = [&]() {
        if (leftop_type == operand_type::I32) {
            BinaryInttoFloatConverse(entrybb, leftop);
            leftregno = ((RegOperand*)leftop)->GetRegNo();
			leftop_type = optype_map[leftregno];
        }
        if (rightop_type == operand_type::I32) {
            BinaryInttoFloatConverse(entrybb, rightop);
            rightregno = ((RegOperand*)rightop)->GetRegNo();
			rightop_type = optype_map[rightregno];
        }
    };

    if (leftop_type == operand_type::I32 && rightop_type == operand_type::I32) {
        auto newreg = GetNewRegOperand(++max_reg);
        IRgenArithmeticI32(entrybb, mapToLLVMOpcodeInt(opKind), leftregno, rightregno, max_reg);
        optype_map[max_reg] = operand_type::I32;
        currentop = newreg;
    } else {
        promoteToFloat();
        auto newreg = GetNewRegOperand(++max_reg);
        IRgenArithmeticF32(entrybb, mapToLLVMOpcodeFloat(opKind), leftregno, rightregno, max_reg);
        optype_map[max_reg] = operand_type::FLOAT32;
        currentop = newreg;
    }
}

void GenCompareBinaryIR(OpType::Op opKind, ExprBase lhs, ExprBase rhs) {
    currentop = nullptr;
    currentint = 0;
    currentfloat = 0;

    lhs->codeIR();
    auto leftop = currentop;
    BinaryBoolCheckandConverse(GetCurrentBlock(), leftop);

    rhs->codeIR();
    auto rightop = currentop;
    BinaryBoolCheckandConverse(GetCurrentBlock(), rightop);

    auto &entrybb = GetCurrentBlock();
    auto leftregno = ((RegOperand*)leftop)->GetRegNo();
    auto rightregno = ((RegOperand*)rightop)->GetRegNo();
    auto leftop_type = optype_map[leftregno];
    auto rightop_type = optype_map[rightregno];

    if (leftop_type == operand_type::I32 && rightop_type == operand_type::FLOAT32) {
        BinaryInttoFloatConverse(entrybb, leftop);
        leftregno = ((RegOperand*)leftop)->GetRegNo();
        leftop_type = optype_map[leftregno];
    } else if (leftop_type == operand_type::FLOAT32 && rightop_type == operand_type::I32) {
        BinaryInttoFloatConverse(entrybb, rightop);
        rightregno = ((RegOperand*)rightop)->GetRegNo();
        rightop_type = optype_map[rightregno];
    }

    auto newreg = GetNewRegOperand(++max_reg);

    if (leftop_type == operand_type::FLOAT32 || rightop_type == operand_type::FLOAT32) {
        IRgenFcmp(entrybb, mapToFcmpCond(opKind), leftregno, rightregno, max_reg);
    } else {
        IRgenIcmp(entrybb, mapToIcmpCond(opKind), leftregno, rightregno, max_reg);
    }

    optype_map[max_reg] = operand_type::BOOL;
    currentop = newreg;
}


void Exp::codeIR() { addExp->codeIR(); }
void ConstExp::codeIR() {TODO("ConstExp::codeIR()");}
void AddExp::codeIR() { GenArithmeticBinaryIR(attribute, op.optype, addExp, mulExp);}
void MulExp::codeIR() { 
	// mod 特殊！todo
	GenArithmeticBinaryIR(attribute, op.optype, mulExp, unaryExp);	
}
void RelExp::codeIR() { GenCompareBinaryIR(op.optype, relexp, addexp);}
void EqExp::codeIR()  { GenCompareBinaryIR(op.optype, eqexp, relexp); }
void LAndExp::codeIR() {}
void LOrExp::codeIR() {}
void Lval::codeIR() {}
void FuncRParams::codeIR() {}
void FuncCall::codeIR() {}
void UnaryExp::codeIR() {}
void IntConst::codeIR() {}
void FloatConst::codeIR() {}
void PrimaryExp::codeIR() {}
void AssignStmt::codeIR() {}
void ExprStmt::codeIR() {}
void BlockStmt::codeIR() {}
void IfStmt::codeIR() {}
void WhileStmt::codeIR() {}
void ContinueStmt::codeIR() {}
void BreakStmt::codeIR() {}
void RetStmt::codeIR() {}
void ConstInitValList::codeIR() {}
void ConstInitVal::codeIR() {}
void VarInitValList::codeIR() {}
void VarInitVal::codeIR() {}
void VarDef_no_init::codeIR() {}
void VarDef::codeIR() {}
void ConstDef::codeIR() {}
void VarDecl::codeIR() {}
void ConstDecl::codeIR() {}
void BlockItem_Decl::codeIR() {}
void BlockItem_Stmt::codeIR() {}
void __Block::codeIR() {}
void __FuncFParam::codeIR() {}
void __FuncDef::codeIR() {}
void CompUnit_Decl::codeIR() {}
void CompUnit_FuncDef::codeIR() {}
void __Program::codeIR() {
    AddLibFunctionDeclare();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->codeIR();
    }
}

void AddLibFunctionDeclare() {
    FunctionDeclareInstruction *getint = new FunctionDeclareInstruction(BasicInstruction::I32, "getint");
    llvmIR.function_declare.push_back(getint);

    FunctionDeclareInstruction *getchar = new FunctionDeclareInstruction(BasicInstruction::I32, "getch");
    llvmIR.function_declare.push_back(getchar);

    FunctionDeclareInstruction *getfloat = new FunctionDeclareInstruction(BasicInstruction::FLOAT32, "getfloat");
    llvmIR.function_declare.push_back(getfloat);

    FunctionDeclareInstruction *getarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getarray");
    getarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getarray);

    FunctionDeclareInstruction *getfloatarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getfarray");
    getfloatarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getfloatarray);

    FunctionDeclareInstruction *putint = new FunctionDeclareInstruction(BasicInstruction::VOID, "putint");
    putint->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putint);

    FunctionDeclareInstruction *putch = new FunctionDeclareInstruction(BasicInstruction::VOID, "putch");
    putch->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putch);

    FunctionDeclareInstruction *putfloat = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfloat");
    putfloat->InsertFormal(BasicInstruction::FLOAT32);
    llvmIR.function_declare.push_back(putfloat);

    FunctionDeclareInstruction *putarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putarray");
    putarray->InsertFormal(BasicInstruction::I32);
    putarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putarray);

    FunctionDeclareInstruction *putfarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfarray");
    putfarray->InsertFormal(BasicInstruction::I32);
    putfarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putfarray);

    FunctionDeclareInstruction *starttime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_starttime");
    starttime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(starttime);

    FunctionDeclareInstruction *stoptime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_stoptime");
    stoptime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(stoptime);

    // 一些llvm自带的函数，也许会为你的优化提供帮助
    FunctionDeclareInstruction *llvm_memset =
    new FunctionDeclareInstruction(BasicInstruction::VOID, "llvm.memset.p0.i32");
    llvm_memset->InsertFormal(BasicInstruction::PTR);
    llvm_memset->InsertFormal(BasicInstruction::I8);
    llvm_memset->InsertFormal(BasicInstruction::I32);
    llvm_memset->InsertFormal(BasicInstruction::I1);
    llvmIR.function_declare.push_back(llvm_memset);

    FunctionDeclareInstruction *llvm_umax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umax.i32");
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umax);

    FunctionDeclareInstruction *llvm_umin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umin.i32");
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umin);

    FunctionDeclareInstruction *llvm_smax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smax.i32");
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smax);

    FunctionDeclareInstruction *llvm_smin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smin.i32");
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smin);
}
