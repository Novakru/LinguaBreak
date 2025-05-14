#ifndef PHI_PROCESS_H
#define PHI_PROCESS_H
#include"../basic/machine.h"
#include"../machine_instruction.h"
class MachinePhiInstruction : public MachineBaseInstruction {
private:
    Register result;
    std::vector<std::pair<int, MachineBaseOperand *>> phi_list;

public:
    std::vector<Register *> GetReadReg();
    std::vector<Register *> GetWriteReg();

    MachinePhiInstruction(Register result) : result(result), MachineBaseInstruction(MachineBaseInstruction::PHI) {}
    Register GetResult() { return result; }
    void SetResult(Register result) { this->result = result; }
    std::vector<std::pair<int, MachineBaseOperand *>> &GetPhiList() { return phi_list; }
    MachineBaseOperand *removePhiList(int label) {
        for (auto it = phi_list.begin(); it != phi_list.end(); ++it) {
            if (it->first == label) {
                auto ret = it->second;
                phi_list.erase(it);
                return ret;
            }
        }
        return nullptr;
    }
    void pushPhiList(int label, Register reg) { phi_list.push_back(std::make_pair(label, new MachineRegister(reg))); }
    void pushPhiList(int label, int imm32) {
        phi_list.push_back(std::make_pair(label, new MachineImmediateInt(imm32)));
    }
    void pushPhiList(int label, float immf32) {
        phi_list.push_back(std::make_pair(label, new MachineImmediateFloat(immf32)));
    }
    void pushPhiList(int label, MachineBaseOperand *op) { phi_list.push_back(std::make_pair(label, op)); }
    int GetLatency() { return 0; }
};
//PHI指令销毁PASS
class MachinePhiDestruction {
private:
    void PhiDestructionInCurrentFunction();
    MachineUnit *unit;
public:
    void Execute();
    MachinePhiDestruction(MachineUnit *unit) : unit(unit) {}
};
#endif