#!/usr/bin/env python3

# rom_8bit_program.py: generate program ROM for the minimal 8-but computer

import rom_8bit_common as c

rom_data = []

def emit_literal(raw):
    pc = len(rom_data)
    rom_data.append(raw & 0xff)
    return pc

def emit_op(op, addr = 0):
    raw = (op & 0b00011111) | ((addr & 0b00000111) << 5)
    return emit_literal(raw)

def main():
    emit_op(c.OPCODE_LTA)
    emit_literal(15)
    emit_op(c.OPCODE_LTB)
    emit_literal(1)
    emit_op(c.OPCODE_PRA)
    emit_op(c.OPCODE_ADD)
    emit_op(c.OPCODE_PRA)
    emit_op(c.OPCODE_HLT)
    for i in range(len(rom_data), 256):
        rom_data.append(0xff)
    c.write_binary("prog_8bit.bin", rom_data, 8)

if __name__ == "__main__":
    main()