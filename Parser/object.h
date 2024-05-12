#pragma once

#include <memory>
#include <typeinfo>
#include <string>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number() = default;
    Number(int v) : value_(v) {
    }
    int GetValue() const {
        return value_;
    }

private:
    int64_t value_;
};

class Symbol : public Object {
public:
    Symbol() = default;
    Symbol(const std::string& str) : s_(str) {
    }
    const std::string& GetName() const {
        return s_;
    }

private:
    const std::string s_;
};

class Cell : public Object {
public:
    Cell() = default;
    Cell(std::shared_ptr<Object>& f, std::shared_ptr<Object>& s) : first_(f), second_(s) {
    }
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    void operator=(const std::shared_ptr<Cell>& other) {
        first_ = other->first_;
        second_ = other->second_;
    }

    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

class Quote : public Object {
public:
    Quote() = default;
    Quote(std::shared_ptr<Object> object) : object_(object) {
    }
    std::string GetValue() const {
        return q_;
    }

    std::string q_;
    char v_;
    std::shared_ptr<Object> object_;
};

class Boolen : public Object {
public:
    Boolen() = default;
    Boolen(const std::string& str) : s_(str) {
    }
    const std::string& GetName() const {
        return s_;
    }

private:
    const std::string s_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    std::shared_ptr<T> c = std::dynamic_pointer_cast<T>(obj);
    return c;
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto l = std::dynamic_pointer_cast<T>(obj);

    if (l == nullptr) {
        return false;
    } else if (typeid(T) == typeid(*l)) {
        return true;
    } else {
        return false;
    }
}
