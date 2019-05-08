// python.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Python bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lsim_context.h"
#include "basic.h"
#include "load_logisim.h"

namespace py = pybind11;

PYBIND11_MODULE(lsimpy, m) {
    py::enum_<Value>(m, "Value", py::arithmetic())
        .value("ValueFalse", Value::VALUE_FALSE)
        .value("ValueTrue", Value::VALUE_TRUE)
        .value("ValueUndefined", Value::VALUE_UNDEFINED)
        .value("ValueError", Value::VALUE_ERROR)
        .export_values();

    py::class_<Component>(m, "Component")
        .def("pin", &Component::pin)
        .def("num_pins", &Component::num_pins)
        .def("pins", (const Component::pin_container_t &(Component::*)() const)&Component::pins)
        .def("pins", (Component::pin_container_t (Component::*)(size_t,size_t) const)&Component::pins)
        .def("input_pin", &Component::input_pin)
        .def("output_pin", &Component::output_pin)
        .def("control_pin", &Component::control_pin)
        .def("input_pins", &Component::input_pins)
        .def("output_pins", &Component::output_pins)
        .def("control_pins", &Component::control_pins)
        .def("read_pin", &Component::read_pin)
        .def("write_pin", &Component::write_pin)
        .def("write_output_pins", (void (Component::*)(Value))&Component::write_output_pins)
        .def("write_output_pins", (void (Component::*)(value_container_t))&Component::write_output_pins)
        .def("write_output_pins", (void (Component::*)(uint64_t))&Component::write_output_pins)
        ;

    py::class_<Simulator>(m, "Simulator")
        .def(py::init<>())
        .def("step", &Simulator::step)
        .def("run_until_stable", &Simulator::run_until_stable)

        .def("component_by_name",
                [](const Simulator &sim, const char *name) {
                    return sim.active_circuit()->component_by_name(name);
                },
                py::return_value_policy::reference)
        .def("read_nibble", &Simulator::read_nibble)
        .def("read_byte", &Simulator::read_byte)
        ;

    py::class_<CircuitLibrary>(m, "CircuitLibrary")
        .def(py::init<const char *, Simulator *>())
        ;

    py::class_<LSimContext>(m, "LSimContext")
        .def(py::init<>())
        .def("sim", &LSimContext::sim, py::return_value_policy::reference)
        .def("user_library", &LSimContext::user_library, py::return_value_policy::reference)
        ;

    m.def("load_logisim",
            [](LSimContext *lsim_context, const char *name) -> bool {
                auto circuit = load_logisim(lsim_context, name);
                if (!circuit) {
                    return false;
                }

                lsim_context->sim()->init(circuit);
                return true;
            }
    );
}