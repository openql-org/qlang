#include "lexer.hpp"
#include "token.hpp"

using namespace qlang;

std::map<std::string, TokenType> Lexer::keywords;

bool Lexer::try_readc(char c) {
  if (c == peekc()) {
    head++;
    return true;
  }
  return false;
}

Lexer::Lexer(const std::string &path) : path(path) {
  Lexer::init_keywords();
  std::ifstream ifs(path);
  if (ifs.fail()) {
    throw "Can not open " + path;
  }
  std::istreambuf_iterator<char> it(ifs);
  std::istreambuf_iterator<char> last;
  source_program = std::move(std::string(it, last));
}

Token Lexer::nextToken() {
  if (buffer.size() > 0) {
    auto t = buffer.back();
    buffer.pop_back();
    return std::move(t);
  }

  skip_blank();
  switch (peekc()) {
  case '0' ... '9':
    return std::move(read_number());
  case 'a' ... 'z':
  case 'A' ... 'Z':
  case '_':
    return std::move(read_ident());
  case '\0':
    return std::move(Token(TokenType::TEOF));
  case '+':
    readc();
    return std::move(Token(TokenType::Plus));
  case '-':
    readc();
    return std::move(Token(TokenType::Minus));
  case '*':
    readc();
    return std::move(Token(TokenType::Mul));
  case '/':
    readc();
    return std::move(Token(TokenType::Div));
  case '=':
    readc();
    if (try_readc('=')) {
      return std::move(Token(TokenType::Equal));
    } else {
      return std::move(Token(TokenType::Assign));
    }
  case '<':
    readc();
    if (try_readc('=')) {
      return std::move(Token(TokenType::LessEqual));
    } else if (try_readc('>')) {
      return std::move(Token(TokenType::NotEqual));
    } else {
      return std::move(Token(TokenType::Less));
    }
  case '>':
    readc();
    if (try_readc('=')) {
      return std::move(Token(TokenType::GreaterEqual));
    } else {
      return std::move(Token(TokenType::Greater));
    }
  case '(':
    readc();
    return std::move(Token(TokenType::ParenL));
  case ')':
    readc();
    return std::move(Token(TokenType::ParenR));
  case '}':
    readc();
    return std::move(Token(TokenType::CurlyBracesR));
  case '{':
    readc();
    return std::move(Token(TokenType::CurlyBracesL));
  case ';':
    readc();
    return std::move(Token(TokenType::Semicolon));
  case ',':
    readc();
    return std::move(Token(TokenType::Colon));
  case ':':
  default:
    throw "invalid char";
  }
}

Token Lexer::take(TokenType type) {
  Token t = nextToken();
  if (t.type != type) {
    throw "unexpected token";
  }
  return std::move(t);
}

void Lexer::untake(Token &&token) { buffer.push_back(std::move(token)); }

void Lexer::print_all() {
  while (true) {
    Token token = nextToken();
    std::cout << token << std::endl;
    if (token.type == TokenType::TEOF) {
      break;
    }
  }
}

void Lexer::skip_blank() {
  // skip ' ', '\t', '\n', '\v', '\f', '\r'
  while (1) {
    char n = peekc();
    if (n == '/' && try_readc('/')) {
      // comment out
      n = readc();
      while (1) {
        if (n == '\n' || n == '\r') break;
        n = readc();
      }
    }

    if (!isspace(n)) {
      break;
    }
    readc();
  }
}

Token Lexer::read_number() {
  std::string digit;
  while (isdigit(peekc())) {
    digit.push_back(readc());
  }
  return std::move(Token(std::stoll(digit)));
}

bool is_ident_piece(char c) { return isalnum(c) || c == '_'; }

Token Lexer::read_ident() {
  std::string ident;
  while (is_ident_piece(peekc())) {
    ident.push_back(readc());
  }
    
  auto itr = keywords.find(ident);
  if (itr == keywords.end()) {
    return std::move(Token(std::move(ident)));
  } else {
    return std::move(Token(itr->second));
  }
}

void Lexer::print_head() { std::cout << "head: " << head << std::endl; }

void Lexer::init_keywords() {
  if (keywords.size() != 0) {
    return;
  }
  keywords["const"] = TokenType::Const;
  keywords["var"] = TokenType::Var;
  keywords["func"] = TokenType::Function;
  keywords["if"] = TokenType::If;
  keywords["while"] = TokenType::While;
  keywords["do"] = TokenType::Do;
  keywords["main"] = TokenType::Main;
  keywords["return"] = TokenType::Return;
  keywords["write"] = TokenType::Write;
  keywords["writeln"] = TokenType::Writeln;
  keywords["qfunc"] = TokenType::QFunc;
  keywords["odd"] = TokenType::Odd;
  keywords["int"] = TokenType::Integer;
  keywords["qint"] = TokenType::Qint;
}
