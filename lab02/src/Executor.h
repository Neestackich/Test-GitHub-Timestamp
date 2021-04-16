
#ifndef RISCV_SIM_EXECUTOR_H
#define RISCV_SIM_EXECUTOR_H

#include "Instruction.h"

class Executor
{
public:
    void performComputation(InstructionPtr& instr) {
        if (instr->_src1 && (instr->_imm || instr->_src2)) {
            Word secOprnd = instr->_imm ? instr->_imm.value() : instr->_src2Val;
            Word aluResult = performAluOperation(instr->_src1Val, secOprnd , instr->_aluFunc);
            if (instr->_type == IType::Ld) {
                instr->_addr = aluResult;
            } else if (instr->_type == IType::St) {
                instr->_addr = aluResult;
                instr->_data = instr->_src2Val;
            } else if (instr->_type == IType::Alu || instr->_type == IType::Unsupported || instr->_type == IType::Br ) {
                instr->_data = aluResult;
            }
        }
    }
    void setNextIp(InstructionPtr& instr, Word ip) {
        if ((instr->_src1 && instr->_src2) || instr->_brFunc == BrFunc::AT || instr->_brFunc == BrFunc::NT) {
            if(isBrunchFunction(instr->_src1Val, instr->_src2Val, instr->_brFunc)) {
                if (instr->_type == IType::Br || instr->_type == IType::J || instr->_type == IType::Auipc ) {
                    instr->_nextIp = ip + instr->_imm.value();
                } else if (instr->_type == IType::Alu) {
                    instr->_nextIp = ip + 4;
                } else if (instr->_type == IType::Jr) {
                    instr->_nextIp = instr->_imm.value() + instr->_src1Val;
                } else if (instr->_type == IType::Csrr)  {
                    instr->_nextIp = instr->_csrVal;
                } else if (instr->_type == IType::St)  {
                    instr->_nextIp = instr->_src2Val;
                } else if (instr->_type == IType::Csrw)  {
                    instr->_nextIp = instr->_src1Val;
                }
            } else {
                instr->_nextIp = ip + 4;
            }
        } else {
            instr->_nextIp = ip + 4;
        }
    }
    Word performAluOperation(Word A, Word B, AluFunc aluFunction) {
        if (aluFunction == AluFunc::Or) { 
            return A | B;
        } else if (aluFunction == AluFunc::And) { 
            return A & B;
        } else if (aluFunction == AluFunc::Xor) { 
            return A ^ B;
        } else if (aluFunction == AluFunc::Sub) { 
            return A - B;
        } else if (aluFunction == AluFunc::Add) {
            return A + B;
        } else if (aluFunction == AluFunc::Slt) { 
            return static_cast<SignedWord>(A) < static_cast<SignedWord>(B);
        } else if (aluFunction == AluFunc::Sltu) { 
            return A < B;
        } else if (aluFunction == AluFunc::Srl) { 
            return A >> (B % 32);
        } else if (aluFunction == AluFunc::Sll) { 
            return A << (B % 32);
        } else if (aluFunction == AluFunc::Sra) { 
            return static_cast<SignedWord>(A) >> (B % 32);
        }
        return 0;
    }
    bool isBrunchFunction(Word A, Word B, BrFunc branchFunction) {
        if (branchFunction == BrFunc::Eq) { 
            return A == B;
        } else if (branchFunction == BrFunc::Neq) {
            return A != B;
        } else if (branchFunction == BrFunc::Lt) {
            return static_cast<SignedWord>(A) < static_cast<SignedWord>(B);
        } else if (branchFunction == BrFunc::Ltu) { 
            return A < B;
        } else if (branchFunction == BrFunc::Ge) { 
            return static_cast<SignedWord>(A) >= static_cast<SignedWord>(B);
        } else if (branchFunction == BrFunc::Geu) {
            return A >= B;
        } else if (branchFunction == BrFunc::NT) {
            return false;
        } else if (branchFunction == BrFunc::AT) {
            return true;
        }
        return false;
    }
    void Execute(InstructionPtr& instr, Word ip) {
        performComputation(instr);
        if (instr->_type == IType::St) {
            instr->_data = instr->_src2Val;
        } else if (instr->_type == IType::Jr || instr->_type == IType::J) { 
            instr->_data = ip + 4;
        } else if (instr->_type == IType::Auipc) { 
            instr->_data = ip + instr->_imm.value();
        } else if (instr->_type == IType::Csrw) { 
            instr->_data = instr->_src1Val;
        } else if (instr->_type == IType::Csrr) {
            instr->_data = instr->_csrVal;
        }
        setNextIp(instr, ip);        
    }
};

#endif // RISCV_SIM_EXECUTOR_H
