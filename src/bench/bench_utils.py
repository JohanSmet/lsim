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