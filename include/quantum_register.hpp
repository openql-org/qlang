#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "error.hpp"
#include "lexer.hpp"
#include "llvm_table.hpp"
#include "token.hpp"

namespace qlang {
enum QuantumRegister {
  q0 = 0,   q1,   q2,   q3,   q4,   q5,   q6,   q7,
  q8,   q9,   q10,  q11,  q12,  q13,  q14,  q15,
  q16,  q17,  q18,  q19,  q20,  q21,  q22,  q23,
  q24,  q25,  q26,  q27,  q28,  q29,  q30,  q31
};

enum QuantumRegisterABI {
  qzero, qt1, qt2,  qt3,  qt4, qt5, qt6,  qt7,
  qs0, qs1, qa0,  qa1,  qa2, qa3, qa4,  qa5,
  qa6, qa7, qs2,  qs3,  qs4, qs5, qs6,  qs7,
  qs8, qs9, qs10, qs11, qt8, qt9, qt10, qt11
};

static QuantumRegister& operator++(QuantumRegister& r) {
    r = static_cast<QuantumRegister>(r + 1);
    return r;
}

static QuantumRegisterABI& operator++(QuantumRegisterABI& r) {
    r = static_cast<QuantumRegisterABI>(r + 1);
    return r;
}

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

static std::ostream &operator<<(std::ostream &out, const QuantumRegisterABI reg) {
  switch (reg) {
  case QuantumRegisterABI::qzero:
    return out << "qzero";
  case QuantumRegisterABI::qt1:
    return out << "qt1";
  case QuantumRegisterABI::qt2:
    return out << "qt2";
  case QuantumRegisterABI::qt3:
    return out << "qt3";
  case QuantumRegisterABI::qt4:
    return out << "qt4";
  case QuantumRegisterABI::qt5:
    return out << "qt";
  case QuantumRegisterABI::qt6:
    return out << "qt6";
  case QuantumRegisterABI::qt7:
    return out << "qt7";
  case QuantumRegisterABI::qs0:
    return out << "qs0";
  case QuantumRegisterABI::qs1:
    return out << "qs1";
  case QuantumRegisterABI::qa0:
    return out << "qa0";
  case QuantumRegisterABI::qa1:
    return out << "qa1";
  case QuantumRegisterABI::qa2:
    return out << "qa2";
  case QuantumRegisterABI::qa3:
    return out << "qa3";
  case QuantumRegisterABI::qa4:
    return out << "qa4";
  case QuantumRegisterABI::qa5:
    return out << "qa5";
  case QuantumRegisterABI::qa6:
    return out << "qa6";
  case QuantumRegisterABI::qa7:
    return out << "qs7";
  case QuantumRegisterABI::qs2:
    return out << "qs2";
  case QuantumRegisterABI::qs3:
    return out << "qs3";
  case QuantumRegisterABI::qs4:
    return out << "qs4";
  case QuantumRegisterABI::qs5:
    return out << "qs5";
  case QuantumRegisterABI::qs6:
    return out << "qs6";
  case QuantumRegisterABI::qs7:
    return out << "qs7";
  case QuantumRegisterABI::qs8:
    return out << "qs8";
  case QuantumRegisterABI::qs9:
    return out << "qs9";
  case QuantumRegisterABI::qs10:
    return out << "qs10";
  case QuantumRegisterABI::qs11:
    return out << "qs1";
  case QuantumRegisterABI::qt8:
    return out << "qt8";
  case QuantumRegisterABI::qt9:
    return out << "qt9";
  case QuantumRegisterABI::qt10:
    return out << "qt10";
  case QuantumRegisterABI::qt11:
    return out << "qt11";
  }
}


}
