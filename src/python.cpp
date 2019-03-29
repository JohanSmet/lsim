// python.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Python bindings

#include <pybind11/pybind11.h>
#include "simulator.h"
#include "circuit.h"
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
        .def("pins", (const Component::pin_container_t &(Component::*)())&Component::pin)
        .def("pins", (Component::pin_container_t (Component::*)(size_t,size_t))&Component::pin)
        .def("read_pin", &Component::read_pin)
        ;

    py::class_<Connector, Component>(m, "Connector")
        .def("change_data", (void (Connector::*)(uint64_t))&Connector::change_data)
        .def("change_data", (void (Connector::*)(Value))&Connector::change_data)
        .def("change_data", (void (Connector::*)(Component::value_container_t))&Connector::change_data)
        ;

    py::class_<Simulator>(m, "Simulator")
        .def(py::init<>())
        .def("init", &Simulator::init)
        .def("step", &Simulator::step)
        .def("run_until_stable", &Simulator::run_until_stable)

        .def("component_by_name",
                [](const Simulator &sim, const char *name) {
                    return sim.active_circuit()->component_by_name(name);
                })
        .def("connector_by_name",
                [](const Simulator &sim, const char *name) {
                    return dynamic_cast<Connector *>(sim.active_circuit()->component_by_name(name));
                })
        ;

    m.def("load_logisim",(bool (*)(Simulator *, const char *)) &load_logisim);
}