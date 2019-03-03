#include "catch.hpp"
#include "load_logisim.h"
#include "circuit.h"

#include <cstring>

const char *logisim_test_data = R"FILE(
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<project source="2.15.0" version="1.0">
This file is intended to be loaded by Logisim-evolution (https://github.com/reds-heig/logisim-evolution).
<lib desc="#Wiring" name="0"/>
  <lib desc="#Gates" name="1"/>
  <lib desc="#Plexers" name="2">
    <tool name="Multiplexer">
      <a name="enable" val="false"/>
    </tool>
    <tool name="Demultiplexer">
      <a name="enable" val="false"/>
    </tool>
  </lib>
  <lib desc="#Arithmetic" name="3"/>
  <lib desc="#Memory" name="4">
    <tool name="ROM">
      <a name="contents">addr/data: 8 8
0
</a>
    </tool>
  </lib>
  <lib desc="#I/O" name="5"/>
  <lib desc="#TTL" name="6"/>
  <main name="test"/>
  <options>
    <a name="gateUndefined" val="ignore"/>
    <a name="simlimit" val="1000"/>
    <a name="simrand" val="0"/>
    <a name="tickmain" val="half_period"/>
  </options>
  <mappings>
    <tool lib="9" map="Button2" name="Menu Tool"/>
    <tool lib="9" map="Button3" name="Menu Tool"/>
    <tool lib="9" map="Ctrl Button1" name="Menu Tool"/>
  </mappings>
  <toolbar>
    <tool lib="9" name="Poke Tool"/>
    <tool lib="9" name="Edit Tool"/>
    <tool lib="9" name="Text Tool">
      <a name="text" val=""/>
      <a name="font" val="SansSerif plain 12"/>
      <a name="halign" val="center"/>
      <a name="valign" val="base"/>
    </tool>
    <sep/>
    <tool lib="0" name="Pin"/>
    <tool lib="0" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
    </tool>
    <tool lib="1" name="NOT Gate"/>
    <tool lib="1" name="AND Gate"/>
    <tool lib="1" name="OR Gate"/>
  </toolbar>
  <circuit name="test">
    <a name="circuit" val="test"/>
    <a name="clabel" val=""/>
    <a name="clabelup" val="east"/>
    <a name="clabelfont" val="SansSerif bold 16"/>
    <a name="circuitnamedbox" val="true"/>
    <a name="circuitnamedboxfixedsize" val="true"/>
    <a name="circuitvhdlpath" val=""/>
    <wire from="(190,170)" to="(220,170)"/>
    <wire from="(240,170)" to="(270,170)"/>
    <wire from="(300,170)" to="(320,170)"/>
    <comp lib="1" loc="(240,170)" name="Buffer"/>
    <comp lib="0" loc="(190,170)" name="Pin">
      <a name="label" val="In"/>
    </comp>
    <comp lib="1" loc="(300,170)" name="NOT Gate"/>
    <comp lib="0" loc="(320,170)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="Out"/>
    </comp>
  </circuit>
</project>
)FILE";

const char *logisim_adder_data = R"FILE(
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<project source="2.15.0" version="1.0">
This file is intended to be loaded by Logisim-evolution (https://github.com/reds-heig/logisim-evolution).
<lib desc="#Wiring" name="0"/>
  <lib desc="#Gates" name="1"/>
  <lib desc="#Plexers" name="2">
    <tool name="Multiplexer">
      <a name="enable" val="false"/>
    </tool>
    <tool name="Demultiplexer">
      <a name="enable" val="false"/>
    </tool>
  </lib>
  <lib desc="#Arithmetic" name="3"/>
  <lib desc="#Memory" name="4">
    <tool name="ROM">
      <a name="contents">addr/data: 8 8
0
</a>
    </tool>
  </lib>
  <lib desc="#I/O" name="5"/>
  <lib desc="#TTL" name="6"/>
  <main name="main"/>
  <options>
    <a name="gateUndefined" val="ignore"/>
    <a name="simlimit" val="1000"/>
    <a name="simrand" val="0"/>
    <a name="tickmain" val="half_period"/>
  </options>
  <mappings>
    <tool lib="9" map="Button2" name="Menu Tool"/>
    <tool lib="9" map="Button3" name="Menu Tool"/>
    <tool lib="9" map="Ctrl Button1" name="Menu Tool"/>
  </mappings>
  <toolbar>
    <tool lib="9" name="Poke Tool"/>
    <tool lib="9" name="Edit Tool"/>
    <tool lib="9" name="Text Tool">
      <a name="text" val=""/>
      <a name="font" val="SansSerif plain 12"/>
      <a name="halign" val="center"/>
      <a name="valign" val="base"/>
    </tool>
    <sep/>
    <tool lib="0" name="Pin"/>
    <tool lib="0" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
    </tool>
    <tool lib="1" name="NOT Gate"/>
    <tool lib="1" name="AND Gate"/>
    <tool lib="1" name="OR Gate"/>
  </toolbar>
  <circuit name="main">
    <a name="circuit" val="main"/>
    <a name="clabel" val=""/>
    <a name="clabelup" val="east"/>
    <a name="clabelfont" val="SansSerif bold 16"/>
    <a name="circuitnamedbox" val="true"/>
    <a name="circuitnamedboxfixedsize" val="true"/>
    <a name="circuitvhdlpath" val=""/>
    <wire from="(130,130)" to="(130,200)"/>
    <wire from="(90,90)" to="(150,90)"/>
    <wire from="(130,130)" to="(190,130)"/>
    <wire from="(290,160)" to="(340,160)"/>
    <wire from="(260,100)" to="(310,100)"/>
    <wire from="(130,200)" to="(310,200)"/>
    <wire from="(400,150)" to="(400,170)"/>
    <wire from="(290,80)" to="(290,160)"/>
    <wire from="(190,110)" to="(190,130)"/>
    <wire from="(90,50)" to="(260,50)"/>
    <wire from="(150,90)" to="(150,180)"/>
    <wire from="(90,130)" to="(130,130)"/>
    <wire from="(260,50)" to="(260,80)"/>
    <wire from="(460,180)" to="(500,180)"/>
    <wire from="(190,110)" to="(220,110)"/>
    <wire from="(260,80)" to="(290,80)"/>
    <wire from="(290,80)" to="(380,80)"/>
    <wire from="(150,180)" to="(310,180)"/>
    <wire from="(340,190)" to="(430,190)"/>
    <wire from="(400,170)" to="(430,170)"/>
    <wire from="(310,140)" to="(340,140)"/>
    <wire from="(370,150)" to="(400,150)"/>
    <wire from="(310,100)" to="(310,140)"/>
    <wire from="(420,90)" to="(500,90)"/>
    <wire from="(150,90)" to="(220,90)"/>
    <wire from="(310,100)" to="(380,100)"/>
    <comp lib="0" loc="(90,130)" name="Pin">
      <a name="label" val="B"/>
    </comp>
    <comp lib="1" loc="(460,180)" name="OR Gate">
      <a name="size" val="30"/>
    </comp>
    <comp lib="1" loc="(370,150)" name="AND Gate">
      <a name="size" val="30"/>
    </comp>
    <comp lib="0" loc="(90,90)" name="Pin">
      <a name="label" val="A"/>
    </comp>
    <comp lib="0" loc="(500,180)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="Co"/>
    </comp>
    <comp lib="1" loc="(260,100)" name="XOR Gate">
      <a name="size" val="30"/>
    </comp>
    <comp lib="0" loc="(500,90)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="Sum"/>
    </comp>
    <comp lib="1" loc="(420,90)" name="XOR Gate">
      <a name="size" val="30"/>
    </comp>
    <comp lib="0" loc="(90,50)" name="Pin">
      <a name="label" val="Ci"/>
    </comp>
    <comp lib="1" loc="(340,190)" name="AND Gate">
      <a name="size" val="30"/>
    </comp>
  </circuit>
</project>
)FILE";

const char *logisim_multi_data = R"FILE(
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<project source="2.14.6" version="1.0">
This file is intended to be loaded by Logisim-evolution (https://github.com/reds-heig/logisim-evolution).
<lib desc="#Wiring" name="0"/>
  <lib desc="#Gates" name="1"/>
  <lib desc="#Plexers" name="2">
    <tool name="Multiplexer">
      <a name="enable" val="false"/>
    </tool>
    <tool name="Demultiplexer">
      <a name="enable" val="false"/>
    </tool>
  </lib>
  <lib desc="#Arithmetic" name="3"/>
  <lib desc="#Memory" name="4">
    <tool name="ROM">
      <a name="contents">addr/data: 8 8
0
</a>
    </tool>
  </lib>
  <lib desc="#I/O" name="5"/>
  <main name="main"/>
  <options>
    <a name="gateUndefined" val="ignore"/>
    <a name="simlimit" val="1000"/>
    <a name="simrand" val="0"/>
    <a name="tickmain" val="half_period"/>
  </options>
  <mappings>
    <tool lib="8" map="Button2" name="Menu Tool"/>
    <tool lib="8" map="Button3" name="Menu Tool"/>
    <tool lib="8" map="Ctrl Button1" name="Menu Tool"/>
  </mappings>
  <toolbar>
    <tool lib="8" name="Poke Tool"/>
    <tool lib="8" name="Edit Tool"/>
    <tool lib="8" name="Text Tool">
      <a name="text" val=""/>
      <a name="font" val="SansSerif plain 12"/>
      <a name="halign" val="center"/>
      <a name="valign" val="base"/>
    </tool>
    <sep/>
    <tool lib="0" name="Pin"/>
    <tool lib="0" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="labelloc" val="east"/>
    </tool>
    <tool lib="1" name="NOT Gate"/>
    <tool lib="1" name="AND Gate"/>
    <tool lib="1" name="OR Gate"/>
  </toolbar>
  <circuit name="main">
    <a name="circuit" val="main"/>
    <a name="clabel" val=""/>
    <a name="clabelup" val="east"/>
    <a name="clabelfont" val="SansSerif bold 16"/>
    <a name="circuitnamedbox" val="true"/>
    <a name="circuitvhdlpath" val=""/>
    <wire from="(140,80)" to="(170,80)"/>
    <wire from="(140,120)" to="(170,120)"/>
    <wire from="(220,100)" to="(280,100)"/>
    <wire from="(140,70)" to="(140,80)"/>
    <wire from="(140,120)" to="(140,130)"/>
    <wire from="(100,70)" to="(140,70)"/>
    <wire from="(100,130)" to="(140,130)"/>
    <wire from="(100,90)" to="(170,90)"/>
    <wire from="(100,110)" to="(170,110)"/>
    <comp lib="0" loc="(100,90)" name="Pin">
      <a name="label" val="I2"/>
    </comp>
    <comp lib="1" loc="(220,100)" name="AND Gate">
      <a name="inputs" val="4"/>
    </comp>
    <comp lib="0" loc="(280,100)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O"/>
      <a name="labelloc" val="east"/>
    </comp>
    <comp lib="0" loc="(100,70)" name="Pin">
      <a name="label" val="I1"/>
    </comp>
    <comp lib="0" loc="(100,110)" name="Pin">
      <a name="label" val="I3"/>
    </comp>
    <comp lib="0" loc="(100,130)" name="Pin">
      <a name="label" val="I4"/>
    </comp>
  </circuit>
</project>
)FILE";


TEST_CASE ("Small Logisim Circuit", "[logisim]") {
    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    REQUIRE(load_logisim(circuit.get(), logisim_test_data, std::strlen(logisim_test_data)));

    auto in = circuit->component_by_name("In");
    REQUIRE(in);

    auto out = circuit->component_by_name("Out");
    REQUIRE(out);

    static_cast<Connector *>(in)->change_data(VALUE_TRUE);
    circuit->simulation_until_pin_change(out->pin(0));
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
}

TEST_CASE ("Logisim 1-bit adder circuit", "[logisim]") {

    // load circuit
    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    REQUIRE(load_logisim(circuit.get(), logisim_adder_data, std::strlen(logisim_adder_data)));

    // input pins
    auto *pin_Ci = static_cast<Connector *> (circuit->component_by_name("Ci"));
    REQUIRE(pin_Ci);

    auto pin_A = static_cast<Connector *> (circuit->component_by_name("A"));
    REQUIRE(pin_A);

    auto pin_B = static_cast<Connector *> (circuit->component_by_name("B"));
    REQUIRE(pin_B);

    // output pins
    auto pin_Co = static_cast<Connector *> (circuit->component_by_name("Co"));
    REQUIRE(pin_Co);

    auto pin_Sum = static_cast<Connector *> (circuit->component_by_name("Sum"));
    REQUIRE(pin_Sum);

    bool truth_table[][5] = {
        // Ci           A            B            Co           Sum
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},

        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE}
    };

    size_t num_tests = sizeof(truth_table) / sizeof(truth_table[0]);

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        pin_Ci->change_data(truth_table[test_idx][0]);
        pin_A->change_data(truth_table[test_idx][1]);
        pin_B->change_data(truth_table[test_idx][2]);
        circuit->simulation_until_stable(5);
        REQUIRE(circuit->read_value(pin_Co->pin(0)) == truth_table[test_idx][3]);
        REQUIRE(circuit->read_value(pin_Sum->pin(0)) == truth_table[test_idx][4]);
    }
}

TEST_CASE ("Logisim multi-input circuit", "[logisim]") {

    // load circuit
    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    REQUIRE(load_logisim(circuit.get(), logisim_multi_data, std::strlen(logisim_multi_data)));

    // input pins
    auto *pin_I1 = static_cast<Connector *> (circuit->component_by_name("I1"));
    REQUIRE(pin_I1);

    auto *pin_I2 = static_cast<Connector *> (circuit->component_by_name("I2"));
    REQUIRE(pin_I2);

    auto *pin_I3 = static_cast<Connector *> (circuit->component_by_name("I3"));
    REQUIRE(pin_I3);

    auto *pin_I4 = static_cast<Connector *> (circuit->component_by_name("I4"));
    REQUIRE(pin_I4);

    // output pins
    auto pin_O = static_cast<Connector *> (circuit->component_by_name("O"));
    REQUIRE(pin_O);

    bool truth_table[][5] = {
        // I1           I2           I3           I4           O
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},

        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},

        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},

        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE}
    };

    size_t num_tests = sizeof(truth_table) / sizeof(truth_table[0]);

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        pin_I1->change_data(truth_table[test_idx][0]);
        pin_I2->change_data(truth_table[test_idx][1]);
        pin_I3->change_data(truth_table[test_idx][2]);
        pin_I4->change_data(truth_table[test_idx][3]);
        circuit->simulation_until_stable(5);
        REQUIRE(circuit->read_value(pin_O->pin(0)) == truth_table[test_idx][4]);
    }
}