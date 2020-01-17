#pragma once

#include <cassert>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <vector>

#include "error.hpp"
#include "quantum_register.hpp"

namespace qlang {
enum class IdType {
  Const,
  Var,
  Param,
  Function,
  Qint,
};

class IdInfo {
public:
  IdInfo(const std::string &name, IdType type, llvm::Function *func,
         llvm::Value *val, size_t level, QuantumRegister qreg)
      : name(name), type(type), func(func), val(val), level(level), qreg(qreg) {}

public:
  std::string name;
  IdType type;
  llvm::Function *func;
  llvm::Value *val;
  size_t level;
  QuantumRegister qreg;
};

static std::ostream &operator<<(std::ostream &out, const IdInfo &idinfo) {
  out << idinfo.name;
  switch (idinfo.type) {
    case IdType::Const:
    out << " Const ";
    break;
    case IdType::Qint:
    out << " Qint ";
    break;
    case IdType::Var:
    out << " Var ";
    break;
    case IdType::Param:
    out << " Param ";
    break;
    case IdType::Function:
    out << " Function ";
    break;
  }
  return out;
}


class Table {
public:
  const IdInfo &find(const std::string &name) const {
    auto itr =
        std::find_if(infos.rbegin(), infos.rend(),
                     [&](const IdInfo &info) { return info.name == name; });
    if (itr == infos.rend()) {
      undefinedError(name);
    }

    return *itr;
  }

  void appendConst(const std::string &name, llvm::Value *val) {
    infos.emplace_back(name, IdType::Const, nullptr, val, cur_level, QuantumRegister::q0);
  }

  void appendVar(const std::string &name, llvm::Value *val) {
    infos.emplace_back(name, IdType::Var, nullptr, val, cur_level, QuantumRegister::q0);
  }

  void appendQint(const std::string &name, QuantumRegister r) {
    infos.emplace_back(name, IdType::Qint, nullptr, nullptr, cur_level, r);
  }

  void appendQint(const std::string &name) {
    infos.emplace_back(name, IdType::Qint, nullptr, nullptr, cur_level, QuantumRegister::q0);
  }

  void appendParam(const std::string &name) {
    infos.emplace_back(name, IdType::Param, nullptr, nullptr, cur_level, QuantumRegister::q0);
  }

  void appendFunction(const std::string &name, llvm::Function *func) {
    infos.emplace_back(name, IdType::Function, func, nullptr, cur_level, QuantumRegister::q0);
  }

  void enterBlock() { cur_level++; }
  void leaveBlock() {
    while (!infos.empty() && infos.back().level == cur_level) {
      infos.pop_back();
    }
    cur_level--;
  }

  size_t getLevel() const { return cur_level; }

private:
  std::vector<IdInfo> infos;
  size_t cur_level;
};
} // namespace qlang
