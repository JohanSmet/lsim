#!/usr/bin/env python3

# rom_led_hex.py: ROM that replaces the logic to display a single hexadecimal digit
#   - input : 4 bits (the number to display)
#   - output : 8 bits (the segments to light up)
#
#   +-a-+
#   f   b
#   +-g-+
#   e   c
#   +-d-+ dp
#

import struct


rom_data = [
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

def main():
    write_binary("led_hex.bin", rom_data, 8)

if __name__ == "__main__":
    main()