// property.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Key/value pair to store extra information about specific components

#ifndef LSIM_PROPERTY_H
#define LSIM_PROPERTY_H

#include <string>
#include <memory>

class Property {
public:
    typedef std::unique_ptr<Property> uptr_t;
public:
    Property(const char *key) : m_key(key) {}
    virtual const char *key() const {return m_key.c_str();};

    // value accessors
    virtual std::string value_as_string() const = 0;
    virtual int64_t value_as_integer() const = 0;
    virtual bool value_as_boolean() const = 0;

    // value modifiers
    virtual void value(const char *val) = 0;
    virtual void value(int64_t val) = 0;
    virtual void value(bool val) = 0;

private:
    std::string m_key;
};

class StringProperty : public Property {
public:
    StringProperty(const char *key, const char *value);

    // value accessors
    std::string value_as_string() const override;
    int64_t value_as_integer() const override;
    bool value_as_boolean() const override;

    // value modifiers
    void value(const char *val) override;
    void value(int64_t val)  override;
    void value(bool val) override;

private:
    std::string m_value;
};

class IntegerProperty : public Property {
public:
    IntegerProperty(const char *key, int64_t value);

    // value accessors
    std::string value_as_string() const override;
    int64_t value_as_integer() const override;
    bool value_as_boolean() const override;

    // value modifiers
    void value(const char *val) override;
    void value(int64_t val)  override;
    void value(bool val) override;

private:
    int64_t m_value;
};

class BoolProperty : public Property {
public:
    BoolProperty(const char *key, bool value);

    // value accessors
    std::string value_as_string() const override;
    int64_t value_as_integer() const override;
    bool value_as_boolean() const override;

    // value modifiers
    void value(const char *val) override;
    void value(int64_t val)  override;
    void value(bool val) override;

private:    
    bool m_value;
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


#endif // LSIM_PROPERTY_H