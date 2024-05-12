#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <string>

struct SymbolToken {
    std::string name;
    SymbolToken() = default;
    SymbolToken(std::string n) : name(n) {
    }

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    QuoteToken() = default;
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    DotToken() = default;
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;
    ConstantToken() = default;
    ConstantToken(int v) : value(v) {
    }

    bool operator==(const ConstantToken& other) const;
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    std::istream* s_;
    Token t_;
    bool end_ = false;
};
