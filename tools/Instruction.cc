#include "../include/Instruction.h"
#include "../include/basic_block.h"
#include <assert.h>
#include <unordered_map>

static std::unordered_map<int, RegOperand *> RegOperandMap;
static std::map<int, LabelOperand *> LabelOperandMap;
static std::map<std::string, GlobalOperand *> GlobalOperandMap;

RegOperand *GetNewRegOperand(int RegNo) {
    auto it = RegOperandMap.find(RegNo);
    if (it == RegOperandMap.end()) {
        auto R = new RegOperand(RegNo);
        RegOperandMap[RegNo] = R;
        return R;
    } else {
        return it->second;
    }
}

LabelOperand *GetNewLabelOperand(int LabelNo) {
    auto it = LabelOperandMap.find(LabelNo);
    if (it == LabelOperandMap.end()) {
        auto L = new LabelOperand(LabelNo);
        LabelOperandMap[LabelNo] = L;
        return L;
    } else {
        return it->second;
    }
}

GlobalOperand *GetNewGlobalOperand(std::string name) {
    auto it = GlobalOperandMap.find(name);
    if (it == GlobalOperandMap.end()) {
        auto G = new GlobalOperand(name);
        GlobalOperandMap[name] = G;
        return G;
    } else {
        return it->second;
    }
}

void IRgenArithmeticI32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg) {
    B->InsertInstruction(1, new ArithmeticInstruction(opcode, BasicInstruction::LLVMType::I32, GetNewRegOperand(reg1),
                                                      GetNewRegOperand(reg2), GetNewRegOperand(result_reg)));
}

void IRgenArithmeticF32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg) {
    B->InsertInstruction(1,
                         new ArithmeticInstruction(opcode, BasicInstruction::LLVMType::FLOAT32, GetNewRegOperand(reg1),
                                                   GetNewRegOperand(reg2), GetNewRegOperand(result_reg)));
}

void IRgenArithmeticI32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int reg2, int result_reg) {
    B->InsertInstruction(1, new ArithmeticInstruction(opcode, BasicInstruction::LLVMType::I32, new ImmI32Operand(val1),
                                                      GetNewRegOperand(reg2), GetNewRegOperand(result_reg)));
}

void IRgenArithmeticF32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, int reg2,
                               int result_reg) {
    B->InsertInstruction(1,
                         new ArithmeticInstruction(opcode, BasicInstruction::LLVMType::FLOAT32, new ImmF32Operand(val1),
                                                   GetNewRegOperand(reg2), GetNewRegOperand(result_reg)));
}

void IRgenArithmeticI32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int val2, int result_reg) {
    B->InsertInstruction(1, new ArithmeticInstruction(opcode, BasicInstruction::LLVMType::I32, new ImmI32Operand(val1),
                                                      new ImmI32Operand(val2), GetNewRegOperand(result_reg)));
}

void IRgenArithmeticF32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, float val2,
                              int result_reg) {
    B->InsertInstruction(1,
                         new ArithmeticInstruction(opcode, BasicInstruction::LLVMType::FLOAT32, new ImmF32Operand(val1),
                                                   new ImmF32Operand(val2), GetNewRegOperand(result_reg)));
}

void IRgenIcmp(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int reg2, int result_reg) {
    B->InsertInstruction(1, new IcmpInstruction(BasicInstruction::LLVMType::I32, GetNewRegOperand(reg1),
                                                GetNewRegOperand(reg2), cmp_op, GetNewRegOperand(result_reg)));
}

void IRgenFcmp(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, int reg2, int result_reg) {
    B->InsertInstruction(1, new FcmpInstruction(BasicInstruction::LLVMType::FLOAT32, GetNewRegOperand(reg1),
                                                GetNewRegOperand(reg2), cmp_op, GetNewRegOperand(result_reg)));
}

void IRgenIcmpImmRight(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int val2, int result_reg) {
    B->InsertInstruction(1, new IcmpInstruction(BasicInstruction::LLVMType::I32, GetNewRegOperand(reg1),
                                                new ImmI32Operand(val2), cmp_op, GetNewRegOperand(result_reg)));
}

void IRgenFcmpImmRight(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, float val2, int result_reg) {
    B->InsertInstruction(1, new FcmpInstruction(BasicInstruction::LLVMType::FLOAT32, GetNewRegOperand(reg1),
                                                new ImmF32Operand(val2), cmp_op, GetNewRegOperand(result_reg)));
}

void IRgenFptosi(LLVMBlock B, int src, int dst) {
    B->InsertInstruction(1, new FptosiInstruction(GetNewRegOperand(dst), GetNewRegOperand(src)));
}

void IRgenSitofp(LLVMBlock B, int src, int dst) {
    B->InsertInstruction(1, new SitofpInstruction(GetNewRegOperand(dst), GetNewRegOperand(src)));
}

void IRgenZextI1toI32(LLVMBlock B, int src, int dst) {
    B->InsertInstruction(1, new ZextInstruction(BasicInstruction::LLVMType::I32, GetNewRegOperand(dst),
                                                BasicInstruction::LLVMType::I1, GetNewRegOperand(src)));
}

void IRgenGetElementptrIndexI32(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                        std::vector<int> dims, std::vector<Operand> indexs) {
    B->InsertInstruction(1, new GetElementptrInstruction(type, GetNewRegOperand(result_reg), ptr, dims, indexs, BasicInstruction::I32));
}

void IRgenGetElementptrIndexI64(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                        std::vector<int> dims, std::vector<Operand> indexs) {
    B->InsertInstruction(1, new GetElementptrInstruction(type, GetNewRegOperand(result_reg), ptr, dims, indexs, BasicInstruction::I64));
}

void IRgenLoad(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr) {
    B->InsertInstruction(1, new LoadInstruction(type, ptr, GetNewRegOperand(result_reg)));
}

void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, int value_reg, Operand ptr) {
    B->InsertInstruction(1, new StoreInstruction(type, ptr, GetNewRegOperand(value_reg)));
}

void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, Operand value, Operand ptr) {
    B->InsertInstruction(1, new StoreInstruction(type, ptr, value));
}

void IRgenCall(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg,
               std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name) {
    B->InsertInstruction(1, new CallInstruction(type, GetNewRegOperand(result_reg), name, args));
}

void IRgenCallVoid(LLVMBlock B, BasicInstruction::LLVMType type,
                   std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name) {
    B->InsertInstruction(1, new CallInstruction(type, GetNewRegOperand(-1), name, args));
}

void IRgenCallNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, std::string name) {
    B->InsertInstruction(1, new CallInstruction(type, GetNewRegOperand(result_reg), name));
}

void IRgenCallVoidNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, std::string name) {
    B->InsertInstruction(1, new CallInstruction(type, GetNewRegOperand(-1), name));
}

void IRgenRetReg(LLVMBlock B, BasicInstruction::LLVMType type, int reg) {
    B->InsertInstruction(1, new RetInstruction(type, GetNewRegOperand(reg)));
}

void IRgenRetImmInt(LLVMBlock B, BasicInstruction::LLVMType type, int val) {
    B->InsertInstruction(1, new RetInstruction(type, new ImmI32Operand(val)));
}

void IRgenRetImmFloat(LLVMBlock B, BasicInstruction::LLVMType type, float val) {
    B->InsertInstruction(1, new RetInstruction(type, new ImmF32Operand(val)));
}

void IRgenRetVoid(LLVMBlock B) {
    B->InsertInstruction(1, new RetInstruction(BasicInstruction::LLVMType::VOID, nullptr));
}

void IRgenBRUnCond(LLVMBlock B, int dst_label) {
    B->InsertInstruction(1, new BrUncondInstruction(GetNewLabelOperand(dst_label)));
}

void IRgenBrCond(LLVMBlock B, int cond_reg, int true_label, int false_label) {
    B->InsertInstruction(1, new BrCondInstruction(GetNewRegOperand(cond_reg), GetNewLabelOperand(true_label),
                                                  GetNewLabelOperand(false_label)));
}

void IRgenAlloca(LLVMBlock B, BasicInstruction::LLVMType type, int reg) {
    B->InsertInstruction(0, new AllocaInstruction(type, GetNewRegOperand(reg)));
}

void IRgenAllocaArray(LLVMBlock B, BasicInstruction::LLVMType type, int reg, std::vector<int> dims) {
    B->InsertInstruction(0, new AllocaInstruction(type, dims, GetNewRegOperand(reg)));
}

Operand RegOperand::CopyOperand(){
    return GetNewRegOperand(reg_no);
}

Operand ImmI32Operand::CopyOperand(){
    return new ImmI32Operand(immVal);
}

Operand ImmI64Operand::CopyOperand(){
    return new ImmI64Operand(immVal);
}

Operand ImmF32Operand::CopyOperand(){
    return new ImmF32Operand(immVal);
}

Operand LabelOperand::CopyOperand(){
    return new LabelOperand(label_no);
}

Operand GlobalOperand::CopyOperand(){
    return new GlobalOperand(name);
}

// LoadInstruction
void LoadInstruction::ReplaceRegByMap(const std::map<int, int> &Rule){
    if(pointer->GetOperandType() == BasicOperand::REG){
        auto pointerReg = (RegOperand*)pointer;
        auto pointerRegno = pointerReg->GetRegNo();
        if(Rule.find(pointerRegno) != Rule.end()){
            pointer = GetNewRegOperand(Rule.find(pointerRegno)->second);
        }
    }
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
}

// StoreInstruction
void StoreInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
    if(pointer->GetOperandType() == BasicOperand::REG){
        auto pointerReg = (RegOperand*)pointer;
        auto pointerRegno = pointerReg->GetRegNo();
        if(Rule.find(pointerRegno) != Rule.end()){
            pointer = GetNewRegOperand(Rule.find(pointerRegno)->second);
        }
    }
    if(value->GetOperandType() == BasicOperand::REG){
        auto valueReg = (RegOperand*)value;
        auto valueRegno = valueReg->GetRegNo();
        if(Rule.find(valueRegno) != Rule.end()){
            value = GetNewRegOperand(Rule.find(valueRegno)->second);
        }
    }
}

// ArithmeticInstruction
void ArithmeticInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
    if(op1->GetOperandType() == BasicOperand::REG){
        auto op1Reg = (RegOperand*)op1;
        auto op1Regno = op1Reg->GetRegNo();
        if(Rule.find(op1Regno) != Rule.end()){
            op1 = GetNewRegOperand(Rule.find(op1Regno)->second);
        }
    }
    if(op2->GetOperandType() == BasicOperand::REG){
        auto op2Reg = (RegOperand*)op2;
        auto op2Regno = op2Reg->GetRegNo();
        if(Rule.find(op2Regno) != Rule.end()){
            op2 = GetNewRegOperand(Rule.find(op2Regno)->second);
        }
    }
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
}

// IcmpInstruction
void IcmpInstruction::ReplaceRegByMap(const std::map<int, int> &Rule){
    if(op1->GetOperandType() == BasicOperand::REG){
        auto op1Reg = (RegOperand*)op1;
        auto op1Regno = op1Reg->GetRegNo();
        if(Rule.find(op1Regno) != Rule.end()){
            op1 = GetNewRegOperand(Rule.find(op1Regno)->second);
        }
    }
    if(op2->GetOperandType() == BasicOperand::REG){
        auto op2Reg = (RegOperand*)op2;
        auto op2Regno = op2Reg->GetRegNo();
        if(Rule.find(op2Regno) != Rule.end()){
            op2 = GetNewRegOperand(Rule.find(op2Regno)->second);
        }
    }
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
}


// FcmpInstruction
void FcmpInstruction::ReplaceRegByMap(const std::map<int, int> &Rule){
    if(op1->GetOperandType() == BasicOperand::REG){
        auto op1Reg = (RegOperand*)op1;
        auto op1Regno = op1Reg->GetRegNo();
        if(Rule.find(op1Regno) != Rule.end()){
            op1 = GetNewRegOperand(Rule.find(op1Regno)->second);
        }
    }
    if(op2->GetOperandType() == BasicOperand::REG){
        auto op2Reg = (RegOperand*)op2;
        auto op2Regno = op2Reg->GetRegNo();
        if(Rule.find(op2Regno) != Rule.end()){
            op2 = GetNewRegOperand(Rule.find(op2Regno)->second);
        }
    }
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
}

// PhiInstruction
void PhiInstruction::ReplaceRegByMap(const std::map<int, int> &Rule){
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
    for(auto &pair : phi_list){
        auto &op = pair.second;
        if(op->GetOperandType() == BasicOperand::REG){
            auto opReg = (RegOperand*)op;
            auto opRegno = opReg->GetRegNo();
            if(Rule.find(opRegno) != Rule.end()){
                op = GetNewRegOperand(Rule.find(opRegno)->second);
            }
            
        }
    }
}

void PhiInstruction::ReplaceLabelByMap(const std::map<int, int> &Rule) {
    for(auto &pair : phi_list){
        auto &label = pair.first;
        if(label->GetOperandType() == BasicOperand::LABEL){
            auto labelop = (LabelOperand*)label;
            auto labelopno = labelop->GetLabelNo();
            if(Rule.find(labelopno) != Rule.end()){
                label = GetNewLabelOperand(Rule.find(labelopno)->second);
            }
        }
    }
}

// AllocaInstruction
void AllocaInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
}

// BrCondInstruction
void BrCondInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
	if(cond->GetOperandType() == BasicOperand::REG){
        auto condReg = (RegOperand*)cond;
        auto condRegno = condReg->GetRegNo();
        if(Rule.find(condRegno) != Rule.end()){
            cond = GetNewRegOperand(Rule.find(condRegno)->second);
        }
    }
}

void BrCondInstruction::ReplaceLabelByMap(const std::map<int, int> &Rule) {
    if(trueLabel->GetOperandType() == BasicOperand::LABEL){
        auto trueLabelop = (LabelOperand*)trueLabel;
        auto trueLabelopno = trueLabelop->GetLabelNo();
        if(Rule.find(trueLabelopno) != Rule.end()){
            trueLabel = GetNewLabelOperand(Rule.find(trueLabelopno)->second);
        }
    }
    if(falseLabel->GetOperandType() == BasicOperand::LABEL){
        auto falseLabelop = (LabelOperand*)falseLabel;
        auto falseLabelopno = falseLabelop->GetLabelNo();
        if(Rule.find(falseLabelopno) != Rule.end()){
            falseLabel = GetNewLabelOperand(Rule.find(falseLabelopno)->second);
        }
    }
}

// BrUncondInstruction

void BrUncondInstruction::ReplaceLabelByMap(const std::map<int, int> &Rule) {
	if(destLabel->GetOperandType() == BasicOperand::LABEL){
        auto destLabelop = (LabelOperand*)destLabel;
        auto destLabelopno = destLabelop->GetLabelNo();
        if(Rule.find(destLabelopno) != Rule.end()){
            destLabel = GetNewLabelOperand(Rule.find(destLabelopno)->second);
        }
    }
}

// GlobalVarDefineInstruction
void GlobalVarDefineInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
    if(init_val != nullptr && init_val->GetOperandType() == BasicOperand::REG){
        auto init_valReg = (RegOperand*)init_val;
        auto init_valRegno = init_valReg->GetRegNo();
        if(Rule.find(init_valRegno) != Rule.end()){
            init_val = GetNewRegOperand(Rule.find(init_valRegno)->second);
        }
    }
}

// GlobalStringConstInstruction

// CallInstruction
void CallInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
	if(result != nullptr && result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
    for(auto &[ty, op] : args){
        if(op->GetOperandType() == BasicOperand::REG){
            auto opReg = (RegOperand*)op;
            auto opRegno = opReg->GetRegNo();
            if(Rule.find(opRegno) != Rule.end()){
                op = GetNewRegOperand(Rule.find(opRegno)->second);
            }
        }
    }
}


// RetInstruction
void RetInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
	if(ret_val != nullptr && ret_val->GetOperandType() == BasicOperand::REG){
        auto retReg = (RegOperand*)ret_val;
        auto retRegno = retReg->GetRegNo();
        if(Rule.find(retRegno) != Rule.end()){
            ret_val = GetNewRegOperand(Rule.find(retRegno)->second);
        }
    }
}

// GetElementptrInstruction
void GetElementptrInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
	if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
    if(ptrval->GetOperandType() == BasicOperand::REG){
        
        auto ptrvalReg = (RegOperand*)ptrval;
        auto ptrvalRegno = ptrvalReg->GetRegNo();
        if(Rule.find(ptrvalRegno) != Rule.end()){
            ptrval = GetNewRegOperand(Rule.find(ptrvalRegno)->second);
        }
    }
    for(auto &op : indexes){
        if(op->GetOperandType() == BasicOperand::REG){
            auto opReg = (RegOperand*)op;
            auto opRegno = opReg->GetRegNo();
            if(Rule.find(opRegno) != Rule.end()){
                op = GetNewRegOperand(Rule.find(opRegno)->second);
            }
        }
    }
}

// FunctionDefineInstruction
void FunctionDefineInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
	for(auto &op : formals_reg){
        if(op->GetOperandType() == BasicOperand::REG){
            auto opReg = (RegOperand*)op;
            auto opRegno = opReg->GetRegNo();
            if(Rule.find(opRegno) != Rule.end()){
                op = GetNewRegOperand(Rule.find(opRegno)->second);
            }
        }
    }
}

// FunctionDeclareInstruction

// FptosiInstruction
void FptosiInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
    if(value->GetOperandType() == BasicOperand::REG){
        auto valueReg = (RegOperand*)value;
        auto valueRegno = valueReg->GetRegNo();
        if(Rule.find(valueRegno) != Rule.end()){
            value = GetNewRegOperand(Rule.find(valueRegno)->second);
        }
    }
}

// SitofpInstruction
void SitofpInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
    if(value->GetOperandType() == BasicOperand::REG){
        auto valueReg = (RegOperand*)value;
        auto valueRegno = valueReg->GetRegNo();
        if(Rule.find(valueRegno) != Rule.end()){
            value = GetNewRegOperand(Rule.find(valueRegno)->second);
        }
    }
}

// ZextInstruction
void ZextInstruction::ReplaceRegByMap(const std::map<int, int> &Rule) {
    if(result->GetOperandType() == BasicOperand::REG){
        auto resultReg = (RegOperand*)result;
        auto resultRegno = resultReg->GetRegNo();
        if(Rule.find(resultRegno) != Rule.end()){
            result = GetNewRegOperand(Rule.find(resultRegno)->second);
        }
    }
    if(value->GetOperandType() == BasicOperand::REG){
        auto valueReg = (RegOperand*)value;
        auto valueRegno = valueReg->GetRegNo();
        if(Rule.find(valueRegno) != Rule.end()){
            value = GetNewRegOperand(Rule.find(valueRegno)->second);
        }
    }
}

std::vector<Operand> LoadInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(pointer);
    return vec;
}

std::vector<Operand> StoreInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(pointer);
    vec.push_back(value);
    return vec;
}

std::vector<Operand> ArithmeticInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(op1);
    vec.push_back(op2);
    return vec;
}

std::vector<Operand> IcmpInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(op1);
    vec.push_back(op2);
    return vec;
}

std::vector<Operand> FcmpInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(op1);
    vec.push_back(op2);
    return vec;
}

std::vector<Operand> PhiInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    for(auto [labelop, valop] : phi_list){
        vec.push_back(valop);
    }
    return vec;
}

std::vector<Operand> AllocaInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    return vec;
}

std::vector<Operand> BrCondInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(cond);
    vec.push_back(trueLabel);
    vec.push_back(falseLabel);
    return vec;
}

std::vector<Operand> BrUncondInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(destLabel);
    return vec;
}

std::vector<Operand> GlobalVarDefineInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    if(init_val != nullptr){
        vec.push_back(init_val);
    }
    return vec;
}

std::vector<Operand> GlobalStringConstInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    return vec;
}

std::vector<Operand> CallInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    for(auto [tp, op] : args){
        vec.push_back(op);
    }
    return vec;
}

std::vector<Operand> RetInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    if(ret_val != nullptr){
        vec.push_back(ret_val);
    }
    return vec;
}

std::vector<Operand> GetElementptrInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    for(auto op : indexes){
        vec.push_back(op);
    }
    vec.push_back(ptrval);
    return vec;
}

std::vector<Operand> FunctionDefineInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    for(auto op : formals_reg){
        vec.push_back(op);
    }
    return vec;
}

std::vector<Operand> FunctionDeclareInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    return vec;
}

std::vector<Operand> FptosiInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(value);
    return vec;
}

std::vector<Operand> SitofpInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(value);
    return vec;
}

std::vector<Operand> ZextInstruction::GetNonResultOperands(){
    std::vector<Operand> vec;
    vec.push_back(value);
    return vec;
}

void PhiInstruction::SetNewLabelFrom(int old_id, int new_id){
    for (auto &pair : phi_list) {
        if (((LabelOperand *)pair.first)->GetLabelNo() == old_id) {
            pair.first = GetNewLabelOperand(new_id);
            return;
        }
    }
}


Instruction LoadInstruction::CopyInstruction() {
    Operand npointer = pointer->CopyOperand();
    Operand nresult = result->CopyOperand();
    return new LoadInstruction(type, npointer, nresult);
}

Instruction StoreInstruction::CopyInstruction() {
    Operand npointer = pointer->CopyOperand();
    Operand nvalue = value->CopyOperand();
    return new StoreInstruction(type, npointer, nvalue);
}

Instruction ArithmeticInstruction::CopyInstruction() {

    Operand nop1 = op1->CopyOperand();
    Operand nop2 = op2->CopyOperand();
    Operand nresult = result->CopyOperand();
    return new ArithmeticInstruction(opcode, type, nop1, nop2, nresult);
}

Instruction IcmpInstruction::CopyInstruction() {
    Operand nop1 = op1->CopyOperand();
    Operand nop2 = op2->CopyOperand();
    Operand nresult = result->CopyOperand();
    return new IcmpInstruction(type, nop1, nop2, cond, nresult);
}

Instruction FcmpInstruction::CopyInstruction() {
    Operand nop1 = op1->CopyOperand();
    Operand nop2 = op2->CopyOperand();
    Operand nresult = result->CopyOperand();
    return new FcmpInstruction(type, nop1, nop2, cond, nresult);
}

Instruction PhiInstruction::CopyInstruction() {
    Operand nresult = result->CopyOperand();
    std::vector<std::pair<Operand, Operand>> nval_labels;
    // std::map<operand,operand> nval_labels;

    for (auto Phiop : phi_list) {
        Operand nlabel = Phiop.first->CopyOperand();
        Operand nvalue = Phiop.second->CopyOperand();
        // nval_labels.push_back({nlabel,nvalue});
        nval_labels.push_back(std::make_pair(nlabel, nvalue));
    }

    return new PhiInstruction(type, nresult, nval_labels);
}

Instruction AllocaInstruction::CopyInstruction() {
    Operand nresult = result->CopyOperand();
    std::vector<int> ndims;
    for (auto dimint : dims) {
        ndims.push_back(dimint);
    }
    return new AllocaInstruction(type, ndims, nresult);
}

Instruction BrCondInstruction::CopyInstruction() {
    Operand ncond = cond->CopyOperand();
    Operand ntrueLabel = trueLabel->CopyOperand();
    Operand nfalseLabel = falseLabel->CopyOperand();
    return new BrCondInstruction(ncond, ntrueLabel, nfalseLabel);
}

Instruction BrUncondInstruction::CopyInstruction() {
    Operand ndestLabel = destLabel->CopyOperand();
    return new BrUncondInstruction(ndestLabel);
}

Instruction CallInstruction::CopyInstruction() {
    Operand nresult = NULL;
    if (ret_type != VOID) {
        nresult = result->CopyOperand();
    }
    std::vector<std::pair<enum LLVMType, Operand>> nargs;
    for (auto n : args) {
        nargs.push_back({n.first, n.second->CopyOperand()});
    }
    return new CallInstruction(ret_type, nresult, name, nargs);
}

Instruction RetInstruction::CopyInstruction() { return new RetInstruction(ret_type, ret_val); }

Instruction GetElementptrInstruction::CopyInstruction() {
    auto type = index_type;
    Operand nresult = result->CopyOperand();
    Operand nptrval = ptrval->CopyOperand();
    std::vector<Operand> nindexes;
    for (auto index : indexes) {
        Operand nindex = index->CopyOperand();
        nindexes.push_back(nindex);
    }

    return new GetElementptrInstruction(type, nresult, nptrval, dims, nindexes, type);
}

Instruction FptosiInstruction::CopyInstruction() {
    Operand nresult = result->CopyOperand();
    Operand nvalue = value->CopyOperand();

    return new FptosiInstruction(nresult, nvalue);
}

Instruction SitofpInstruction::CopyInstruction() {
    Operand nresult = result->CopyOperand();
    Operand nvalue = value->CopyOperand();

    return new SitofpInstruction(nresult, nvalue);
}

Instruction ZextInstruction::CopyInstruction() {
    Operand nresult = result->CopyOperand();
    Operand nvalue = value->CopyOperand();

    return new ZextInstruction(to_type, nresult, from_type, nvalue);
}

void LoadInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    pointer = opvec[0];
}

void StoreInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    pointer = opvec[0];
    value = opvec[1];
}

void ArithmeticInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    op1 = opvec[0];
    op2 = opvec[1];
}

void IcmpInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    op1 = opvec[0];
    op2 = opvec[1];
}

void FcmpInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    op1 = opvec[0];
    op2 = opvec[1];
}

void PhiInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    int i = 0;
    for (auto &pair : phi_list) {
        pair.second = opvec[i];
        i++;
    }
}

void BrCondInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    cond = opvec[0];
}

void CallInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    int i = 0;
    for (auto &pair : args) {
        pair.second = opvec[i];
        i++;
    }
}

void RetInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    if(!opvec.empty()){
        ret_val = opvec[0];
    }
}

void GetElementptrInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    int i = 0;
    for(auto &op : indexes){
        op = opvec[i++];
    }
    ptrval = opvec[opvec.size() - 1];
}

void FptosiInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    value = opvec[0];
}

void SitofpInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    value = opvec[0];
}

void ZextInstruction::SetNonResultOperands(std::vector<Operand> opvec){
    value = opvec[0];
}


BasicInstruction::LLVMIROpcode mapToLLVMOpcodeInt(OpType::Op kind) {
    switch (kind) {
        case OpType::Op::Add:  return BasicInstruction::LLVMIROpcode::ADD;
        case OpType::Op::Sub:  return BasicInstruction::LLVMIROpcode::SUB;
        case OpType::Op::Mul:  return BasicInstruction::LLVMIROpcode::MUL;
        case OpType::Op::Div:  return BasicInstruction::LLVMIROpcode::DIV;
        default: throw std::runtime_error("Unknown BinaryOpKind");
    }
}


BasicInstruction::LLVMIROpcode mapToLLVMOpcodeFloat(OpType::Op kind) {
    switch (kind) {
        case OpType::Op::Add:  return BasicInstruction::LLVMIROpcode::FADD;
        case OpType::Op::Sub:  return BasicInstruction::LLVMIROpcode::FSUB;
        case OpType::Op::Mul:  return BasicInstruction::LLVMIROpcode::FMUL;
        case OpType::Op::Div:  return BasicInstruction::LLVMIROpcode::FDIV;
        default: throw std::runtime_error("Unknown BinaryOpKind");
    }
}


BasicInstruction::IcmpCond mapToIcmpCond(OpType::Op kind) {
    switch (kind) {
        case OpType::Op::Eq : return BasicInstruction::IcmpCond::eq;
        case OpType::Op::Neq: return BasicInstruction::IcmpCond::ne;
        case OpType::Op::Lt : return BasicInstruction::IcmpCond::slt;
        case OpType::Op::Le : return BasicInstruction::IcmpCond::sle;
        case OpType::Op::Gt : return BasicInstruction::IcmpCond::sgt;
        case OpType::Op::Ge : return BasicInstruction::IcmpCond::sge;
        default:
            throw std::runtime_error("Invalid IcmpCond kind");
    }
}

BasicInstruction::FcmpCond mapToFcmpCond(OpType::Op kind) {
    switch (kind) {
        case OpType::Op::Eq : return BasicInstruction::FcmpCond::OEQ;
        case OpType::Op::Neq: return BasicInstruction::FcmpCond::ONE;
        case OpType::Op::Lt : return BasicInstruction::FcmpCond::OLT;
        case OpType::Op::Le : return BasicInstruction::FcmpCond::OLE;
        case OpType::Op::Gt : return BasicInstruction::FcmpCond::OGT;
        case OpType::Op::Ge : return BasicInstruction::FcmpCond::OGE;
        default:
            throw std::runtime_error("Invalid FcmpCond kind");
    }
}

