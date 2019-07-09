#!/usr/bin/env python3

# rom_builder.py: class to create a custom ROM circuit 
#   - 8, 16 or 32bit output
#   - maximum size: 512k bytes
#   - the ROM-circuit does not have a nice visual layout or visible wires

import lsimpy
import math
import struct
import argparse

import sys

class RomBuilder:
    def __init__(self, word_size, word_count):
        assert word_count > 0 and ((word_count & (word_count - 1)) == 0), "word_count should be a power of two"
        assert word_size in [8, 16, 32], "word_size should be 8/16/32"
        assert word_count * word_size / 8 <= 512 * 1024, "ROM is limited to a maximum of 512k bytes"

        self.lsim = lsimpy.LSimContext()
        self.lsim.add_folder("examples", "../../examples")
        self.word_size = word_size
        self.word_count = word_count

        # use word_count as a guide to set initial number of adress lines
        self.addr_lines = int(math.log2(self.word_count))         
    
        # set number of columns depending on word size (always 128 bits wide)
        self.col_count = int(128 / self.word_size)                # number of colums
        self.col_lines = int(math.log2(self.col_count))           # address lines used for columns

        # set number of rows as required, at a mininum 2 (smallest demuxer) and maximum 6 (largest demuxer)
        self.row_lines = max(min(6, self.addr_lines - self.col_lines), 2)     # address lines used for rows   
        self.row_count = 2**self.row_lines

        # set number of segments as required
        self.segment_lines = max(self.addr_lines - self.col_lines - self.row_lines, 0)
        self.segment_count = 2**self.segment_lines

        # total number of address lines might be larger than original guideline for small data sets
        self.addr_lines = self.col_lines + self.row_lines + self.segment_lines

        # prepare multidimensional array to hold 'data' pull low gates
        self.data = [[[[] for r in range(self.row_count)] for c in range(self.col_count)] for s in range(self.segment_count)]

        # precalculate address calculation constants
        self.row_mask = (1 << self.row_lines) - 1
        self.col_shift = self.row_lines
        self.col_mask = ((1 << self.col_lines) - 1) << self.col_shift
        self.seg_shift = self.row_lines + self.col_lines
        self.seg_mask = ((1 << self.segment_lines) - 1) << self.seg_shift

        self.segments = []

    def __split_address(self, address):
        s = (address & self.seg_mask) >> self.seg_shift
        c = (address & self.col_mask) >> self.col_shift
        r = address & self.row_mask
        return s, c, r

    def __prepare_segment(self, segment_id):
        circuit_desc = self.lsim.create_user_circuit("segment_{:04d}".format(segment_id))

        # input connectors
        pin_CE = circuit_desc.add_connector_in("CE", 1, False)
        pin_OE = circuit_desc.add_connector_in("OE", 1, False)
        pin_Addr = circuit_desc.add_connector_in("Addr", self.col_lines + self.row_lines, False)

        # output connectors
        pin_Y = circuit_desc.add_connector_out("Y", self.word_size, False)

        # GND constant
        gnd = circuit_desc.add_constant(lsimpy.ValueFalse)

        # OE should be ignore if CE is low
        OE_CE = circuit_desc.add_and_gate(2)
        circuit_desc.connect(pin_CE.output_pin_id(0), OE_CE.input_pin_id(0))
        circuit_desc.connect(pin_OE.output_pin_id(0), OE_CE.input_pin_id(1))

        # row demuxer
        row_demux = circuit_desc.add_sub_circuit("lib_muxers.demux1to{}".format(self.row_count))

        circuit_desc.connect(pin_CE.output_pin_id(0), row_demux.port_by_name("I"))
        for i in range(0, self.row_lines):
            circuit_desc.connect(pin_Addr.output_pin_id(i), row_demux.port_by_name("Sel[{}]".format(i)))

        # column demuxer
        col_demux = circuit_desc.add_sub_circuit("lib_muxers.demux1to{}".format(self.col_count))

        circuit_desc.connect(OE_CE.output_pin_id(0), col_demux.port_by_name("I"))
        for i in range(0, self.col_lines):
            circuit_desc.connect(pin_Addr.output_pin_id(self.row_lines + i), col_demux.port_by_name("Sel[{}]".format(i)))

        # iterate columns
        col_buffers = []

        for col in range(0, self.col_count):
            # output buffer
            buffer = circuit_desc.add_tristate_buffer(self.word_size)
            col_buffers.append(buffer)

            circuit_desc.connect(buffer.control_pin_id(0), col_demux.port_by_name("O[{}]".format(col)))

            # data lines
            for i in range(0, self.word_size):
                pr = circuit_desc.add_pull_resistor(lsimpy.ValueTrue)
                circuit_desc.connect(pr.output_pin_id(0), buffer.input_pin_id(i))
                circuit_desc.connect(buffer.output_pin_id(i), pin_Y.input_pin_id(i))

        # bundle important variables
        result = {
            "circuit_desc": circuit_desc,
            "GND": gnd,
            "row_demux": row_demux,
            "col_buffers": col_buffers
        }

        return result

    def __prepare_segment_demuxer(self, circuit_desc):
        if self.segment_lines <= 6:
            # 1 demuxer is suffices
            seg_demux = circuit_desc.add_sub_circuit("lib_muxers.demux1to{}".format(self.segment_count))

            circuit_desc.connect(circuit_desc.port_by_name("CE"), seg_demux.port_by_name("I"))
            for i in range(self.segment_lines):
                addr = self.row_lines + self.col_lines + i
                port = seg_demux.port_by_name("Sel[{}]".format(i))
                if lsimpy.pin_id_invalid(port):
                    port = seg_demux.port_by_name("Sel")
                circuit_desc.connect(circuit_desc.port_by_name("Addr[{}]".format(addr)), port)
            self.segment_enables = [seg_demux.port_by_name("O[{}]".format(s)) for s in range(self.segment_count)]
        elif self.segment_lines <= 9:
            # 2 levels of demuxers
            s1_lines = self.segment_lines - 6
            s2_count = 2**s1_lines
            
            # first level demuxer
            s1_demux = circuit_desc.add_sub_circuit("lib_muxers.demux1to{}".format(s2_count))
            circuit_desc.connect(circuit_desc.port_by_name("CE"), s1_demux.port_by_name("I"))

            for i in range(s1_lines):
                addr = self.row_lines + self.col_lines + 6 + i
                port = s1_demux.port_by_name("Sel[{}]".format(i))
                if lsimpy.pin_id_invalid(port):
                    port = s1_demux.port_by_name("Sel")
                circuit_desc.connect(circuit_desc.port_by_name("Addr[{}]".format(addr)), port)

            # second level demuxers
            self.segment_enables = []

            for s in range(s2_count):
                s2_demux = circuit_desc.add_sub_circuit("lib_muxers.demux1to64")
                circuit_desc.connect(s1_demux.port_by_name("O[{}]".format(s)), s2_demux.port_by_name("I"))
                for i in range(6):
                    addr = self.row_lines + self.col_lines + i
                    port = s2_demux.port_by_name("Sel[{}]".format(i))
                    circuit_desc.connect(circuit_desc.port_by_name("Addr[{}]".format(addr)), port)
                self.segment_enables += [s2_demux.port_by_name("O[{}]".format(s)) for s in range(64)]
        else:
            print("Too many segments")
            exit(-1)

    def prepare_rom(self):
        self.lsim.user_library().clear_circuits()
        self.lsim.load_reference_library("lib_muxers", "examples/cpu_8bit/lib_muxers.lsim")
        self.lsim.user_library().add_reference("lib_muxers")

        # prepare segments
        for s in range(self.segment_count):
            segment = self.__prepare_segment(s)
            self.segments.append(segment)

        # build main circuit
        if self.segment_count == 1:
            self.lsim.user_library().rename_circuit(self.segments[0]["circuit_desc"], "rom")
            self.lsim.user_library().change_main_circuit("rom")
            return

        # create wrapper for multiple segments
        circuit_desc = self.lsim.create_user_circuit("rom")
        self.lsim.user_library().change_main_circuit("rom")

        # >> input connectors
        pin_CE = circuit_desc.add_connector_in("CE", 1, False)
        pin_OE = circuit_desc.add_connector_in("OE", 1, False)
        pin_Addr = circuit_desc.add_connector_in("Addr", self.addr_lines, False)

        # >> output connectors
        pin_Y = circuit_desc.add_connector_out("Y", self.word_size, False)

        # >> segment demuxer 
        self.__prepare_segment_demuxer(circuit_desc)

        # >> integrate all segments into the circuit
        for s in range(self.segment_count):
            rom_seg = circuit_desc.add_sub_circuit(self.segments[s]["circuit_desc"].name())
            circuit_desc.connect(self.segment_enables[s], rom_seg.port_by_name("CE"))
            circuit_desc.connect(pin_OE.output_pin_id(0), rom_seg.port_by_name("OE"))
            for i in range(self.row_lines + self.col_lines):
                circuit_desc.connect(pin_Addr.output_pin_id(i), rom_seg.port_by_name("Addr[{}]".format(i)))
            for o in range(self.word_size):
                circuit_desc.connect(pin_Y.input_pin_id(o), rom_seg.port_by_name("Y[{}]".format(o)))

    def clear_location(self, address):
        s, c, r = self.__split_address(address)
        pull_downs = self.data[s][c][r]
        for pd in pull_downs:
            self.circuit_desc.remove_component(pd.id())
        pull_downs.clear()

    def set_location(self, address, data_word):
        self.clear_location(address)
        s, c, r = self.__split_address(address)

        segment = self.segments[s]

        for i in range(self.word_size):
            if (data_word >> i) & 1 == 0:
                pd = segment["circuit_desc"].add_tristate_buffer(1)
                segment["circuit_desc"].connect(segment["GND"].output_pin_id(0), pd.input_pin_id(0))
                segment["circuit_desc"].connect(segment["row_demux"].port_by_name("O[{}]".format(r)), pd.control_pin_id(0))
                segment["circuit_desc"].connect(segment["col_buffers"][c].input_pin_id(i), pd.output_pin_id(0))

    def export_library(self, filename):
        self.lsim.save_user_library(filename)

    def __unpack_data(self, data_bin):
        data_fmt = "=%il" if self.word_size == 32 else "=%ih" if self.word_size == 16 else "=%iB"
        data_len = len(data_bin) / (self.word_size / 8)
        data = struct.unpack(data_fmt % data_len, data_bin)
        return data

    def write(self, data_bin):
        data = self.__unpack_data(data_bin)
        limit = min(len(data), self.word_count)
        for i in range(limit):
            self.set_location(i, data[i])

    def verify(self, data_bin):
        data = self.__unpack_data(data_bin)

        sim = self.lsim.sim()
        circuit_desc = self.lsim.user_library().main_circuit()
        circuit = circuit_desc.instantiate(sim)

        sim.init()

        pins_Addr = [circuit_desc.port_by_name("Addr[{}]".format(i)) for i in range(self.addr_lines)]
        pins_Y = [circuit_desc.port_by_name("Y[{}]".format(i)) for i in range(self.word_size)]

        circuit.write_port("CE", lsimpy.ValueTrue)
        circuit.write_port("OE", lsimpy.ValueTrue)

        for loc in (range(min(len(data), self.word_count))):
            circuit.write_pins(pins_Addr, loc)
            sim.run_until_stable(5)
            value = circuit.read_byte(pins_Y)      # XXX read_pins or something
            if value != data[loc]:
                print("Verify: ERROR at location {}: is {} but expected {}".format(loc,value, data[loc]))

def read_binary(filename):
    try:
        with open(filename, 'rb') as f:
            data_in = f.read()
            return data_in
    except IOError as err:
        print(err)
        exit(-1)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("circuit_file", help="filename of the ROM-circuit to be created")
    parser.add_argument("data_file", help="filename of binary data to write to the ROM-circuit")
    parser.add_argument("-w", "--word-size", type=int, choices=[8,16,32], default=8,
                        help="number of bits in the output of the ROM")
    args = parser.parse_args()

    # read input data
    print ("*** reading data")
    data_in = read_binary(args.data_file)
    print ("data size = {} bytes".format(len(data_in)))

    # calculate required ROM size (rounding to a power of 2)
    word_count = int(len(data_in) / (args.word_size / 8))
    word_count = 1 << (word_count-1).bit_length()

    # create rom
    print ("*** preparing ROM")
    print ("word-size = {} - word-count = {}".format(args.word_size, word_count))
    rom = RomBuilder(args.word_size, word_count)
    rom.prepare_rom()

    print ("*** programming ROM")
    rom.write(data_in)

    print ("*** exporting ROM-circuit")
    rom.export_library(args.circuit_file)

    print ("*** verifying ROM")
    rom.verify(data_in)

def test():
    # simple test routine
    rom = RomBuilder(8, 1024)

    print ("*** preparing data")
    data_mask = (1 << rom.word_size) - 1
    data_list = [i & data_mask for i in range(rom.word_count)]
    data_fmt = "=%sl" if rom.word_size == 32 else "=%sh" if rom.word_size == 16 else "=%sB"
    data_bin = struct.pack(data_fmt % len(data_list) , *data_list)

    print ("*** preparing ROM")
    rom.prepare_rom()

    print ("*** writing ROM")
    rom.write(data_bin)

    #print ("*** saving ROM")
    #rom.export_library("examples/rom_scripted.lsim")

    print ("*** verifying ROM")
    rom.verify(data_bin)

if __name__ == "__main__":
    main()