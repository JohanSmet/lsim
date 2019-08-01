#!/usr/bin/env python3

# rom_8bit_ctrl.py: microcode ROM for the control logic module of the minimal 8-bit computer
#   - input :   [0-2] = microcode sub step
#               [3-7] = opcode
#               [8-8] = zero flag
#               [9-9] = carry flag
#   - output : 16 bits = the control signals
#               [00] = AI  : register-A input enable
#               [01] = AO  : register-A output enable
#               [02] = BI  : register-B input enable
#               [03] = BO  : register-B output enable
#               [04] = RI  : RAM input enable
#               [05] = RO  : RAM output enable
#               [06] = INC : increment program counter
#               [07] = JMP : load program counter
#               [08] = IF  : Instruction decoder - instruction fetch
#               [09] = DL  : Instruction decoder - data load
#               [10] = DO  : Display Output
#               [11] = HLT : stop clock
#               [12] = CE  : ALU - enable compute
#               [13] = CO  : ALU - output compute result to databus
#               [14] = free - not used
#               [15] = RS  : reset control logic counter
#

import rom_8bit_common as c

SIGNAL_AI  = 1 << 0
SIGNAL_AO  = 1 << 1
SIGNAL_BI  = 1 << 2
SIGNAL_BO  = 1 << 3
SIGNAL_RI  = 1 << 4
SIGNAL_RO  = 1 << 5
SIGNAL_INC = 1 << 6
SIGNAL_JMP = 1 << 7
SIGNAL_IF  = 1 << 8
SIGNAL_DL  = 1 << 9
SIGNAL_DO  = 1 << 10
SIGNAL_HLT = 1 << 11
SIGNAL_CE  = 1 << 12
SIGNAL_CO  = 1 << 13
SIGNAL_RS  = 1 << 15

rom_data = [0 for i in range(2**10)]
cur_addr = {}

def addr(op, step, zf = 0, cf = 0):
    return (step & 0b111) + ((op & 0b11111) << 3) + ((zf & 1) << 8) + ((cf & 1) << 9)

def emit(signals):
    for zf in cur_addr['zf']:
        for cf in cur_addr['cf']:
            rom_data[addr(cur_addr['op'], cur_addr['step'], zf, cf)] = signals
    cur_addr['step'] += 1

def opcode_start(op, zf = [0,1], cf = [0,1]):
    cur_addr['op'] = op
    cur_addr['step'] = 0
    cur_addr['cf'] = cf
    cur_addr['zf'] = zf

    emit(SIGNAL_IF)
    emit(SIGNAL_INC)

def opcode_NOP():
    opcode_start(c.OPCODE_NOP)
    emit(SIGNAL_RS)

def opcode_LDA():
    opcode_start(c.OPCODE_LDA)
    emit(SIGNAL_AI | SIGNAL_RO)
    emit(SIGNAL_RS)

def opcode_LDB():
    opcode_start(c.OPCODE_LDB)
    emit(SIGNAL_BI | SIGNAL_RO)
    emit(SIGNAL_RS)

def opcode_LTA():
    opcode_start(c.OPCODE_LTA)
    emit(SIGNAL_DL | SIGNAL_AI)
    emit(SIGNAL_INC)
    emit(SIGNAL_RS)

def opcode_LTB():
    opcode_start(c.OPCODE_LTB)
    emit(SIGNAL_DL | SIGNAL_BI)
    emit(SIGNAL_INC)
    emit(SIGNAL_RS)

def opcode_PRA():
    opcode_start(c.OPCODE_PRA)
    emit(SIGNAL_AO | SIGNAL_DO)
    emit(SIGNAL_RS)

def opcode_STA():
    opcode_start(c.OPCODE_STA)
    emit(SIGNAL_AO | SIGNAL_RI)
    emit(SIGNAL_RS)

def opcode_STB():
    opcode_start(c.OPCODE_STB)
    emit(SIGNAL_BO | SIGNAL_RI)
    emit(SIGNAL_RS)

def opcode_HLT():
    opcode_start(c.OPCODE_HLT)
    emit(SIGNAL_HLT)
    emit(SIGNAL_RS)

def opcode_alu(op):
    opcode_start(op)
    emit(SIGNAL_CE)
    emit(SIGNAL_CE | SIGNAL_CO | SIGNAL_AI)

def main():
    opcode_NOP()
    opcode_LDA()
    opcode_LDB()
    opcode_LTA()
    opcode_LTB()
    opcode_PRA()
    opcode_STA()
    opcode_STB()
    opcode_HLT()
    opcode_alu(c.OPCODE_ADD)
    opcode_alu(c.OPCODE_SUB)
    c.write_binary("ctrl_8bit.bin", rom_data, 16)

if __name__ == "__main__":
    main()