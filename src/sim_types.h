// sim_types.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// basic types for the simulator

#ifndef LSIM_SIM_TYPES_H
#define LSIM_SIM_TYPES_H

#include <vector>

namespace lsim {

typedef uint32_t node_t;
typedef uint32_t pin_t;

enum Value {
    VALUE_FALSE         = 0,
    VALUE_TRUE          = 1,
    VALUE_UNDEFINED     = 2,
    VALUE_ERROR         = 3,
};

enum Priority {
    PRIORITY_NORMAL     = 0,
    PRIORITY_DEFERRED   = 1
};

typedef std::vector<node_t> node_container_t;
typedef std::vector<pin_t> pin_container_t;
typedef std::vector<Value> value_container_t;

const pin_t PIN_UNDEFINED = static_cast<pin_t>(-1);
const node_t NODE_INVALID = static_cast<node_t>(-1);

// pin-ids are used in the circuit description
typedef uint64_t pin_id_t;
typedef std::vector<pin_id_t>   pin_id_container_t;

// component types - seems okay for now for these to be all listed here, not really expecting much extra types
typedef uint32_t ComponentType;
const ComponentType COMPONENT_CONNECTOR_IN = 0x0001;
const ComponentType COMPONENT_CONNECTOR_OUT = 0x0002;
const ComponentType COMPONENT_CONSTANT = 0x0003;
const ComponentType COMPONENT_PULL_RESISTOR = 0x0004;
const ComponentType COMPONENT_BUFFER = 0x0011;
const ComponentType COMPONENT_TRISTATE_BUFFER = 0x0012;
const ComponentType COMPONENT_AND_GATE = 0x0013;
const ComponentType COMPONENT_OR_GATE = 0x0014;
const ComponentType COMPONENT_NOT_GATE = 0x0015;
const ComponentType COMPONENT_NAND_GATE = 0x0016;
const ComponentType COMPONENT_NOR_GATE = 0x0017;
const ComponentType COMPONENT_XOR_GATE = 0x0018;
const ComponentType COMPONENT_XNOR_GATE = 0x0019;
const ComponentType COMPONENT_VIA = 0x0020;
const ComponentType COMPONENT_OSCILLATOR = 0x0021;
const ComponentType COMPONENT_7_SEGMENT_LED = 0x0101;
const ComponentType COMPONENT_SUB_CIRCUIT = 0x1001;
const ComponentType COMPONENT_TEXT = 0x2001;

// forward declarations
class Component;
class CircuitDescription;

class SimComponent;
class Simulator;

} // namespace lsim

#endif // LSIM_SIM_TYPES_H