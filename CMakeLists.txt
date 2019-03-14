cmake_minimum_required(VERSION 3.1)

project(lsim)

# force C++14 for all targets
set(CMAKE_CXX_STANDARD 14)

# platform detection
string(TOUPPER ${CMAKE_SYSTEM_NAME} PLATFORM_NAME)
string(CONCAT PLATFORM_DEF "PLATFORM_" ${PLATFORM_NAME})

#
# external dependencies
#

add_subdirectory("libs/pugixml")
set (PUGIXML_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/libs/pugixml/src)
list (APPEND EXTRA_LIBS pugixml)

#
# simulator library
#

set (LIB_TARGET lsim)

set (LIB_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/basic.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/circuit.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/error.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/gate.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/load_logisim.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/simulator.cpp"
)

set (LIB_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/basic.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/circuit.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/error.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/gate.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/load_logisim.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/simulator.h"
)

add_library(${LIB_TARGET} STATIC ${LIB_SOURCES} ${LIB_HEADERS})
target_include_directories(${LIB_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs ${PUGIXML_INCLUDE})
target_compile_definitions(${LIB_TARGET} PRIVATE ${PLATFORM_DEF})

#
# MAIN executable
#

#set(APP_TARGET lsim_cli)
#
#set(APP_SOURCES
#	"${CMAKE_CURRENT_SOURCE_DIR}/src/cli/main.cpp"
#)
#
#set(APP_HEADERS
#	""
#)
#
#add_executable(${APP_TARGET} ${APP_SOURCES} ${APP_HEADERS})
#target_include_directories(${APP_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs ${CMAKE_CURRENT_SOURCE_DIR}/src)
#target_compile_definitions(${APP_TARGET} PRIVATE ${PLATFORM_DEF})
#target_link_libraries(${APP_TARGET} ${LIB_TARGET} ${EXTRA_LIBS})

#
# Unit tests
#

enable_testing()

set (TEST_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/test_gate.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/test_circuit.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/test_logisim.cpp"
)

set (TEST_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/catch.hpp"
)

add_executable(test_runner ${TEST_SOURCES} ${TEST_HEADERS})
target_include_directories(test_runner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(test_runner ${LIB_TARGET} ${EXTRA_LIBS})
add_test(NAME unittests COMMAND test_runner)

#add_custom_command(
     #TARGET test_runner
     #COMMENT "Run tests"
     #POST_BUILD 
     #WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     #COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> --output-on-failures
#)
