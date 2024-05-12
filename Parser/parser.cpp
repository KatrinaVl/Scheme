#include <parser.h>
#include "../Tokenizer/tokenizer.h"
#include "../error.h"

int64_t count_open = 0;

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    auto c = tokenizer->GetToken();
    if (tokenizer->IsEnd()) {
        throw SyntaxError("empty");
    }
    if (ConstantToken* x = std::get_if<ConstantToken>(&c)) {
        if (!tokenizer->IsEnd()) {
            tokenizer->Next();
            return std::make_shared<Number>(x->value);
        }
    } else if (QuoteToken* x = std::get_if<QuoteToken>(&c)) {
        tokenizer->Next();
        if (!tokenizer->IsEnd()) {
            auto o = std::make_shared<Quote>(ReadQuote(tokenizer));
            if (!tokenizer->IsEnd()) {
                throw SyntaxError("After quote also ergu");
            }
            return o;
        } else {
            throw SyntaxError("not arguments for quote");
        }
    } else if (SymbolToken* x = std::get_if<SymbolToken>(&c)) {
        if (!tokenizer->IsEnd()) {
            tokenizer->Next();
            return std::make_shared<Symbol>(x->name);
        }
    } else if (BracketToken* x = std::get_if<BracketToken>(&c)) {
        if (*x == BracketToken::OPEN) {
            count_open = 0;
            int64_t& c = count_open;
            ++count_open;
            auto o = ReadList(tokenizer);
            --count_open;
            if (!tokenizer->IsEnd()) {
                throw SyntaxError("count_open");
            }
            return o;
        }
        throw SyntaxError("does not open bracket");
    } else if (DotToken* x = std::get_if<DotToken>(&c)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(x->name_);
    } else {
        throw SyntaxError("does not this token");
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    tokenizer->Next();
    auto c = tokenizer->GetToken();
    if (BracketToken* x = std::get_if<BracketToken>(&c)) {
        if (*x == BracketToken::CLOSE) {
            tokenizer->Next();
            return nullptr;
        }
        if (tokenizer->IsEnd()) {
            throw SyntaxError("does not close");
        }
    }
    std::shared_ptr<Cell> root = std::make_shared<Cell>();
    auto curr_cell = root;
    auto parent = root;
    bool was_dot = false;
    bool was_quote = false;
    while (c != Token{BracketToken::CLOSE}) {
        std::shared_ptr<Object> curr;
        if (c == Token{BracketToken::OPEN}) {
            curr = ReadList(tokenizer);
        } else if (QuoteToken* x = std::get_if<QuoteToken>(&c)) {
            tokenizer->Next();
            curr = std::make_shared<Quote>(ReadQuote(tokenizer));
        } else {
            curr = Read(tokenizer);
        }
        if (curr == nullptr) {
            curr_cell->first_ = curr;
            if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
                curr_cell->second_ = nullptr;
                break;
            } else {
                curr_cell->second_ = std::make_shared<Cell>();
                parent = curr_cell;
                curr_cell = As<Cell>(curr_cell->second_);
                c = tokenizer->GetToken();
            }
        }
        if (Is<Number>(curr)) {
            curr_cell->first_ = curr;
            if (!was_dot) {
                if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
                    curr_cell->second_ = nullptr;
                    break;
                } else if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
                    while (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
                        curr_cell->second_ = std::make_shared<Cell>();
                        curr_cell = As<Cell>(curr_cell->second_);
                        curr_cell->first_ = ReadList(tokenizer);
                        c = tokenizer->GetToken();
                    }
                } else {
                    curr_cell->second_ = std::make_shared<Cell>();
                    parent = curr_cell;
                    curr_cell = As<Cell>(curr_cell->second_);
                    c = tokenizer->GetToken();
                }
            } else {
                if (tokenizer->GetToken() != Token{BracketToken::CLOSE}) {
                    throw SyntaxError(" after dot and number need ')' ");
                }
                curr_cell = parent;
                curr_cell->second_ = curr;
                was_dot = false;
                c = tokenizer->GetToken();
            }
        } else if (Is<Cell>(curr)) {
            if (tokenizer->IsEnd()) {
                throw SyntaxError("Does not )");
            }
            curr_cell->first_ = curr;
            if (!was_dot) {
                if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
                    curr_cell->second_ = nullptr;
                    break;
                } else if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
                    while (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
                        curr_cell->second_ = std::make_shared<Cell>();
                        curr_cell = As<Cell>(curr_cell->second_);
                        curr_cell->first_ = ReadList(tokenizer);
                        c = tokenizer->GetToken();
                    }
                } else {
                    curr_cell->second_ = std::make_shared<Cell>();
                    parent = curr_cell;
                    curr_cell = As<Cell>(curr_cell->second_);
                    c = tokenizer->GetToken();
                }
            } else {
                if (tokenizer->GetToken() != Token{BracketToken::CLOSE}) {
                    throw SyntaxError(" after dot and number need ')' ");
                }
                curr_cell = parent;
                curr_cell->second_ = curr;
                was_dot = false;
                c = tokenizer->GetToken();
            }
        } else if (Is<Symbol>(curr)) {
            auto cc = As<Symbol>(curr);
            if (cc->GetName() == "." && !was_dot) {
                curr_cell = parent;
                if (curr_cell->first_ == nullptr) {
                    throw SyntaxError("does not first argument in pair");
                }
                curr_cell = As<Cell>(curr_cell->second_);
                was_dot = true;
                if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
                    throw SyntaxError("does not token after dot");
                }
                if (tokenizer->IsEnd()) {
                    throw SyntaxError("does not token after dot");
                }
                if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
                    curr_cell = parent;
                    curr_cell->second_ = ReadList(tokenizer);
                    if (tokenizer->IsEnd()) {
                        throw SyntaxError("end not in right place");
                    }
                }
            } else if (cc->GetName() == "quote") {
                auto o = std::make_shared<Quote>(ReadQuote(tokenizer));
                curr_cell->first_ = o;
                parent = curr_cell;
                curr_cell = As<Cell>(curr_cell->second_);

            } else {
                curr_cell->first_ = curr;
                if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
                    curr_cell->second_ = nullptr;
                    break;
                }
                curr_cell->second_ = std::make_shared<Cell>();
                parent = curr_cell;
                curr_cell = As<Cell>(curr_cell->second_);
            }
            c = tokenizer->GetToken();
        } else if (Is<Quote>(curr)) {
            curr_cell->first_ = curr;
            if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
                curr_cell->second_ = nullptr;
                break;
            } else {
                curr_cell->second_ = std::make_shared<Cell>();
                parent = curr_cell;
                curr_cell = As<Cell>(curr_cell->second_);
                c = tokenizer->GetToken();
            }

        } else {
            throw SyntaxError("syntax error");
        }
    }
    tokenizer->Next();
    return root;
}

std::shared_ptr<Object> ReadQuote(Tokenizer* tokenizer) {
    auto c = tokenizer->GetToken();
    if (ConstantToken* x = std::get_if<ConstantToken>(&c)) {
        if (!tokenizer->IsEnd()) {
            tokenizer->Next();
            return std::make_shared<Number>(x->value);
        }
    } else if (QuoteToken* x = std::get_if<QuoteToken>(&c)) {
        tokenizer->Next();
        if (!tokenizer->IsEnd()) {
            return ReadQuote(tokenizer);
        } else {
            throw SyntaxError("not arguments for quote");
        }
    } else if (SymbolToken* x = std::get_if<SymbolToken>(&c)) {
        if (!tokenizer->IsEnd()) {
            tokenizer->Next();
            return std::make_shared<Symbol>(x->name);
        }
    } else if (DotToken* x = std::get_if<DotToken>(&c)) {
        if (!tokenizer->IsEnd()) {
            tokenizer->Next();
            return std::make_shared<Symbol>(".");
        }
    } else if (BracketToken* x = std::get_if<BracketToken>(&c)) {
        if (*x == BracketToken::OPEN) {
            auto o = ReadList(tokenizer);
            return o;
        }
        throw SyntaxError("does not open bracket");
    }
}
