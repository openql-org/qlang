#pragma once

#include <string>

namespace qlang {
enum class TokenType {
  Integer,
  Ident,

  // keywords
  Const,
  Var,
  Function,
  If,
  Then,
  While,
  Do,
  Main,
  Return,
  Write,
  Writeln,
  Odd,
  Qint,

  Plus,  // +
  Minus, // -
  Mul,   // *
  Div,   // /

  Equal,        // =
  NotEqual,     // <>
  Less,         // <
  LessEqual,    // <=
  Greater,      // >
  GreaterEqual, // >=

  Assign,    // :=
  Semicolon, // ;
  Colon,     // ,
  ParenL,    // (
  ParenR,    // (
  CurlyBracesR,    // }
  CurlyBracesL,    // {

  TEOF,
};

class Token {
public:
  Token() : type(TokenType::TEOF) {}
  Token(TokenType type) : type(type) {}
  Token(long long integer) : type(TokenType::Integer), integer(integer) {}
  Token(std::string &&ident) : type(TokenType::Ident), ident(ident) {}

public:
  TokenType type;
  long long integer;
  std::string ident;
};

static std::ostream &operator<<(std::ostream &out, const TokenType type) {
  switch (type) {
  case TokenType::Integer:
    return out << "Integer";
  case TokenType::Ident:
    return out << "Ident";

  // keywords
  case TokenType::Qint:
    return out << "Qint";
  case TokenType::Const:
    return out << "Const";
  case TokenType::Var:
    return out << "Var";
  case TokenType::Function:
    return out << "Function";
  case TokenType::If:
    return out << "If";
  case TokenType::Then:
    return out << "Then";
  case TokenType::While:
    return out << "While";
  case TokenType::Do:
    return out << "Do";
  case TokenType::Main:
    return out << "Main";
  case TokenType::Return:
    return out << "Return";
  case TokenType::Write:
    return out << "Write";
  case TokenType::Writeln:
    return out << "Writeln";
  case TokenType::Odd:
    return out << "Odd";

  case TokenType::Plus:
    return out << "Plus";
  case TokenType::Minus:
    return out << "Minus";
  case TokenType::Mul:
    return out << "Mul";
  case TokenType::Div:
    return out << "Div";

  case TokenType::Equal:
    return out << "Equal";
  case TokenType::NotEqual:
    return out << "NotEqual";
  case TokenType::Less:
    return out << "Less";
  case TokenType::LessEqual:
    return out << "LessEqual";
  case TokenType::Greater:
    return out << "Greater";
  case TokenType::GreaterEqual:
    return out << "GreaterEqual";

  case TokenType::Assign:
    return out << "Assign";
  case TokenType::Semicolon:
    return out << "Semicolon";
  case TokenType::Colon:
    return out << "Colon";
  case TokenType::ParenL:
    return out << "ParenL";
  case TokenType::ParenR:
    return out << "ParenR";
  case TokenType::CurlyBracesR:
    return out << "CurlyBracesR";
  case TokenType::CurlyBracesL:
    return out << "CurlyBracesL";
  case TokenType::TEOF:
    return out << "EOF";
  }
}

static std::ostream &operator<<(std::ostream &out, const Token &token) {
  out << token.type;
  if (token.type == TokenType::Integer) {
    out << " " << token.integer;
  } else if (token.type == TokenType::Ident) {
    out << " " << token.ident;
  }
  return out;
}

} // namespace pl0
