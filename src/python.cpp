// python.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Python bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lsim_context.h"
#include "model_circuit.h"
#include "circuit_instance.h"
#include "serialize.h"

namespace py = pybind11;
using namespace lsim;

PYBIND11_MODULE(lsimpy, m) {
    m.def("pin_id_invalid", [](pin_id_t pin) -> bool {return pin == PIN_ID_INVALID;});

    py::enum_<Value>(m, "Value", py::arithmetic())
        .value("ValueFalse", lsim::Value::VALUE_FALSE)
        .value("ValueTrue", Value::VALUE_TRUE)
        .value("ValueUndefined", Value::VALUE_UNDEFINED)
        .value("ValueError", Value::VALUE_ERROR)
        .export_values()
    ;

    py::class_<Point>(m, "Point")
        .def(py::init<float, float>())
        .def("x", [](Point *point) -> float { return point->x; })
        .def("y", [](Point *point) -> float { return point->y; })
        ;

    py::class_<ModelComponent>(m, "ModelComponent")
        .def("id", &ModelComponent::id)
        .def("type", &ModelComponent::type)
        .def("nested_circuit", &ModelComponent::nested_circuit, py::return_value_policy::reference)
        .def("num_inputs", &ModelComponent::num_inputs)
        .def("num_outputs", &ModelComponent::num_outputs)
        .def("num_controls", &ModelComponent::num_controls)
        .def("input_pin_id", &ModelComponent::input_pin_id)
        .def("output_pin_id", &ModelComponent::output_pin_id)
        .def("control_pin_id", &ModelComponent::control_pin_id)
        .def("port_by_name", &ModelComponent::port_by_name)
        .def("position", &ModelComponent::position)
        .def("set_position", &ModelComponent::set_position)
        .def("angle", &ModelComponent::angle)
        .def("set_angle", &ModelComponent::set_angle)
        ;

    py::class_<ModelWire>(m, "ModelWire")
        .def("id", &ModelWire::id)
        .def("add_pin", &ModelWire::add_pin)
        .def("num_pins", &ModelWire::num_pins)
        .def("pin", &ModelWire::pin)
        ;

    py::class_<ModelCircuit>(m, "ModelCircuit")
        .def("name", &ModelCircuit::name)
        .def("change_name", &ModelCircuit::change_name)
        .def("disconnect_component", &ModelCircuit::disconnect_component)
        .def("remove_component", &ModelCircuit::remove_component)
        .def("add_connector_in", &ModelCircuit::add_connector_in, py::return_value_policy::reference)
        .def("add_connector_out", &ModelCircuit::add_connector_out, py::return_value_policy::reference)
        .def("add_constant", &ModelCircuit::add_constant, py::return_value_policy::reference)
        .def("add_pull_resistor", &ModelCircuit::add_pull_resistor, py::return_value_policy::reference)
        .def("add_buffer", &ModelCircuit::add_buffer, py::return_value_policy::reference)
        .def("add_tristate_buffer", &ModelCircuit::add_tristate_buffer, py::return_value_policy::reference)
        .def("add_and_gate", &ModelCircuit::add_and_gate, py::return_value_policy::reference)
        .def("add_or_gate", &ModelCircuit::add_or_gate, py::return_value_policy::reference)
        .def("add_not_gate", &ModelCircuit::add_not_gate, py::return_value_policy::reference)
        .def("add_nand_gate", &ModelCircuit::add_nand_gate, py::return_value_policy::reference)
        .def("add_nor_gate", &ModelCircuit::add_nor_gate, py::return_value_policy::reference)
        .def("add_xor_gate", &ModelCircuit::add_xor_gate, py::return_value_policy::reference)
        .def("add_xnor_gate", &ModelCircuit::add_xnor_gate, py::return_value_policy::reference)
        .def("add_sub_circuit", (ModelComponent *(ModelCircuit::*)(const char *))&ModelCircuit::add_sub_circuit, py::return_value_policy::reference)
        .def("create_wire", &ModelCircuit::create_wire, py::return_value_policy::reference)
        .def("connect", &ModelCircuit::connect, py::return_value_policy::reference)
        .def("remove_wire", &ModelCircuit::remove_wire)
        .def("port_by_name", &ModelCircuit::port_by_name)
        .def("instantiate", &ModelCircuit::instantiate, py::arg("sim"), py::arg("top_level") = true)
    ;

    py::class_<Simulator>(m, "Simulator")
        .def(py::init<>())
        .def("init", &Simulator::init)
        .def("step", &Simulator::step)
        .def("run_until_stable", &Simulator::run_until_stable)
        ;

    py::class_<CircuitLibrary>(m, "CircuitLibrary")
        .def(py::init<const char *>())
        .def("main_circuit", &CircuitLibrary::main_circuit, py::return_value_policy::reference)
        .def("change_main_circuit", &CircuitLibrary::change_main_circuit)
        .def("circuit_by_name", &CircuitLibrary::circuit_by_name, py::return_value_policy::reference)
        .def("rename_circuit", &CircuitLibrary::rename_circuit)
        .def("clear_circuits", &CircuitLibrary::clear_circuits)
        .def("add_reference", &CircuitLibrary::add_reference)
        ;

    py::class_<LSimContext>(m, "LSimContext")
        .def(py::init<>())
        .def("sim", &LSimContext::sim, py::return_value_policy::reference)
        .def("user_library", &LSimContext::user_library, py::return_value_policy::reference)
        .def("create_user_circuit", &LSimContext::create_user_circuit, py::return_value_policy::reference)
        .def("load_user_library",
                [](LSimContext *context, const char *name) -> bool {
                    return deserialize_library(context, context->user_library(), context->full_file_path(name).c_str());
                })
        .def("save_user_library",
                [](LSimContext *context, const char *name) -> bool {
                    return serialize_library(context, context->user_library(), context->full_file_path(name).c_str());
                })
        .def("load_reference_library", &LSimContext::load_reference_library)
        .def("add_folder", &LSimContext::add_folder)
        ;

    py::class_<CircuitInstance>(m, "CircuitInstance")
        .def("read_pin", &CircuitInstance::read_pin)
        .def("read_nibble", (uint8_t (CircuitInstance::*)(uint32_t)) &CircuitInstance::read_nibble)
        .def("read_nibble", (uint8_t (CircuitInstance::*)(const pin_id_container_t &)) &CircuitInstance::read_nibble)
        .def("read_byte", (uint8_t (CircuitInstance::*)(uint32_t)) &CircuitInstance::read_byte)
        .def("read_byte", (uint8_t (CircuitInstance::*)(const pin_id_container_t &)) &CircuitInstance::read_byte)
        .def("read_pins", &CircuitInstance::read_pins)
        .def("write_pin", &CircuitInstance::write_pin)
        .def("write_output_pins", (void (CircuitInstance::*)(uint32_t, value_container_t)) &CircuitInstance::write_output_pins)
        .def("write_output_pins", (void (CircuitInstance::*)(uint32_t, uint64_t)) &CircuitInstance::write_output_pins)
        .def("write_output_pins", (void (CircuitInstance::*)(uint32_t, Value)) &CircuitInstance::write_output_pins)
        .def("write_nibble", &CircuitInstance::write_nibble)
        .def("write_byte", &CircuitInstance::write_byte)
        .def("write_pins", (void (CircuitInstance::*)(const pin_id_container_t &, const value_container_t&))&CircuitInstance::write_pins)
        .def("write_pins", (void (CircuitInstance::*)(const pin_id_container_t &, uint64_t))&CircuitInstance::write_pins)
        .def("write_port",
                [](CircuitInstance *circuit, const char *port, Value value) {
                    circuit->write_pin(circuit->description()->port_by_name(port), value);
                })
        .def("read_port",
                [](CircuitInstance *circuit, const char *port) -> Value {
                    return circuit->read_pin(circuit->description()->port_by_name(port));
                })
        
        ;
    
}