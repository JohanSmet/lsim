#!/usr/bin/env python3

# rom_8bit_common.py - Johan Smet - BSD-3-Clause (see LICENSE)
#   common definitions for the minimal 8-bit computer

import struct

# OPCODES
OPCODE_NOP = 0b00000    # (0) No Operation
OPCODE_LDA = 0b00001    # (1) Load Register-A from memory
OPCODE_LDB = 0b00010    # (2) Load Register-B from memory
OPCODE_LTA = 0b00011    # (3) Load literal into Register-A
OPCODE_LTB = 0b00100    # (4) Load literal into Register-B
OPCODE_STA = 0b00101    # (5) Store from Register-A to memory    
OPCODE_STB = 0b00110    # (6) Store from Register-B to memory    
OPCODE_PRA = 0b00111    # (7) Output Register-A to display
OPCODE_JMP = 0b01000    # (8) Jump to address
OPCODE_JNZ = 0b01001    # (9) Conditional Jump (zero-flag NOT set) to address
OPCODE_JZ  = 0b01010    # (10) Conditional Jump (zero-flag SET) to address
OPCODE_JC  = 0b01100    # (12) Conditional Jump (carry-flag SET) to address
OPCODE_HLT = 0b01111    # (15) Halt CPU

OPCODE_ADD = 0b10000    # (16) Add register-B to register-A (result in register-A)
OPCODE_SUB = 0b10001    # (17) Subtract register-B from register-A (result in register-A)

def write_binary(filename, data_list, word_size):
    assert word_size in [8,16,32], "word_size should be 8/16/32"

    # pack data
    data_fmt = "=%sL" if word_size == 32 else "=%sH" if word_size == 16 else "=%sB"
    data_bin = struct.pack(data_fmt % len(data_list), *data_list)

    # write data
    try:
        with open(filename, 'wb') as f:
            f.write(data_bin)
    except IOError as err:
        print(err)
        exit(-1)