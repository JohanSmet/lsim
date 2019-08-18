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

def emit_op_data(op, data):
    pc = emit_op(op)
    emit_literal(data)
    return pc

def prog_counter():
    emit_op_data(c.OPCODE_LTA, 0)   # move 0 into register-A
    emit_op_data(c.OPCODE_LTB, 1)   # move 1 into register-B
    emit_op(c.OPCODE_PRA)           # display register-A
    l = emit_op(c.OPCODE_ADD)       # add register-B to register-A
    emit_op(c.OPCODE_PRA)           # display register-A
    emit_op_data(c.OPCODE_JMP, l)   # jump to label-l
    emit_op(c.OPCODE_HLT)           # halt CPU (never reached but still :-)

def prog_count_down():
    emit_op_data(c.OPCODE_LTA, 255) # move start value into register-A
    emit_op_data(c.OPCODE_LTB, 1)   # move 1 into register-B
    l = emit_op(c.OPCODE_PRA)       # display register-A
    emit_op(c.OPCODE_SUB)           # subtract register-B from register-A
    emit_op_data(c.OPCODE_JMP, l)   # jump to label-l
    emit_op(c.OPCODE_HLT)           # halt CPU (never reached but still :-)

def prog_count_up_down():
    emit_op_data(c.OPCODE_LTA, 0)   # (0+1) move 0 into register-A
    emit_op_data(c.OPCODE_LTB, 1)   # (2+3) move 1 into register-B
    l = emit_op(c.OPCODE_ADD)       # (4) add register-B to register-A
    emit_op_data(c.OPCODE_JC, 10)   # (5+6) jump to second loop if carry flag is set
    emit_op(c.OPCODE_PRA)           # (7) display register-A
    emit_op_data(c.OPCODE_JMP, l)   # (8+9) jump to label-l
    k = emit_op(c.OPCODE_SUB)       # (10) subtract register-B from register-A
    emit_op(c.OPCODE_PRA)           # (11) display register-A
    emit_op_data(c.OPCODE_JZ, l)    # (12+13) jump to first loop if zero flag is set
    emit_op_data(c.OPCODE_JMP, k)   # (14+15) jump to label-k
    emit_op(c.OPCODE_HLT)           # (16) halt CPU (never reached but still :-)

def prog_fibonnaci():
    # initialize memory
    emit_op_data(c.OPCODE_LTA, 0)   # move 0 into register-A
    emit_op(c.OPCODE_STA, 1)        # store register-A to mem[1]
    emit_op_data(c.OPCODE_LTA, 1)   # move 1 into register-A
    emit_op(c.OPCODE_STA, 2)        # store register-A to mem[2]
    emit_op_data(c.OPCODE_LTA, 14)  # move number of iterations into register-A
    emit_op(c.OPCODE_STA, 3)        # store register-A to mem[3]
    # fibonnaci loop
    l = emit_op(c.OPCODE_LDA, 1)    # load register-A from mem[1]
    emit_op(c.OPCODE_PRA)           # display register-A
    emit_op(c.OPCODE_LDB, 2)        # load register-B from mem[2]
    emit_op(c.OPCODE_ADD)           # add register-B to register-A
    emit_op(c.OPCODE_STB, 1)        # store register-B to mem[1]
    emit_op(c.OPCODE_STA, 2)        # store register-A to mem[2]
    emit_op(c.OPCODE_LDA, 3)        # load loop-counter into register-A
    emit_op_data(c.OPCODE_LTB, 1)   # move 1 into register-B
    emit_op(c.OPCODE_SUB)           # substract register-B from register-A
    emit_op(c.OPCODE_STA, 3)        # store loop-counter 
    emit_op_data(c.OPCODE_JZ, 0)    # jump to initialization when done
    emit_op_data(c.OPCODE_JMP, l)   # jump to label-l
    emit_op(c.OPCODE_HLT)           # halt CPU 

def fill_rom(size):
    for i in range(len(rom_data), size):
        rom_data.append(0xff)

def main():
    prog_count_up_down()
    fill_rom(256)
    prog_fibonnaci()
    fill_rom(512)
    c.write_binary("prog_8bit.bin", rom_data, 8)

if __name__ == "__main__":
    main()