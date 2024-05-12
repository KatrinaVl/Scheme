#include <tokenizer.h>
#include <cstdio>
#include <cctype>
#include "error.h"

Tokenizer::Tokenizer(std::istream *in) {
    s_ = in;
    end_ = false;
    Next();
}
bool Tokenizer::IsEnd() {
    if (end_) {
        return true;
    }
    return false;
}

bool First(char c) {
    if (std::isalpha(c) || c == '<' || c == '=' || c == '>' || c == '*' || c == '/' || c == '#') {
        return true;
    }
    return false;
}

bool Second(char c) {
    if (std::isalpha(c) || c == '<' || c == '=' || c == '>' || c == '*' || c == '/' || c == '#' ||
        std::isdigit(c) || c == '?' || c == '!' || c == '-') {
        return true;
    }
    return false;
}

void Tokenizer::Next() {
    if (!end_) {
        int c = s_->get();
        std::string curr_str;
        while (true) {
            char curr_c = c;
            if (c == EOF) {
                end_ = true;
                return;
            }
            if (curr_c == ' ' || curr_c == '\n') {
                c = s_->get();
            } else if (curr_c == '(') {
                t_.emplace<1>(BracketToken::OPEN);
                return;
            } else if (curr_c == ')') {
                t_.emplace<1>(BracketToken::CLOSE);
                return;
            } else if (c == 39) {
                t_.emplace<3>();
                return;
            } else if (curr_c == '.') {
                t_.emplace<4>();
                return;
            } else if (std::isdigit(c)) {
                curr_str += c;
                c = s_->peek();
                if (std::isdigit(c)) {
                    c = s_->get();
                } else {
                    t_.emplace<0>(std::stoi(curr_str));
                    return;
                }
            } else if (First(c) || (Second(c) && !curr_str.empty())) {
                curr_str += c;
                c = s_->peek();
                if (Second(c)) {
                    c = s_->get();
                } else {
                    t_.emplace<2>(curr_str);
                    return;
                }
            } else if (curr_c == '-' || curr_c == '+') {
                curr_str += c;
                c = s_->peek();
                if (std::isdigit(c)) {
                    c = s_->get();
                } else {
                    t_.emplace<2>(curr_str);
                    return;
                }
            } else {
                throw SyntaxError("syntax error");
            }
        }
    }
}

Token Tokenizer::GetToken() {
    return t_;
}

bool SymbolToken::operator==(const SymbolToken &other) const {
    if (name == other.name) {
        return true;
    }
    return false;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    if (value == other.value) {
        return true;
    }
    return false;
}
