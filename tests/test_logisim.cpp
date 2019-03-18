#include "catch.hpp"
#include "load_logisim.h"
#include "simulator.h"
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

const char *logisim_tristate_data = R"FILE(
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
    <wire from="(130,50)" to="(130,70)"/>
    <wire from="(80,80)" to="(120,80)"/>
    <wire from="(140,80)" to="(180,80)"/>
    <comp lib="1" loc="(140,80)" name="Controlled Buffer">
      <a name="control" val="left"/>
    </comp>
    <comp lib="0" loc="(180,80)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O"/>
      <a name="labelloc" val="east"/>
    </comp>
    <comp lib="0" loc="(80,80)" name="Pin">
      <a name="label" val="A"/>
    </comp>
    <comp lib="0" loc="(130,50)" name="Pin">
      <a name="facing" val="south"/>
      <a name="label" val="en"/>
    </comp>
  </circuit>
</project>
)FILE";

const char *logisim_nested_data = R"FILE(
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
  <main name="adder4bit"/>
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
    </tool>
    <tool lib="1" name="NOT Gate"/>
    <tool lib="1" name="AND Gate"/>
    <tool lib="1" name="OR Gate"/>
  </toolbar>
  <circuit name="adder1bit">
    <a name="circuit" val="adder1bit"/>
    <a name="clabel" val=""/>
    <a name="clabelup" val="east"/>
    <a name="clabelfont" val="SansSerif bold 16"/>
    <a name="circuitnamedbox" val="true"/>
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
  <circuit name="adder4bit">
    <a name="circuit" val="adder4bit"/>
    <a name="clabel" val=""/>
    <a name="clabelup" val="east"/>
    <a name="clabelfont" val="SansSerif bold 16"/>
    <a name="circuitnamedbox" val="true"/>
    <a name="circuitvhdlpath" val=""/>
    <wire from="(350,80)" to="(350,150)"/>
    <wire from="(350,200)" to="(350,270)"/>
    <wire from="(350,320)" to="(350,390)"/>
    <wire from="(330,420)" to="(380,420)"/>
    <wire from="(330,440)" to="(380,440)"/>
    <wire from="(330,300)" to="(380,300)"/>
    <wire from="(330,60)" to="(380,60)"/>
    <wire from="(330,180)" to="(380,180)"/>
    <wire from="(180,60)" to="(230,60)"/>
    <wire from="(180,80)" to="(230,80)"/>
    <wire from="(180,100)" to="(230,100)"/>
    <wire from="(180,200)" to="(230,200)"/>
    <wire from="(180,220)" to="(230,220)"/>
    <wire from="(180,320)" to="(230,320)"/>
    <wire from="(180,340)" to="(230,340)"/>
    <wire from="(180,440)" to="(230,440)"/>
    <wire from="(180,460)" to="(230,460)"/>
    <wire from="(210,150)" to="(210,180)"/>
    <wire from="(210,270)" to="(210,300)"/>
    <wire from="(210,390)" to="(210,420)"/>
    <wire from="(330,80)" to="(350,80)"/>
    <wire from="(330,200)" to="(350,200)"/>
    <wire from="(330,320)" to="(350,320)"/>
    <wire from="(210,180)" to="(230,180)"/>
    <wire from="(210,300)" to="(230,300)"/>
    <wire from="(210,420)" to="(230,420)"/>
    <wire from="(210,150)" to="(350,150)"/>
    <wire from="(210,270)" to="(350,270)"/>
    <wire from="(210,390)" to="(350,390)"/>
    <comp loc="(330,60)" name="adder1bit"/>
    <comp loc="(330,180)" name="adder1bit"/>
    <comp loc="(330,300)" name="adder1bit"/>
    <comp loc="(330,420)" name="adder1bit"/>
    <comp lib="0" loc="(180,60)" name="Pin">
      <a name="label" val="Ci"/>
    </comp>
    <comp lib="0" loc="(180,80)" name="Pin">
      <a name="label" val="A1"/>
    </comp>
    <comp lib="0" loc="(180,100)" name="Pin">
      <a name="label" val="B1"/>
    </comp>
    <comp lib="0" loc="(180,200)" name="Pin">
      <a name="label" val="A2"/>
    </comp>
    <comp lib="0" loc="(180,220)" name="Pin">
      <a name="label" val="B2"/>
    </comp>
    <comp lib="0" loc="(180,320)" name="Pin">
      <a name="label" val="A3"/>
    </comp>
    <comp lib="0" loc="(180,340)" name="Pin">
      <a name="label" val="B3"/>
    </comp>
    <comp lib="0" loc="(180,460)" name="Pin">
      <a name="label" val="B4"/>
    </comp>
    <comp lib="0" loc="(180,440)" name="Pin">
      <a name="label" val="A4"/>
    </comp>
    <comp lib="0" loc="(380,60)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="S1"/>
      <a name="labelloc" val="east"/>
    </comp>
    <comp lib="0" loc="(380,300)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="S3"/>
      <a name="labelloc" val="east"/>
    </comp>
    <comp lib="0" loc="(380,420)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="S4"/>
      <a name="labelloc" val="east"/>
    </comp>
    <comp lib="0" loc="(380,440)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="Co"/>
      <a name="labelloc" val="east"/>
    </comp>
    <comp lib="0" loc="(380,180)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="S2"/>
      <a name="labelloc" val="east"/>
    </comp>
    <comp lib="8" loc="(193,51)" name="Text">
      <a name="text" val="8"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(352,55)" name="Text">
      <a name="text" val="11"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(200,74)" name="Text">
      <a name="text" val="9"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(203,95)" name="Text">
      <a name="text" val="10"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(401,119)" name="Text">
      <a name="text" val="12 (merge 16)"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(194,196)" name="Text">
      <a name="text" val="17"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(207,219)" name="Text">
      <a name="text" val="18"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(349,174)" name="Text">
      <a name="text" val="19"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(411,237)" name="Text">
      <a name="text" val="20 (merge 24)"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(199,310)" name="Text">
      <a name="text" val="25"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(194,334)" name="Text">
      <a name="text" val="26"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(351,297)" name="Text">
      <a name="text" val="27"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(409,357)" name="Text">
      <a name="text" val="28 (merge 32)"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(200,433)" name="Text">
      <a name="text" val="33"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(190,453)" name="Text">
      <a name="text" val="34"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(344,415)" name="Text">
      <a name="text" val="35"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
    <comp lib="8" loc="(343,450)" name="Text">
      <a name="text" val="36"/>
      <a name="font" val="SansSerif plain 12"/>
    </comp>
  </circuit>
</project>
)FILE";

const char *logisim_bus_data = R"FILE(
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
    <wire from="(170,90)" to="(200,90)"/>
    <wire from="(220,90)" to="(250,90)"/>
    <wire from="(270,60)" to="(300,60)"/>
    <wire from="(280,110)" to="(310,110)"/>
    <wire from="(270,70)" to="(290,70)"/>
    <wire from="(290,90)" to="(310,90)"/>
    <wire from="(300,60)" to="(300,70)"/>
    <wire from="(290,70)" to="(290,90)"/>
    <wire from="(300,70)" to="(310,70)"/>
    <wire from="(270,80)" to="(280,80)"/>
    <wire from="(280,80)" to="(280,110)"/>
    <wire from="(270,50)" to="(310,50)"/>
    <comp lib="0" loc="(170,90)" name="Pin">
      <a name="width" val="4"/>
      <a name="label" val="I"/>
    </comp>
    <comp lib="1" loc="(220,90)" name="Buffer">
      <a name="width" val="4"/>
    </comp>
    <comp lib="0" loc="(250,90)" name="Splitter">
      <a name="fanout" val="4"/>
      <a name="incoming" val="4"/>
    </comp>
    <comp lib="0" loc="(310,50)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O0"/>
    </comp>
    <comp lib="0" loc="(310,70)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O1"/>
    </comp>
    <comp lib="0" loc="(310,110)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O3"/>
    </comp>
    <comp lib="0" loc="(310,90)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O2"/>
    </comp>
  </circuit>
</project>
)FILE";

const char *logisim_tunnel_data = R"FILE(
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
  <main name="main"/>
  <options>
    <a name="gateUndefined" val="ignore"/>
    <a name="simlimit" val="1000"/>
    <a name="simrand" val="0"/>
    <a name="tickmain" val="half_period"/>
  </options>
  <circuit name="main">
    <a name="circuit" val="main"/>
    <a name="clabel" val=""/>
    <a name="clabelup" val="east"/>
    <a name="clabelfont" val="SansSerif bold 16"/>
    <a name="circuitnamedbox" val="true"/>
    <a name="circuitnamedboxfixedsize" val="true"/>
    <a name="circuitvhdlpath" val=""/>
    <wire from="(110,90)" to="(150,90)"/>
    <wire from="(110,130)" to="(130,130)"/>
    <wire from="(130,50)" to="(150,50)"/>
    <wire from="(160,130)" to="(180,130)"/>
    <comp lib="0" loc="(130,50)" name="Pin">
      <a name="label" val="I"/>
    </comp>
    <comp lib="0" loc="(150,50)" name="Tunnel">
      <a name="label" val="T"/>
    </comp>
    <comp lib="0" loc="(110,90)" name="Tunnel">
      <a name="facing" val="east"/>
      <a name="label" val="T"/>
    </comp>
    <comp lib="0" loc="(150,90)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O0"/>
    </comp>
    <comp lib="0" loc="(110,130)" name="Tunnel">
      <a name="facing" val="east"/>
      <a name="label" val="T"/>
    </comp>
    <comp lib="1" loc="(160,130)" name="NOT Gate"/>
    <comp lib="0" loc="(180,130)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O1"/>
    </comp>
  </circuit>
</project>
)FILE";

const char *logisim_multi_tunnel_data = R"FILE(
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
  <main name="main"/>
  <options>
    <a name="gateUndefined" val="ignore"/>
    <a name="simlimit" val="1000"/>
    <a name="simrand" val="0"/>
    <a name="tickmain" val="half_period"/>
  </options>
  <circuit name="main">
    <a name="circuit" val="main"/>
    <a name="clabel" val=""/>
    <a name="clabelup" val="east"/>
    <a name="clabelfont" val="SansSerif bold 16"/>
    <a name="circuitnamedbox" val="true"/>
    <a name="circuitnamedboxfixedsize" val="true"/>
    <a name="circuitvhdlpath" val=""/>
    <wire from="(130,50)" to="(160,50)"/>
    <wire from="(160,100)" to="(190,100)"/>
    <wire from="(110,100)" to="(140,100)"/>
    <wire from="(190,120)" to="(200,120)"/>
    <wire from="(190,100)" to="(190,120)"/>
    <wire from="(160,90)" to="(200,90)"/>
    <comp lib="0" loc="(130,50)" name="Pin">
      <a name="width" val="2"/>
      <a name="label" val="I"/>
    </comp>
    <comp lib="0" loc="(160,50)" name="Tunnel">
      <a name="width" val="2"/>
      <a name="label" val="T"/>
    </comp>
    <comp lib="0" loc="(110,100)" name="Tunnel">
      <a name="facing" val="east"/>
      <a name="width" val="2"/>
      <a name="label" val="T"/>
    </comp>
    <comp lib="0" loc="(140,100)" name="Splitter">
      <a name="appear" val="center"/>
    </comp>
    <comp lib="0" loc="(200,90)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O0"/>
    </comp>
    <comp lib="0" loc="(200,120)" name="Pin">
      <a name="facing" val="west"/>
      <a name="output" val="true"/>
      <a name="label" val="O1"/>
    </comp>
  </circuit>
</project>
)FILE";

TEST_CASE ("Small Logisim Circuit", "[logisim]") {
    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    REQUIRE(load_logisim(sim.get(), logisim_test_data, std::strlen(logisim_test_data)));
    auto circuit = sim->get_main_circuit();
    REQUIRE(circuit);

    auto in = circuit->component_by_name("In");
    REQUIRE(in);

    auto out = circuit->component_by_name("Out");
    REQUIRE(out);

    sim->init();
    static_cast<Connector *>(in)->change_data(VALUE_TRUE);
    sim->run_until_stable(5);
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
}

TEST_CASE ("Logisim 1-bit adder circuit", "[logisim]") {

    // load circuit
    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    REQUIRE(load_logisim(sim.get(), logisim_adder_data, std::strlen(logisim_adder_data)));
    auto circuit = sim->get_main_circuit();
    REQUIRE(circuit);

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

    Value truth_table[][5] = {
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

    sim->init();

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        pin_Ci->change_data(truth_table[test_idx][0]);
        pin_A->change_data(truth_table[test_idx][1]);
        pin_B->change_data(truth_table[test_idx][2]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_value(pin_Co->pin(0)) == truth_table[test_idx][3]);
        REQUIRE(circuit->read_value(pin_Sum->pin(0)) == truth_table[test_idx][4]);
    }
}

TEST_CASE ("Logisim multi-input circuit", "[logisim]") {

    // load circuit
    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    REQUIRE(load_logisim(sim.get(), logisim_multi_data, std::strlen(logisim_multi_data)));
    auto circuit = sim->get_main_circuit();
    REQUIRE(circuit);

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

    Value truth_table[][5] = {
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

    sim->init();

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        pin_I1->change_data(truth_table[test_idx][0]);
        pin_I2->change_data(truth_table[test_idx][1]);
        pin_I3->change_data(truth_table[test_idx][2]);
        pin_I4->change_data(truth_table[test_idx][3]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_value(pin_O->pin(0)) == truth_table[test_idx][4]);
    }
}

TEST_CASE ("Logisim tri-state circuit", "[logisim]") {

    // load circuit
    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    REQUIRE(load_logisim(sim.get(), logisim_tristate_data, std::strlen(logisim_tristate_data)));
    auto circuit = sim->get_main_circuit();
    REQUIRE(circuit);

    // input pins
    auto *pin_in = static_cast<Connector *> (circuit->component_by_name("A"));
    REQUIRE(pin_in);

    auto *pin_en = static_cast<Connector *> (circuit->component_by_name("en"));
    REQUIRE(pin_en);

    // output pins
    auto pin_O = static_cast<Connector *> (circuit->component_by_name("O"));
    REQUIRE(pin_O);

    Value truth_table[][3] = {
        // in           en           out 
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_UNDEFINED},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_UNDEFINED}
    };

    size_t num_tests = sizeof(truth_table) / sizeof(truth_table[0]);

    sim->init();

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        pin_in->change_data(truth_table[test_idx][0]);
        pin_en->change_data(truth_table[test_idx][1]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_value(pin_O->pin(0)) == truth_table[test_idx][2]);
    }
}

TEST_CASE ("Logisim nested circuit", "[logisim]") {

    // load circuit
    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    REQUIRE(load_logisim(sim.get(), logisim_nested_data, std::strlen(logisim_nested_data)));
    auto circuit = sim->get_main_circuit();
    REQUIRE(circuit);

    // input pins
    Connector *pin_A[] = {
        static_cast<Connector *> (circuit->component_by_name("A1")),
        static_cast<Connector *> (circuit->component_by_name("A2")),
        static_cast<Connector *> (circuit->component_by_name("A3")),
        static_cast<Connector *> (circuit->component_by_name("A4"))
    };
    Connector *pin_B[] = {
        static_cast<Connector *> (circuit->component_by_name("B1")),
        static_cast<Connector *> (circuit->component_by_name("B2")),
        static_cast<Connector *> (circuit->component_by_name("B3")),
        static_cast<Connector *> (circuit->component_by_name("B4"))
    };
    auto pin_Ci = static_cast<Connector *> (circuit->component_by_name("Ci"));

    // output pins
    auto pin_O = {static_cast<Connector *> (circuit->component_by_name("S1"))->pin(0),
                  static_cast<Connector *> (circuit->component_by_name("S2"))->pin(0),
                  static_cast<Connector *> (circuit->component_by_name("S3"))->pin(0),
                  static_cast<Connector *> (circuit->component_by_name("S4"))->pin(0)};
    auto pin_Co = static_cast<Connector *> (circuit->component_by_name("Co"))->pin(0);

    for (int ci = 0; ci < 2; ++ci) {
        pin_Ci->change_data(ci);

        for (int a = 0; a < 16; ++a) {
            for (int b = 0; b < 16; ++b) {
                int req_O = (a + b + ci) & 0xf;
                int req_Co = ((a + b + ci) >> 4) & 0x1;

                for (int i = 0; i < 4; ++i) {
                    pin_A[i]->change_data((a >> i) & 1);
                    pin_B[i]->change_data((b >> i) & 1);
                }

                sim->run_until_stable(5);

                REQUIRE(sim->read_nibble(pin_O) == req_O);
                REQUIRE(sim->read_pin(pin_Co) == req_Co);
            }
        }
    }
}

TEST_CASE ("Logisim +1 databits", "[logisim]") {

    // load circuit
    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    REQUIRE(load_logisim(sim.get(), logisim_bus_data, std::strlen(logisim_bus_data)));
    auto circuit = sim->get_main_circuit();
    REQUIRE(circuit);

    auto *pin_I = static_cast<Connector *> (circuit->component_by_name("I"));
    REQUIRE(pin_I);

    auto pin_O = {static_cast<Connector *> (circuit->component_by_name("O0"))->pin(0),
                  static_cast<Connector *> (circuit->component_by_name("O1"))->pin(0),
                  static_cast<Connector *> (circuit->component_by_name("O2"))->pin(0),
                  static_cast<Connector *> (circuit->component_by_name("O3"))->pin(0)};

    sim->init();

    for (int i = 0; i < 16; ++i) {
        pin_I->change_data(i);
        sim->run_until_stable(5);
        REQUIRE(sim->read_nibble(pin_O) == i);
    }
}

TEST_CASE ("Logisim Tunnel", "[logisim]") {

    // load circuit
    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    REQUIRE(load_logisim(sim.get(), logisim_tunnel_data, std::strlen(logisim_tunnel_data)));
    auto circuit = sim->get_main_circuit();
    REQUIRE(circuit);

    auto *pin_I = static_cast<Connector *> (circuit->component_by_name("I"));
    REQUIRE(pin_I);

    auto pin_O0 = static_cast<Connector *> (circuit->component_by_name("O0"));
    REQUIRE(pin_O0);
    auto pin_O1 = static_cast<Connector *> (circuit->component_by_name("O1"));
    REQUIRE(pin_O1);

    sim->init();

    Value truth_table[][3] = {
        // I                O0                O1
        {VALUE_FALSE,     VALUE_FALSE,      VALUE_TRUE},
        {VALUE_TRUE,      VALUE_TRUE,       VALUE_FALSE},
        {VALUE_UNDEFINED, VALUE_UNDEFINED,  VALUE_ERROR}
    };

    for (const auto &test: truth_table) {
        pin_I->change_data(test[0]);
        sim->run_until_stable(5);
        REQUIRE(pin_O0->read_pin(0) == test[1]);
        REQUIRE(pin_O1->read_pin(0) == test[2]);
    }
}

TEST_CASE ("Logisim Multi-bit tunnel", "[logisim]") {

    // load circuit
    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    REQUIRE(load_logisim(sim.get(), logisim_multi_tunnel_data, std::strlen(logisim_multi_tunnel_data)));
    auto circuit = sim->get_main_circuit();
    REQUIRE(circuit);

    auto *pin_I = static_cast<Connector *> (circuit->component_by_name("I"));
    REQUIRE(pin_I);

    auto pin_O0 = static_cast<Connector *> (circuit->component_by_name("O0"));
    REQUIRE(pin_O0);
    auto pin_O1 = static_cast<Connector *> (circuit->component_by_name("O1"));
    REQUIRE(pin_O1);

    sim->init();

    Value truth_table[][4] = {
        // I[0]         I[1]         O0           O1
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE}
    };

    for (const auto &test: truth_table) {
        pin_I->change_data({test[0], test[1]});
        sim->run_until_stable(5);
        REQUIRE(pin_O0->read_pin(0) == test[2]);
        REQUIRE(pin_O1->read_pin(0) == test[3]);
    }
}