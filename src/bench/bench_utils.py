#!/usr/bin/env python3

count_check = 0
count_failure = 0

def CHECK(was, expected, op_str):
    global count_check, count_failure
    count_check = count_check + 1
    if was != expected:
        count_failure = count_failure + 1
        print("FAILURE: {} = {} (expected {})".format(op_str, was, expected))

def print_stats():
    global count_check, count_failure
    print("======================================================")
    if count_failure == 0:
        print("All tests passed ({} checks executed)".format(count_check))
    else:
        print("{} out of {} checks failed!".format(count_failure, count_check))

def instantiate_circuit(lsim, name):
    circuit_desc = lsim.user_library().circuit_by_name(name)
    circuit = circuit_desc.instantiate(lsim.sim())
    lsim.sim().init()
    return circuit

def run_thruth_table(lsim, circuit_name, truth_table):
    print ("* Testing {}".format(circuit_name))

    circuit = instantiate_circuit(lsim, circuit_name)
    for test in truth_table:
        for v in test[0].items():
            circuit.write_port(v[0], v[1])
        lsim.sim().run_until_stable(5)
        for v in test[1].items():
            CHECK(circuit.read_port(v[0]), v[1], "{}: {}".format(circuit_name, test[0]))