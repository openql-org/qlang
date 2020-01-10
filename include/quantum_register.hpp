#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "error.hpp"
#include "lexer.hpp"
#include "llvm_table.hpp"
#include "token.hpp"

namespace qlang {
enum class QuantumRegister {
  q0 = 0,   q1,   q2,   q3,   q4,   q5,   q6,   q7,
  q8,   q9,   q10,  q11,  q12,  q13,  q14,  q15,
  q16,  q17,  q18,  q19,  q20,  q21,  q22,  q23,
  q24,  q25,  q26,  q27,  q28,  q29,  q30,  q31
};


static std::ostream &operator<<(std::ostream &out, const QuantumRegister reg) {
  switch (reg) {
  case QuantumRegister::q0:
    return out << "q0";
  case QuantumRegister::q1:
    return out << "q1";
  case QuantumRegister::q2:
    return out << "q2";
  case QuantumRegister::q3:
    return out << "q3";
  case QuantumRegister::q4:
    return out << "q4";
  case QuantumRegister::q5:
    return out << "q5";
  case QuantumRegister::q6:
    return out << "q6";
  case QuantumRegister::q7:
    return out << "q7";
  case QuantumRegister::q8:
    return out << "q8";
  case QuantumRegister::q9:
    return out << "q9";
  case QuantumRegister::q10:
    return out << "q10";
  case QuantumRegister::q11:
    return out << "q11";
  case QuantumRegister::q12:
    return out << "q12";
  case QuantumRegister::q13:
    return out << "q13";
  case QuantumRegister::q14:
    return out << "q14";
  case QuantumRegister::q15:
    return out << "q15";
  case QuantumRegister::q16:
    return out << "q16";
  case QuantumRegister::q17:
    return out << "q17";
  case QuantumRegister::q18:
    return out << "q18";
  case QuantumRegister::q19:
    return out << "q19";
  case QuantumRegister::q20:
    return out << "q20";
  case QuantumRegister::q21:
    return out << "q21";
  case QuantumRegister::q22:
    return out << "q22";
  case QuantumRegister::q23:
    return out << "q23";
  case QuantumRegister::q24:
    return out << "q24";
  case QuantumRegister::q25:
    return out << "q25";
  case QuantumRegister::q26:
    return out << "q26";
  case QuantumRegister::q27:
    return out << "q27";
  case QuantumRegister::q28:
    return out << "q28";
  case QuantumRegister::q29:
    return out << "q29";
  case QuantumRegister::q30:
    return out << "q30";
  case QuantumRegister::q31:
    return out << "q31";
  }
}


}
