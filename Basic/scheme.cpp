#include "scheme.h"
#include <sstream>
#include <vector>
#include "../error.h"

std::shared_ptr<Object> MakeCell(std::shared_ptr<Object> obj);
std::shared_ptr<Object> MakeTree(std::shared_ptr<Object> obj);

void Go(std::shared_ptr<Object> obj, std::vector<std::shared_ptr<Object>>& objects) {
    if (Is<Number>(obj)) {
        objects.push_back(As<Number>(obj));
        return;
    } else if (Is<Symbol>(obj)) {
        objects.push_back(As<Symbol>(obj));
        return;
    } else if (Is<Quote>(obj)) {
        objects.push_back(As<Quote>(obj)->object_);
        return;
    } else if (Is<Boolen>(obj)) {
        objects.push_back(As<Boolen>(obj));
        return;
    } else if (Is<Cell>(obj)) {
        if (As<Cell>(obj)->first_ == nullptr && As<Cell>(obj)->second_ == nullptr) {
            objects.push_back(std::make_shared<Object>());
        }
        if (Is<Cell>(As<Cell>(obj)->first_)) {
            auto o = MakeCell(As<Cell>(obj)->first_);
            As<Cell>(obj)->first_ = o;
        }
        while (As<Cell>(obj)->first_ != nullptr || As<Cell>(As<Cell>(obj)->first_)) {
            Go(As<Cell>(obj)->first_, objects);
            while (As<Cell>(obj)->second_ != nullptr) {
                Go(As<Cell>(obj)->second_, objects);
                return;
            }
            if (As<Cell>(obj)->second_ == nullptr) {
                objects.push_back(nullptr);
                return;
            }
        }
    }
}

std::vector<std::shared_ptr<Object>> Helper(std::shared_ptr<Object>& obj) {
    std::vector<std::shared_ptr<Object>> objects;
    if (Is<Number>(obj)) {
        objects.push_back(As<Number>(obj));
    } else if (Is<Symbol>(obj)) {
        objects.push_back(As<Symbol>(obj));
    } else if (Is<Quote>(obj)) {
        objects.push_back(As<Quote>(obj)->object_);
    } else if (Is<Cell>(obj)) {
        Go(obj, objects);
    }
    return objects;
}

std::string Print(std::shared_ptr<Object> obj);

std::string PrintCell(std::shared_ptr<Object> obj, bool& first) {
    std::string curr;
    auto o = As<Cell>(obj);
    if (o->first_ == nullptr && o->second_ == nullptr) {
        return "()";
    }
    if (!Is<Cell>(o->first_) && !Is<Cell>(o->second_) && o->first_ != nullptr &&
        o->second_ != nullptr) {
        curr += Print(o->first_) + " . ";
        curr += Print(o->second_);
        return curr;
    }
    while (o) {
        if (!Is<Cell>(o->first_) && !Is<Cell>(o->second_) && o->first_ != nullptr &&
            o->second_ != nullptr) {
            curr += Print(o->first_) + " . ";
            curr += Print(o->second_);
            return curr;
        }
        if (!o->second_) {
            curr += Print(o->first_);
        } else {
            curr += Print(o->first_) + " ";
        }
        o = As<Cell>(o->second_);
    }
    return curr;
}

std::string Print(std::shared_ptr<Object> obj) {
    bool first = true;
    std::string curr;
    if (obj == nullptr) {
        throw RuntimeError("print of nullptr");
    }
    if (Is<Number>(obj)) {
        auto b = As<Number>(obj);
        curr += std::to_string(b->GetValue());
    } else if (Is<Symbol>(obj)) {
        auto b = As<Symbol>(obj);
        curr += b->GetName();
    } else if (Is<Boolen>(obj)) {
        auto b = As<Boolen>(obj);
        curr += b->GetName();
    } else if (Is<Cell>(obj)) {
        curr += "(";
        bool first = true;
        curr += PrintCell(obj, first);
        curr += ")";
    } else if (Is<Quote>(obj)) {
        auto b = As<Quote>(obj)->object_;
        if (b == nullptr) {
            return "()";
        }
        curr += Print(b);
    }
    return curr;
}

std::shared_ptr<Object> MakeCell(std::shared_ptr<Object> obj) {
    auto o = std::make_shared<Object>();
    auto curr = obj;
    if (As<Cell>(obj)->first_ == nullptr && As<Cell>(obj)->second_ == nullptr) {
        return nullptr;
    }
    if (Is<Symbol>(As<Cell>(curr)->first_)) {
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "number?") {
            curr = As<Cell>(curr)->second_;
            if (Is<Number>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                return std::make_shared<Boolen>("#t");
            } else if (!Is<Number>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                return std::make_shared<Boolen>("#f");
            } else {
                throw SyntaxError("more than one element");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "=" ||
            As<Symbol>(As<Cell>(curr)->first_)->GetName() == "<" ||
            As<Symbol>(As<Cell>(curr)->first_)->GetName() == ">" ||
            As<Symbol>(As<Cell>(curr)->first_)->GetName() == "<=" ||
            As<Symbol>(As<Cell>(curr)->first_)->GetName() == ">=") {
            std::vector<std::shared_ptr<Object>> objects = Helper(curr);
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "<") {
                if (objects[1] == nullptr) {
                    return std::make_shared<Boolen>("#t");
                }
                for (int i = 1; i < objects.size() - 1; ++i) {
                    if (!Is<Number>(objects[i]) ||
                        (!Is<Number>(objects[i + 1]) && objects[i + 1] != nullptr)) {
                        throw RuntimeError("Not number in inequalities");
                    }
                    if (objects[i + 1] != nullptr && !(As<Number>(objects[i])->GetValue() <
                                                       As<Number>(objects[i + 1])->GetValue())) {
                        return std::make_shared<Boolen>("#f");
                    }
                }
                return std::make_shared<Boolen>("#t");
            }
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == ">") {
                if (objects[1] == nullptr) {
                    return std::make_shared<Boolen>("#t");
                }
                for (int i = 1; i < objects.size() - 1; ++i) {
                    if (!Is<Number>(objects[i]) ||
                        (!Is<Number>(objects[i + 1]) && objects[i + 1] != nullptr)) {
                        throw RuntimeError("Not number in inequalities");
                    }
                    if (objects[i + 1] != nullptr && !(As<Number>(objects[i])->GetValue() >
                                                       As<Number>(objects[i + 1])->GetValue())) {
                        return std::make_shared<Boolen>("#f");
                    }
                }
                return std::make_shared<Boolen>("#t");
            }
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "=") {
                if (objects[1] == nullptr) {
                    return std::make_shared<Boolen>("#t");
                }
                for (int i = 1; i < objects.size() - 1; ++i) {
                    if (!Is<Number>(objects[i]) ||
                        (!Is<Number>(objects[i + 1]) && objects[i + 1] != nullptr)) {
                        throw RuntimeError("Not number in inequalities");
                    }
                    if (objects[i + 1] != nullptr && !(As<Number>(objects[i])->GetValue() ==
                                                       As<Number>(objects[i + 1])->GetValue())) {
                        return std::make_shared<Boolen>("#f");
                    }
                }
                return std::make_shared<Boolen>("#t");
            }
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "<=") {
                if (objects[1] == nullptr) {
                    return std::make_shared<Boolen>("#t");
                }
                for (int i = 1; i < objects.size() - 1; ++i) {
                    if (!Is<Number>(objects[i]) ||
                        (!Is<Number>(objects[i + 1]) && objects[i + 1] != nullptr)) {
                        throw RuntimeError("Not number in inequalities");
                    }
                    if (objects[i + 1] != nullptr && !(As<Number>(objects[i])->GetValue() <=
                                                       As<Number>(objects[i + 1])->GetValue())) {
                        return std::make_shared<Boolen>("#f");
                    }
                }
                return std::make_shared<Boolen>("#t");
            }
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == ">=") {
                if (objects[1] == nullptr) {
                    return std::make_shared<Boolen>("#t");
                }
                for (int i = 1; i < objects.size() - 1; ++i) {
                    if (!Is<Number>(objects[i]) ||
                        (!Is<Number>(objects[i + 1]) && objects[i + 1] != nullptr)) {
                        throw RuntimeError("Not number in inequalities");
                    }
                    if (objects[i + 1] != nullptr && !(As<Number>(objects[i])->GetValue() >=
                                                       As<Number>(objects[i + 1])->GetValue())) {
                        return std::make_shared<Boolen>("#f");
                    }
                }
                return std::make_shared<Boolen>("#t");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "+" ||
            As<Symbol>(As<Cell>(curr)->first_)->GetName() == "*" ||
            As<Symbol>(As<Cell>(curr)->first_)->GetName() == "/" ||
            As<Symbol>(As<Cell>(curr)->first_)->GetName() == "-") {
            std::vector<std::shared_ptr<Object>> objects = Helper(curr);
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "+") {
                if (objects[1] == nullptr) {
                    return std::make_shared<Number>(0);
                }
                int64_t init = 0;
                if (Is<Number>(objects[1])) {
                    init = As<Number>(objects[1])->GetValue();
                }
                bool f = 0;
                for (int i = 2; i < objects.size(); ++i) {
                    f = 1;
                    if (objects[i] == nullptr) {
                        init += 0;
                    } else if (Is<Number>(objects[i])) {
                        init += As<Number>(objects[i])->GetValue();
                    } else {
                        throw RuntimeError("Not number in Sum");
                    }
                }
                if (f == 0 && !Is<Number>(objects[objects.size() - 1])) {
                    throw RuntimeError("Not number in Sum");
                }
                return std::make_shared<Number>(init);
            }
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "-") {
                if (objects[1] == nullptr) {
                    throw RuntimeError("not one arguments in Sub");
                }
                int64_t init = 0;
                if (Is<Number>(objects[1])) {
                    init = As<Number>(objects[1])->GetValue();
                }
                for (int i = 2; i < objects.size(); ++i) {
                    if (objects[i] == nullptr) {
                        init -= 0;
                    } else if (Is<Number>(objects[i])) {
                        init -= As<Number>(objects[i])->GetValue();
                    } else {
                        throw RuntimeError("Not number in Sum");
                    }
                }
                return std::make_shared<Number>(init);
            }
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "*") {
                if (objects[1] == nullptr) {
                    return std::make_shared<Number>(1);
                }
                int64_t init = 0;
                if (Is<Number>(objects[1])) {
                    init = As<Number>(objects[1])->GetValue();
                }
                for (int i = 2; i < objects.size(); ++i) {
                    if (objects[i] == nullptr) {
                        init *= 1;
                    } else if (Is<Number>(objects[i])) {
                        init *= As<Number>(objects[i])->GetValue();
                    } else {
                        throw RuntimeError("Not number in Sum");
                    }
                }
                return std::make_shared<Number>(init);
            }
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "/") {
                if (objects[1] == nullptr) {
                    throw RuntimeError("not one arguments in Sub");
                }
                int64_t init = 0;
                if (Is<Number>(objects[1])) {
                    init = As<Number>(objects[1])->GetValue();
                }
                for (int i = 2; i < objects.size(); ++i) {
                    if (objects[i] == nullptr) {
                        init /= 1;
                    } else if (Is<Number>(objects[i])) {
                        if (As<Number>(objects[i])->GetValue() == 0) {
                            throw RuntimeError("Div from 0");
                        } else {
                            init /= As<Number>(objects[i])->GetValue();
                        }
                    } else {
                        throw RuntimeError("Not number in Sum");
                    }
                }
                return std::make_shared<Number>(init);
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "max" ||
            As<Symbol>(As<Cell>(curr)->first_)->GetName() == "min") {
            std::vector<std::shared_ptr<Object>> objects = Helper(curr);
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "max") {
                auto b = MaxMin(objects.begin() + 1, objects.end(), 0, Max());
                return b;
            }
            if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "min") {
                auto b = MaxMin(objects.begin() + 1, objects.end(), 0, Min());
                return b;
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "abs") {
            curr = As<Cell>(curr)->second_;
            if (curr == nullptr) {
                throw RuntimeError("Not arguments in abs");
            }
            if (Is<Number>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                curr = As<Cell>(curr)->first_;
                if (As<Number>(curr)->GetValue() < 0) {
                    auto b = std::make_shared<Number>(As<Number>(curr)->GetValue() * -1);
                    return b;
                }
                auto b = As<Number>(curr);
                return b;
            } else {
                throw RuntimeError("argument im abs not a number");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "pair?") {
            curr = As<Cell>(curr)->second_;
            if (Is<Quote>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                curr = As<Quote>(As<Cell>(curr)->first_)->object_;
                if (curr == nullptr) {
                    return std::make_shared<Boolen>("#f");
                }
                if (As<Cell>(curr)) {
                    if (As<Cell>(curr)->second_ != nullptr && !Is<Cell>(As<Cell>(curr)->second_)) {
                        return std::make_shared<Boolen>("#t");
                    } else if (Is<Cell>(As<Cell>(curr)->second_) &&
                               As<Cell>(As<Cell>(curr)->second_)->second_ == nullptr) {
                        return std::make_shared<Boolen>("#t");
                    } else {
                        return std::make_shared<Boolen>("#f");
                    }
                }
            } else {
                throw("not right arguments in pair?");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "null?") {
            curr = As<Cell>(curr)->second_;
            if (Is<Quote>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                curr = As<Quote>(As<Cell>(curr)->first_)->object_;
                if (curr == nullptr) {
                    return std::make_shared<Boolen>("#t");
                } else {
                    return std::make_shared<Boolen>("#f");
                }
            } else {
                throw("not right arguments in null?, need quote");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "list?") {
            curr = As<Cell>(curr)->second_;
            if (Is<Quote>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                curr = As<Quote>(As<Cell>(curr)->first_)->object_;
                if (curr == nullptr) {
                    return std::make_shared<Boolen>("#t");
                } else {
                    std::vector<std::shared_ptr<Object>> objects = Helper(curr);
                    if (objects[objects.size() - 1] == nullptr) {
                        return std::make_shared<Boolen>("#t");
                    } else {
                        return std::make_shared<Boolen>("#f");
                    }
                }
            } else {
                throw("not right arguments in null?, need quote");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "cons") {
            curr = As<Cell>(curr)->second_;
            std::vector<std::shared_ptr<Object>> objects = Helper(curr);
            if (objects.size() == 2) {
                return curr;
            }
            if (objects.size() == 3 && objects[2] == nullptr) {
                auto p = curr;
                p = As<Cell>(curr)->second_;
                p = As<Cell>(p)->first_;
                As<Cell>(curr)->second_ = p;
                return curr;
            }
            return curr;
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "car") {
            curr = As<Cell>(curr)->second_;
            if (Is<Quote>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                curr = As<Quote>(As<Cell>(curr)->first_)->object_;
                if (curr == nullptr) {
                    throw RuntimeError("Does not arguments in car");
                }
                return As<Cell>(curr)->first_;
            } else {
                throw RuntimeError("in car not pair");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "cdr") {
            curr = As<Cell>(curr)->second_;
            if (Is<Quote>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                curr = As<Quote>(As<Cell>(curr)->first_)->object_;
                if (curr == nullptr) {
                    throw RuntimeError("Does not arguments in car");
                }
                if (As<Cell>(curr)->second_ == nullptr) {
                    return std::make_shared<Symbol>("()");
                }
                return As<Cell>(curr)->second_;
            } else {
                throw RuntimeError("in car not pair");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "list") {
            curr = As<Cell>(curr)->second_;
            if (curr == nullptr) {
                return std::make_shared<Symbol>("()");
            }
            return curr;
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "list-ref") {
            curr = As<Cell>(curr)->second_;
            auto qu = As<Cell>(curr)->first_;
            if (!Is<Quote>(qu)) {
                throw RuntimeError("you give not quote in list-ref");
            }
            std::vector<std::shared_ptr<Object>> objects = Helper(As<Quote>(qu)->object_);
            curr = As<Cell>(curr)->second_;
            if (Is<Number>(As<Cell>(curr)->first_)) {
                if (As<Number>(As<Cell>(curr)->first_)->GetValue() >= 0 &&
                    As<Number>(As<Cell>(curr)->first_)->GetValue() < objects.size() - 1) {
                    auto r = objects[As<Number>(As<Cell>(curr)->first_)->GetValue()];
                    return r;
                } else {
                    throw RuntimeError("index in list-ref more on less then need");
                }
            } else {
                throw RuntimeError("does not index");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "list-tail") {
            curr = As<Cell>(curr)->second_;
            auto qu = As<Cell>(curr)->first_;
            if (!Is<Quote>(qu)) {
                throw RuntimeError("you give not quote in list-ref");
            }
            std::vector<std::shared_ptr<Object>> objects = Helper(As<Quote>(qu)->object_);
            curr = As<Cell>(curr)->second_;
            if (Is<Number>(As<Cell>(curr)->first_)) {
                if (As<Number>(As<Cell>(curr)->first_)->GetValue() >= 0 &&
                    As<Number>(As<Cell>(curr)->first_)->GetValue() < objects.size() - 1) {
                    auto r = std::make_shared<Cell>();
                    auto curr_r = r;
                    auto parent = r;
                    int64_t i = As<Number>(As<Cell>(curr)->first_)->GetValue();
                    while (objects[i] != nullptr) {
                        curr_r->first_ = objects[i];
                        parent = curr_r;
                        curr_r->second_ = std::make_shared<Cell>();
                        curr_r = As<Cell>(curr_r->second_);
                        ++i;
                    }
                    curr_r = parent;
                    curr_r->second_ = nullptr;
                    return r;
                }
                if (As<Number>(As<Cell>(curr)->first_)->GetValue() == objects.size() - 1) {
                    return std::make_shared<Symbol>("()");
                } else {
                    throw RuntimeError("index in list-ref more on less then need");
                }
            } else {
                throw RuntimeError("does not index");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "boolean?") {
            curr = As<Cell>(curr)->second_;
            if (Is<Symbol>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                if ((As<Symbol>(As<Cell>(curr)->first_)->GetName() == "#t" ||
                     As<Symbol>(As<Cell>(curr)->first_)->GetName() == "#f")) {
                    return std::make_shared<Boolen>("#t");
                } else {
                    return std::make_shared<Boolen>("#f");
                }
            } else if (!Is<Symbol>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                return std::make_shared<Boolen>("#f");
            } else {
                throw RuntimeError("boolen? takes only one argument");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "not") {
            curr = As<Cell>(curr)->second_;
            if (curr == nullptr) {
                throw RuntimeError("doesn't arguments in not");
            }
            if (Is<Symbol>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "#f") {
                    return std::make_shared<Boolen>("#t");
                } else {
                    return std::make_shared<Boolen>("#f");
                }
            } else if (!Is<Symbol>(As<Cell>(curr)->first_) && As<Cell>(curr)->second_ == nullptr) {
                return std::make_shared<Boolen>("#f");
            } else {
                throw RuntimeError("not takes only one argument in not");
            }
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "and") {
            curr = As<Cell>(curr)->second_;
            if (curr == nullptr) {
                return std::make_shared<Boolen>("#t");
            }
            auto parent = curr;
            while (curr) {
                parent = curr;
                curr = As<Cell>(curr)->first_;
                auto r = MakeTree(curr);
                if (Is<Boolen>(r) && As<Boolen>(r)->GetName() == "#f") {
                    return std::make_shared<Boolen>("#f");
                }
                if (Is<Symbol>(r) && As<Symbol>(r)->GetName() == "#f") {
                    return std::make_shared<Boolen>("#f");
                }
                curr = parent;
                parent = curr;
                curr = As<Cell>(curr)->second_;
            }
            curr = parent;
            curr = As<Cell>(parent)->first_;
            auto r = MakeTree(curr);
            return r;
        }
        if (As<Symbol>(As<Cell>(curr)->first_)->GetName() == "or") {
            curr = As<Cell>(curr)->second_;
            if (curr == nullptr) {
                return std::make_shared<Boolen>("#f");
            }
            auto parent = curr;
            while (curr) {
                parent = curr;
                curr = As<Cell>(curr)->first_;
                auto r = MakeTree(curr);
                if (Is<Boolen>(r) && As<Boolen>(r)->GetName() == "#t") {
                    return std::make_shared<Boolen>("#t");
                }
                if ((Is<Symbol>(r) && As<Symbol>(r)->GetName() != "#f") || Is<Number>(r) ||
                    Is<Quote>(r)) {
                    return r;
                }
                curr = parent;
                parent = curr;
                curr = As<Cell>(curr)->second_;
            }
            curr = parent;
            curr = As<Cell>(parent)->first_;
            auto r = MakeTree(curr);
            return r;
        }
    } else if (Is<Quote>(As<Cell>(curr)->first_)) {
        if (As<Quote>(As<Cell>(curr)->first_)->object_ == nullptr &&
            As<Cell>(curr)->second_ == nullptr) {
            return std::make_shared<Symbol>("()");
        }
        if (As<Cell>(curr)->second_ == nullptr) {
            return As<Cell>(curr)->first_;
        } else {
            throw RuntimeError("it is list, when after quote i see also arguments");
        }
    } else if (Is<Cell>(As<Cell>(curr)->first_)) {
        throw RuntimeError("Cell not read");
    } else if (Is<Number>(As<Cell>(curr)->first_)) {
        throw RuntimeError("not quote");
    }
    return o;
}

std::shared_ptr<Object> MakeTree(std::shared_ptr<Object> obj) {
    auto o = std::make_shared<Object>();
    if (Is<Number>(obj)) {
        return As<Number>(obj);
    } else if (Is<Quote>(obj)) {
        return As<Quote>(obj);
    } else if (Is<Cell>(obj)) {
        return MakeCell(obj);
    } else if (Is<Symbol>(obj)) {
        return As<Symbol>(obj);
    } else if (obj == nullptr) {
        return nullptr;
    }
}

std::string Interpreter::Run(const std::string& str) {
    std::stringstream ss{str};
    Tokenizer tokenizer{&ss};
    auto obj = Read(&tokenizer);

    auto o = MakeTree(obj);
    std::vector<std::shared_ptr<Object>> objects;
    std::string result_str;
    result_str += Print(o);
    return result_str;
}
