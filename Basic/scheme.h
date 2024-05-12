#pragma once

#include <string>
#include <algorithm>
#include "../Parser/object.h"
#include "../Parser/parser.h"
#include "../Tokenizer/tokenizer.h"
#include "../error.h"

class Interpreter {
public:
    std::string Run(const std::string& s);
};

struct Less {
    template <class T>
    bool operator()(T& a, T& b) {
        if (As<Number>(a)->GetValue() < As<Number>(b)->GetValue()) {
            return true;
        }
        return false;
    }
};

struct Large {
    template <class T>
    bool operator()(T& a, T& b) {
        if (As<Number>(a)->GetValue() > As<Number>(b)->GetValue()) {
            return true;
        }
        return false;
    }
};

struct Equ {
    template <class T>
    bool operator()(T& a, T& b) {
        if (As<Number>(a)->GetValue() == As<Number>(b)->GetValue()) {
            return true;
        }
        return false;
    }
};

struct LessOrEqu {
    template <class T>
    bool operator()(T& a, T& b) {
        if (As<Number>(a)->GetValue() <= As<Number>(b)->GetValue()) {
            return true;
        }
        return false;
    }
};

struct LargeOrEqu {
    template <class T>
    bool operator()(T& a, T& b) {
        if (As<Number>(a)->GetValue() >= As<Number>(b)->GetValue()) {
            return true;
        }
        return false;
    }
};

template <class Iterator, class Op>
std::shared_ptr<Boolen> Inequalities(Iterator first, Iterator last, Op op) {
    if (*first == nullptr) {
        return std::make_shared<Boolen>("#t");
    }
    auto second = first;
    ++second;
    while (second != last) {
        if (!Is<Number>(*first) || (!Is<Number>(*second) && *second != nullptr)) {
            throw RuntimeError("Not number in inequalities");
        }
        if (*second != nullptr && !op(*first, *second)) {
            return std::make_shared<Boolen>("#f");
        }
        ++first;
        ++second;
    }
    return std::make_shared<Boolen>("#t");
}

struct Sum {
    template <class T, class S>
    void operator()(T& a, S& b) {
        if (*b == nullptr) {
            a += 0;
        } else if (Is<Number>(*b)) {
            a += As<Number>(*b)->GetValue();
        } else {
            throw RuntimeError("Not number in Sum");
        }
    }
};

struct Prod {
    template <class T, class S>
    void operator()(T& a, S& b) {
        if (*b == nullptr) {
            a *= 1;
        } else if (Is<Number>(*b)) {
            a *= As<Number>(*b)->GetValue();
        } else {
            throw RuntimeError("Not number in Sum");
        }
    }
};

struct Sub {
    template <class T, class S>
    void operator()(T& a, S& b) {
        if (*b == nullptr) {
            a -= 0;
        } else if (Is<Number>(*b)) {
            a -= As<Number>(*b)->GetValue();
        } else {
            throw RuntimeError("Not number in Sum");
        }
    }
};

struct Div {
    template <class T, class S>
    void operator()(T& a, S& b) {
        if (*b == nullptr) {
            a /= 1;
        } else if (Is<Number>(*b)) {
            if (As<Number>(*b)->GetValue() == 0) {
                throw RuntimeError("Div from 0");
            } else {
                a /= As<Number>(*b)->GetValue();
            }
        } else {
            throw RuntimeError("Not number in Sum");
        }
    }
};

template <class Iterator, class Op>
std::shared_ptr<Number> Operations(Iterator first, Iterator last, int64_t init, Op op, char c) {
    if (*first == nullptr && (c == '/' || c == '-')) {
        throw RuntimeError("not two arguments in Div and Sub");
    }
    ++first;
    if ((c == '/' || c == '-') && *first == nullptr) {
        throw RuntimeError("not one arguments in Div and Sub");
    }
    --first;
    if (Is<Number>(*first)) {
        init = As<Number>(*first)->GetValue();
        ++first;
    }
    while (first != last) {
        op(init, first);
        ++first;
    }
    return std::make_shared<Number>(init);
}

struct Max {
    template <class T, class S>
    void operator()(T& a, S& b) {
        if (Is<Number>(b)) {
            if (a < As<Number>(b)->GetValue()) {
                a = As<Number>(b)->GetValue();
            }
        } else {
            throw RuntimeError("Not number in Max");
        }
    }
};

struct Min {
    template <class T, class S>
    void operator()(T& a, S& b) {
        if (Is<Number>(b)) {
            if (a > As<Number>(b)->GetValue()) {
                a = As<Number>(b)->GetValue();
            }
        } else {
            throw RuntimeError("Not number in Max");
        }
    }
};

template <class Iterator, class Op>
std::shared_ptr<Number> MaxMin(Iterator first, Iterator last, int64_t init, Op op) {
    if (*first == nullptr && first + 1 == last) {
        throw RuntimeError("not arguments in MaxMin");
    }
    if (Is<Number>(*first)) {
        init = As<Number>(*first)->GetValue();
        ++first;
    }
    while (first != last) {
        if (*first == nullptr && first + 1 == last) {
            ++first;
        } else {
            op(init, *first);
            ++first;
        }
    }
    return std::make_shared<Number>(init);
}
