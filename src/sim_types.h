// sim_types.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// basic types for the simulator

#ifndef LSIM_SIM_TYPES_H
#define LSIM_SIM_TYPES_H

#include <vector>
#include <cstdint>

namespace lsim {

using node_t = uint32_t;
using pin_t = uint32_t;
using timestamp_t = uint64_t;

enum Value {
    VALUE_FALSE         = 0,
    VALUE_TRUE          = 1,
    VALUE_UNDEFINED     = 2,
    VALUE_ERROR         = 3,
};

using node_container_t = std::vector<node_t>;
using pin_container_t = std::vector<pin_t>;
using value_container_t = std::vector<Value>;

const pin_t PIN_UNDEFINED = static_cast<pin_t>(-1);
const node_t NODE_INVALID = static_cast<node_t>(-1);

// pin-ids are used in the circuit description
using pin_id_t = uint64_t;
using pin_id_container_t = std::vector<pin_id_t>;

// component types - seems okay for now for these to be all listed here, not really expecting much extra types
using ComponentType = uint32_t;
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
const ComponentType COMPONENT_SUB_CIRCUIT = 0x0301;
const ComponentType COMPONENT_TEXT = 0x0401;
const ComponentType COMPONENT_MAX_TYPE_ID = COMPONENT_TEXT;

// component extra data types
struct ExtraDataOscillator {
    timestamp_t  m_next_change;
    int64_t m_duration[2];
};

struct ExtraData7SegmentLED {
    size_t   m_num_samples;
    uint32_t m_samples[8];
};

// forward declarations
class ModelComponent;
class ModelCircuit;

class SimComponent;
class Simulator;

} // namespace lsim

#endif // LSIM_SIM_TYPES_H