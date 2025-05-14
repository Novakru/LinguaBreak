#ifndef PHI_HELP_H
#define PHI_HELP_H
#include"../machine_instruction.h"

struct MachineRegister : public MachineBaseOperand {
    Register reg;
    MachineRegister(Register reg) : MachineBaseOperand(MachineBaseOperand::REG), reg(reg) {}
    std::string toString() {
        if (reg.is_virtual)
            return "%" + std::to_string(reg.reg_no);
        else
            return "phy_" + std::to_string(reg.reg_no);
    }
};

struct MachineImmediateInt : public MachineBaseOperand {
    int imm32;
    MachineImmediateInt(int imm32) : MachineBaseOperand(MachineBaseOperand::IMMI), imm32(imm32) {}
    std::string toString() { return std::to_string(imm32); }
};
struct MachineImmediateFloat : public MachineBaseOperand {
    float fimm32;
    MachineImmediateFloat(float fimm32) : MachineBaseOperand(MachineBaseOperand::IMMF), fimm32(fimm32) {}
    std::string toString() { return std::to_string(fimm32); }
};
struct MachineImmediateDouble : public MachineBaseOperand {
    double dimm64;
    MachineImmediateDouble(double dimm64) : MachineBaseOperand(MachineBaseOperand::IMMD), dimm64(dimm64) {}
    std::string toString() { return std::to_string(dimm64); }
};
#endif