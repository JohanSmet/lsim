// property.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Key/value pair to store extra information about specific components

#include "property.h"

#include <algorithm>

static inline bool string_to_bool(const std::string &val) {
    // tolower should be ok for our use-case. Including ICU in the project for this seems overly complicated.
    auto lower = val;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    return lower == "true" || lower == "yes";   
}

///////////////////////////////////////////////////////////////////////////////
//
// StringProperty
//

StringProperty::StringProperty(const char *key, const char *value) : 
    Property(key),
    m_value(value) {

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

void StringProperty::value(const char *val) {
    m_value = val;
}

void StringProperty::value(int64_t val) {
    m_value = std::to_string(val);   
}

void StringProperty::value(bool val) {
    m_value = (val) ? "true" : "false";
}

///////////////////////////////////////////////////////////////////////////////
//
// IntegerProperty
//

IntegerProperty::IntegerProperty(const char *key, int64_t value) :
    Property(key),
    m_value(value) {
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

void IntegerProperty::value(const char *val) {
    m_value = std::strtoll(val, nullptr, 0);
}

void IntegerProperty::value(int64_t val) {
    m_value = val;
}

void IntegerProperty::value(bool val) {
    m_value = (val) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// BoolProperty
//

BoolProperty::BoolProperty(const char *key, bool value) :
    Property(key),
    m_value(value) {
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

void BoolProperty::value(const char *val) {
    m_value = string_to_bool(val);
}

void BoolProperty::value(int64_t val) {
    m_value = val != 0;
}

void BoolProperty::value(bool val) {
    m_value = val;
}

