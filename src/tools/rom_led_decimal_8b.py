#!/usr/bin/env python3

# rom_led_decimal_8b.py: ROM that replaces the logic to display a 8-bit value on 4 7-segment LED modules
#   - input : [0-7] = the number to display
#             [8-9] = index of the digit to display (0 = least significant digit)
#   - output : 8 bits (the segments to light up)
#
#   +-a-+
#   f   b
#   +-g-+
#   e   c
#   +-d-+ dp
#

import struct

digit_data = [
#     .gfedcba
    0b00111111, # 0
    0b00000110, # 1
    0b01011011, # 2
    0b01001111, # 3
    0b01100110, # 4
    0b01101101, # 5
    0b01111101, # 6
    0b00000111, # 7
    0b01111111, # 8
    0b01101111, # 9
    0b01110111, # A
    0b01111100, # B
    0b00111001, # C
    0b01011110, # D
    0b01111001, # E
    0b01110001  # F
#     .gfedcba
]

def write_binary(filename, data_list, word_size):
    assert word_size in [8,16,32], "word_size should be 8/16/32"

    # pack data
    data_fmt = "=%sl" if word_size == 32 else "=%sh" if word_size == 16 else "=%sB"
    data_bin = struct.pack(data_fmt % len(data_list), *data_list)

    # write data
    try:
        with open(filename, 'wb') as f:
            f.write(data_bin)
    except IOError as err:
        print(err)
        exit(-1)

def assemble_address(index, number):
    return ((index & 0b11) << 8) | (number & 0b11111111)

def main():
    rom_data = [0 for i in range(2**10)]

    for number in range(256):
        for idx in range(3):
            digit = int(number / 10**idx) % 10
            rom_data[assemble_address(idx, number)] = digit_data[digit]

    write_binary("led_decimal_8b.bin", rom_data, 8)

if __name__ == "__main__":
    main()