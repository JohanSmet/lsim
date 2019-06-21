// property.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Key/value pair to store extra information about specific components

#ifndef LSIM_PROPERTY_H
#define LSIM_PROPERTY_H

#include <string>
#include <memory>
#include "sim_types.h"

namespace lsim {

class Property {
public:
    typedef std::unique_ptr<Property> uptr_t;
public:
    Property(const char *key) : m_key(key) {}
    virtual ~Property() = default;
    virtual const char *key() const {return m_key.c_str();};
    virtual uptr_t clone() const = 0;

    // value accessors
    virtual std::string value_as_string() const = 0;
    virtual int64_t value_as_integer() const = 0;
    virtual bool value_as_boolean() const = 0;
    virtual Value value_as_lsim_value() const = 0;

    // value modifiers
    virtual void value(const char *val) = 0;
    virtual void value(int64_t val) = 0;
    virtual void value(bool val) = 0;
    virtual void value(Value val) = 0;

private:
    std::string m_key;
};

class StringProperty : public Property {
public:
    StringProperty(const char *key, const char *value);
    uptr_t clone() const override;

    // value accessors
    std::string value_as_string() const override;
    int64_t value_as_integer() const override;
    bool value_as_boolean() const override;
    Value value_as_lsim_value() const override;

    // value modifiers
    void value(const char *val) override;
    void value(int64_t val)  override;
    void value(bool val) override;
    void value(Value val) override;

private:
    std::string m_value;
};

class IntegerProperty : public Property {
public:
    IntegerProperty(const char *key, int64_t value);
    uptr_t clone() const override;

    // value accessors
    std::string value_as_string() const override;
    int64_t value_as_integer() const override;
    bool value_as_boolean() const override;
    Value value_as_lsim_value() const override;

    // value modifiers
    void value(const char *val) override;
    void value(int64_t val)  override;
    void value(bool val) override;
    void value(Value val) override;

private:
    int64_t m_value;
};

class BoolProperty : public Property {
public:
    BoolProperty(const char *key, bool value);
    uptr_t clone() const override;

    // value accessors
    std::string value_as_string() const override;
    int64_t value_as_integer() const override;
    bool value_as_boolean() const override;
    Value value_as_lsim_value() const override;

    // value modifiers
    void value(const char *val) override;
    void value(int64_t val)  override;
    void value(bool val) override;
    void value(Value val) override;

private:    
    bool m_value;
};

class ValueProperty : public Property {
public:
    ValueProperty(const char *key, Value value);
    uptr_t clone() const override;

    // value accessors
    std::string value_as_string() const override;
    int64_t value_as_integer() const override;
    bool value_as_boolean() const override;
    Value value_as_lsim_value() const override;

    // value modifiers
    void value(const char *val) override;
    void value(int64_t val)  override;
    void value(bool val) override;
    void value(Value val) override;

private:    
    Value m_value;
};

inline Property::uptr_t make_property(const char *key, const char *value) {
    return std::make_unique<StringProperty>(key, value);
}

inline Property::uptr_t make_property(const char *key, int64_t value) {
    return std::make_unique<IntegerProperty>(key, value);
}

inline Property::uptr_t make_property(const char *key, bool value) {
    return std::make_unique<BoolProperty>(key, value);
}

inline Property::uptr_t make_property(const char *key, Value value) {
    return std::make_unique<ValueProperty>(key, value);
}

} // namespace lsim

#endif // LSIM_PROPERTY_H