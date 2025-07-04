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


void LoadInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(pointer->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)pointer)->GetRegNo();
        if(store_map.find(regno)!=store_map.end()){
            int new_regno = store_map.at(regno);
            pointer = GetNewRegOperand(new_regno);
        }
    }
}

void StoreInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(value->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)value)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            value = GetNewRegOperand(new_regno);
        }
    }
}

void ArithmeticInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(op1->GetOperandType()==BasicOperand::REG){
        int regno1 = ((RegOperand*)op1)->GetRegNo();
        if(use_map.find(regno1)!=use_map.end()){
            int new_regno1 = use_map.at(regno1);
            op1 = GetNewRegOperand(new_regno1);
        }
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        int regno2 = ((RegOperand*)op2)->GetRegNo();
        if(use_map.find(regno2)!=use_map.end()){
            int new_regno2 = use_map.at(regno2);
            op2 = GetNewRegOperand(new_regno2);
        }
    }
}

void IcmpInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(op1->GetOperandType()==BasicOperand::REG){
        int regno1 = ((RegOperand*)op1)->GetRegNo();
        if(use_map.find(regno1)!=use_map.end()){
            int new_regno1 = use_map.at(regno1);
            op1 = GetNewRegOperand(new_regno1);
        }
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        int regno2 = ((RegOperand*)op2)->GetRegNo();
        if(use_map.find(regno2)!=use_map.end()){
            int new_regno2 = use_map.at(regno2);
            op2 = GetNewRegOperand(new_regno2);
        }
    }
}

void FcmpInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(op1->GetOperandType()==BasicOperand::REG){
        int regno1 = ((RegOperand*)op1)->GetRegNo();
        if(use_map.find(regno1)!=use_map.end()){
            int new_regno1 = use_map.at(regno1);
            op1 = GetNewRegOperand(new_regno1);
        }
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        int regno2 = ((RegOperand*)op2)->GetRegNo();
        if(use_map.find(regno2)!=use_map.end()){
            int new_regno2 = use_map.at(regno2);
            op2 = GetNewRegOperand(new_regno2);
        }
    }
}

void PhiInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    
}

void AllocaInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void BrCondInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(cond->GetOperandType()==BasicOperand::REG){    
        int regno = ((RegOperand*)cond)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            cond = GetNewRegOperand(new_regno);
        }
    }
}

void BrUncondInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void GlobalVarDefineInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void GlobalStringConstInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void CallInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    for(auto &arg: args){
        if(arg.second->GetOperandType()==BasicOperand::REG){
            int regno = ((RegOperand*)arg.second)->GetRegNo();
            if(use_map.find(regno)!=use_map.end()){
                int new_regno = use_map.at(regno);
                arg.second = GetNewRegOperand(new_regno);
            }
        }
    }
}

void RetInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(ret_type==BasicInstruction::LLVMType::VOID) return;
    if(ret_val->GetOperandType()==BasicOperand::REG){        
        int regno = ((RegOperand*)ret_val)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            ret_val = GetNewRegOperand(new_regno);
        }
    }
}

void GetElementptrInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    for(int i=0; i<indexes.size(); i++){
        if(indexes[i]->GetOperandType()==BasicOperand::REG){
            int regno = ((RegOperand*)indexes[i])->GetRegNo();
            if(use_map.find(regno)!=use_map.end()){
                int new_regno = use_map.at(regno);
                indexes[i] = GetNewRegOperand(new_regno);
            }
        }
    }
}

void FunctionDefineInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void FunctionDeclareInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void FptosiInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(value->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)value)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            value = GetNewRegOperand(new_regno);
        }
    }
}

void SitofpInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(value->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)value)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            value = GetNewRegOperand(new_regno);
        }
    }
}

void ZextInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(value->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)value)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            value = GetNewRegOperand(new_regno);
        }
    }
}

// 下面是获取指令结果寄存器的函数定义
int LoadInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int StoreInstruction::GetDefRegno(){
    if(pointer->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)pointer)->GetRegNo();
    }
    return -1;
}

int ArithmeticInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int IcmpInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int FcmpInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int PhiInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int AllocaInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}


int BrCondInstruction::GetDefRegno(){
    return -1;
}


int BrUncondInstruction::GetDefRegno(){
    return -1;
}


int GlobalVarDefineInstruction::GetDefRegno(){
    return -1;
}


int GlobalStringConstInstruction::GetDefRegno(){
    return -1;
}


int CallInstruction::GetDefRegno(){
    if(result!=nullptr&&result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int RetInstruction::GetDefRegno(){
    return -1;
}

int GetElementptrInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int FunctionDefineInstruction::GetDefRegno(){
    return -1;
}

int FunctionDeclareInstruction::GetDefRegno(){
    return -1;
}

int FptosiInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int SitofpInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int ZextInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}



std::set<int> LoadInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(pointer->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)pointer)->GetRegNo());
    }
    return regno_set;
}

std::set<int> StoreInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(value->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)value)->GetRegNo());
    }
    if(pointer->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)pointer)->GetRegNo());
    }
    return regno_set;
}

std::set<int> ArithmeticInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(op1->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op1)->GetRegNo());
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op2)->GetRegNo());
    }
    return regno_set;
}

std::set<int> IcmpInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(op1->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op1)->GetRegNo());
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op2)->GetRegNo());
    }
    return regno_set;
}

std::set<int> FcmpInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(op1->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op1)->GetRegNo());
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op2)->GetRegNo());
    }
    return regno_set;
}

std::set<int> PhiInstruction::GetUseRegno(){
    std::set<int> regno_set;
    for(auto& phi_pair: phi_list){
        if(phi_pair.second->GetOperandType()==BasicOperand::REG){
            regno_set.insert(((RegOperand*)phi_pair.second)->GetRegNo());
        }
    }
    return regno_set;
}

std::set<int> AllocaInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> BrCondInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(cond->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)cond)->GetRegNo());
    }
    return regno_set;
}

std::set<int> BrUncondInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> GlobalVarDefineInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> GlobalStringConstInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> CallInstruction::GetUseRegno(){
    std::set<int> regno_set;
    for(int i=0; i<args.size(); i++){
        if(args[i].second->GetOperandType()==BasicOperand::REG){
            regno_set.insert(((RegOperand*)args[i].second)->GetRegNo());
        }
    }
    return regno_set;
}

std::set<int> RetInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(ret_val==nullptr)
        return regno_set;
    if(ret_val->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)ret_val)->GetRegNo());
    }
    return regno_set;
}

std::set<int> GetElementptrInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(ptrval->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)ptrval)->GetRegNo());
    }
    for(int i=0; i<indexes.size(); i++){
        if(indexes[i]->GetOperandType()==BasicOperand::REG){
            regno_set.insert(((RegOperand*)indexes[i])->GetRegNo());
        }
    }
    return regno_set;
}

std::set<int> FunctionDefineInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> FunctionDeclareInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> FptosiInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(value->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)value)->GetRegNo());
    }
    return regno_set;
}

std::set<int> SitofpInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(value->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)value)->GetRegNo());
    }
    return regno_set;
}

std::set<int> ZextInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(value->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)value)->GetRegNo());
    }
    return regno_set;
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

int IcmpInstruction::CompConst(int value1, int value2) {
        switch (cond) {
            case IcmpCond::eq:
                return value1 == value2;
            case IcmpCond::ne:
                return value1 != value2;
            case IcmpCond::ugt:
                return (uint32_t)value1 > (uint32_t)value2;
            case IcmpCond::uge:
                return (uint32_t)value1 >= (uint32_t)value2;
            case IcmpCond::ult:
                return (uint32_t)value1 < (uint32_t)value2;
            case IcmpCond::ule:
                return (uint32_t)value1 <= (uint32_t)value2;
            case IcmpCond::sgt:
                return value1 > value2;
            case IcmpCond::sge:
                return value1 >= value2;
            case IcmpCond::slt:
                return value1 <value2;
            case IcmpCond::sle:
                return value1 <= value2;
        }
        return 0;
    }

float FcmpInstruction::CompConst(float value1, float value2) {
        assert(cond!= FcmpCond::FALSE&&cond!= FcmpCond::TRUE);
        switch (cond) {
            case FcmpCond::OEQ:
                return value1 == value2;
            case FcmpCond::OGT:
                return value1 > value2;
            case FcmpCond::OGE:
                return value1 >= value2;
            case FcmpCond::OLT:
                return value1 < value2;
            case FcmpCond::OLE:
                return value1 <= value2;
            case FcmpCond::ONE:
                return value1 != value2;
            case FcmpCond::ORD:
                return 0;
            case FcmpCond::UEQ:
                return 0;
            case FcmpCond::UGT:
                return 0;
            case FcmpCond::UGE:
                return 0;
            case FcmpCond::ULT:
                return 0;
            case FcmpCond::ULE:
                return 0;
            case FcmpCond::UNE:
                return 0;
            case FcmpCond::UNO:
                return 0;
            default:
                assert(0);
        }
        return 0.0f;
    }

int ArithmeticInstruction::CompConst(int value1, int value2){
    switch(opcode){
        case LLVMIROpcode::ADD:
            return value1 + value2;
        case LLVMIROpcode::SUB:
            return value1 - value2;
        case LLVMIROpcode::MUL:
            return value1 * value2;
        case LLVMIROpcode::DIV:
            assert(value2 != 0);
            return value1 / value2;
        case LLVMIROpcode::MOD:
            assert(value2 != 0);
            return value1 % value2;
        default:
            assert(0);
            return -1;
    }
}
float ArithmeticInstruction::CompConst(float value1, float value2){
    switch(opcode){
        case LLVMIROpcode::FADD:
            return value1 + value2;
        case LLVMIROpcode::FSUB:
            return value1 - value2;
        case LLVMIROpcode::FMUL:
            return value1 * value2;
        case LLVMIROpcode::FDIV:
            assert(value2 != 0);
            return value1 / value2;
        default:
            assert(0);
            return -1;
    }
}

bool PhiInstruction::NotEqual(Operand op1, Operand op2){
    if(op1->GetOperandType()!=op2->GetOperandType()){
        return true;
    }
    if(op1->GetOperandType()==BasicOperand::REG){
        int regno1=((RegOperand*)op1)->GetRegNo();
        int regno2=((RegOperand*)op2)->GetRegNo();
        if(regno1!=regno2){
            return true;
        }
        return false;
    }else if(op1->GetOperandType()==BasicOperand::IMMI32){
        int intval1=((ImmI32Operand*)op1)->GetIntImmVal();
        int intval2=((ImmI32Operand*)op2)->GetIntImmVal();
        if(intval1!=intval2){
            return true;
        }
        return false;
    }else if(op1->GetOperandType()==BasicOperand::IMMF32){
        int floatval1=((ImmF32Operand*)op1)->GetFloatVal();
        int floatval2=((ImmF32Operand*)op2)->GetFloatVal();
        if(floatval1!=floatval2){
            return true;
        }
        return false;
    }
    return false;
}

Operand RegOperand::OperandClone(){
    return GetNewRegOperand(reg_no);
}
Operand ImmI32Operand::OperandClone(){
    return new ImmI32Operand(immVal);
}
Operand ImmI64Operand::OperandClone(){
    return new ImmI64Operand(immVal);
}
Operand ImmF32Operand::OperandClone(){
    return new ImmF32Operand(immVal);
}
Operand LabelOperand::OperandClone(){
    return GetNewLabelOperand(label_no); 
}
Operand GlobalOperand::OperandClone(){
    return GetNewGlobalOperand(name);
}

Instruction LoadInstruction::InstructionClone(){
    Instruction new_inst = new LoadInstruction(type, pointer->OperandClone(), result->OperandClone());
    return new_inst;
}
Instruction StoreInstruction::InstructionClone(){
    Instruction new_inst = new StoreInstruction(type, pointer->OperandClone(),value->OperandClone());
    return new_inst;
}
Instruction ArithmeticInstruction::InstructionClone(){
    Instruction new_inst = new ArithmeticInstruction(opcode, type, op1->OperandClone(), op2->OperandClone(), result->OperandClone());
    return new_inst;
}
Instruction IcmpInstruction::InstructionClone(){
    Instruction new_inst = new IcmpInstruction(type, op1->OperandClone(), op2->OperandClone(), cond, result->OperandClone());
    return new_inst;
}
Instruction FcmpInstruction::InstructionClone(){
    Instruction new_inst = new FcmpInstruction(type, op1->OperandClone(), op2->OperandClone(), cond, result->OperandClone());
    return new_inst;
}
Instruction PhiInstruction::InstructionClone(){
    Instruction new_inst = new PhiInstruction(type, result->OperandClone(), phi_list);
    return new_inst;
}
Instruction AllocaInstruction::InstructionClone(){
    std::vector<int>new_dims;
    new_dims.reserve(dims.size());
    for(auto &dim:dims){
        new_dims.emplace_back(dim);
    }
    Instruction new_inst = new AllocaInstruction(type, new_dims,result->OperandClone());
    return new_inst;
}   
Instruction BrCondInstruction::InstructionClone(){
    Instruction new_inst = new BrCondInstruction(cond->OperandClone(), trueLabel->OperandClone(), falseLabel->OperandClone());
    return new_inst;
}
Instruction BrUncondInstruction::InstructionClone(){
    Instruction new_inst = new BrUncondInstruction(destLabel->OperandClone());
    return new_inst;
}
Instruction CallInstruction::InstructionClone(){
    Instruction new_inst;
    if(result==nullptr){
        new_inst = new CallInstruction(ret_type, nullptr, name, args);
    }else{
        new_inst = new CallInstruction(ret_type, result->OperandClone(), name, args);
    }
    return new_inst;
}
Instruction RetInstruction::InstructionClone(){
    Instruction new_inst;
    if(ret_val==nullptr){
        new_inst = new RetInstruction(ret_type, nullptr);
    }else{
        new_inst = new RetInstruction(ret_type, ret_val->OperandClone());
    }
    return new_inst;
}
Instruction GetElementptrInstruction::InstructionClone(){
    Instruction new_inst = new GetElementptrInstruction(type, result->OperandClone(), ptrval->OperandClone(), dims, indexes, index_type);
    return new_inst;
}

Instruction FptosiInstruction::InstructionClone(){
    Instruction new_inst = new FptosiInstruction(result->OperandClone(), value->OperandClone());
    return new_inst;
}
Instruction SitofpInstruction::InstructionClone(){
    Instruction new_inst = new SitofpInstruction(result->OperandClone(), value->OperandClone());
    return new_inst;
}
Instruction ZextInstruction::InstructionClone(){
    Instruction new_inst = new ZextInstruction(to_type, result->OperandClone(),from_type, value->OperandClone());
    return new_inst;
}