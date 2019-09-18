// model_property.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Key/value pair to store extra information about specific components

#include "model_property.h"

#include <algorithm>

namespace {

static inline bool string_to_bool(const std::string &val) {
    // tolower should be ok for our use-case. Including ICU in the project for this seems overly complicated.
    auto lower = val;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    return lower == "true" || lower == "yes";   
}

static const char *VALUE_STRINGS[] = {
    "false",
    "true",
    "undefined",
    "error"
};

static inline lsim::Value string_to_value(const std::string &val) {
    // tolower should be ok for our use-case. Including ICU in the project for this seems overly complicated.
    auto lower = val;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    for (int idx = 0; idx < 4; ++idx) {
        if (lower == VALUE_STRINGS[idx]) {
            return static_cast<lsim::Value>(idx);
        }
    }

    return lsim::VALUE_ERROR;
}

static inline lsim::Value int_to_value(int64_t val) {
    if (val >= 0 && val <= 3) {
        return static_cast<lsim::Value>(val);
    } else {
        return lsim::VALUE_ERROR;
    }
}

} // unnamed namespace

namespace lsim {

///////////////////////////////////////////////////////////////////////////////
//
// StringProperty
//

StringProperty::StringProperty(const char *key, const char *value) : 
    Property(key),
    m_value(value) {
}

Property::uptr_t StringProperty::clone() const {
    return make_property(key(), m_value.c_str());
}

std::string StringProperty::value_as_string() const {
    return m_value;
}

int64_t StringProperty::value_as_integer() const {
    return std::strtol(m_value.c_str(), nullptr, 0);
}

bool StringProperty::value_as_boolean() const {
    return string_to_bool(m_value);
}

Value StringProperty::value_as_lsim_value() const {
    return string_to_value(m_value);
}

void StringProperty::value(const char *val) {
    m_value = val;
}

void StringProperty::value(int64_t val) {
    m_value = std::to_string(val);   
}

void StringProperty::value(bool val) {
    m_value = (val) ? "true" : "false";
}

void StringProperty::value(Value val) {
    m_value = VALUE_STRINGS[val];
}

///////////////////////////////////////////////////////////////////////////////
//
// IntegerProperty
//

IntegerProperty::IntegerProperty(const char *key, int64_t value) :
    Property(key),
    m_value(value) {
}

Property::uptr_t IntegerProperty::clone() const {
    return make_property(key(), m_value);
}

std::string IntegerProperty::value_as_string() const {
    return std::to_string(m_value);
}

int64_t IntegerProperty::value_as_integer() const {
    return m_value;   
}

bool IntegerProperty::value_as_boolean() const {
    return m_value != 0;
}

Value IntegerProperty::value_as_lsim_value() const {
    return int_to_value(m_value);
}

void IntegerProperty::value(const char *val) {
    m_value = std::strtoll(val, nullptr, 0);
}

void IntegerProperty::value(int64_t val) {
    m_value = val;
}

void IntegerProperty::value(bool val) {
    m_value = (val) ? 1 : 0;
}

void IntegerProperty::value(Value val) {
    m_value = static_cast<int64_t> (val);
}

///////////////////////////////////////////////////////////////////////////////
//
// BoolProperty
//

BoolProperty::BoolProperty(const char *key, bool value) :
    Property(key),
    m_value(value) {
}

Property::uptr_t BoolProperty::clone() const {
    return make_property(key(), m_value);
}

std::string BoolProperty::value_as_string() const {
    return (m_value) ? "true" : "false";
}

int64_t BoolProperty::value_as_integer() const {
    return (m_value) ? 1 : 0;   
}

bool BoolProperty::value_as_boolean() const {
    return m_value;
}

Value BoolProperty::value_as_lsim_value() const {
    return m_value ? VALUE_TRUE : VALUE_FALSE;
}

void BoolProperty::value(const char *val) {
    m_value = string_to_bool(val);
}

void BoolProperty::value(int64_t val) {
    m_value = val != 0;
}

void BoolProperty::value(bool val) {
    m_value = val;
}

void BoolProperty::value(Value val) {
    m_value = val == VALUE_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// ValueProperty
//

ValueProperty::ValueProperty(const char *key, Value value) :
    Property(key),
    m_value(value) {
}

Property::uptr_t ValueProperty::clone() const {
    return make_property(key(), m_value);
}

std::string ValueProperty::value_as_string() const {
    return VALUE_STRINGS[m_value];
}

int64_t ValueProperty::value_as_integer() const {
    return static_cast<int64_t>(m_value);
}

bool ValueProperty::value_as_boolean() const {
    return m_value == VALUE_TRUE;
}

Value ValueProperty::value_as_lsim_value() const {
    return m_value;
}

void ValueProperty::value(const char *val) {
    m_value = string_to_value(val);
}

void ValueProperty::value(int64_t val) {
    m_value = int_to_value(val);
}

void ValueProperty::value(bool val) {
    m_value = (val) ? VALUE_TRUE : VALUE_FALSE;
}

void ValueProperty::value(Value val) {
    m_value = val;
}

} // namespace lsim