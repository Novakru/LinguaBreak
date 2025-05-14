#include"phi_process.h"
std::vector<Register *> MachinePhiInstruction::GetReadReg() {
    std::vector<Register *> ret;
    for (auto [label, op] : phi_list) {
        if (op->op_type == MachineBaseOperand::REG) {
            ret.push_back(&(((MachineRegister *)op)->reg));
        }
    }
    return ret;
}
std::vector<Register *> MachinePhiInstruction::GetWriteReg() { return std::vector<Register *>({&result}); }