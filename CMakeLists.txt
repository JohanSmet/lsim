cmake_minimum_required(VERSION 3.11)

project(lsim)

# force C++14 for all targets
set(CMAKE_CXX_STANDARD 14)

# platform detection
string(TOUPPER ${CMAKE_SYSTEM_NAME} PLATFORM_NAME)
string(CONCAT PLATFORM_DEF "PLATFORM_" ${PLATFORM_NAME})

#
# external dependencies
#

set (BUILD_SHARED_LIBS ON)
add_subdirectory("libs/pugixml")
set (PUGIXML_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/libs/pugixml/src)
list (APPEND EXTRA_LIBS pugixml)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
	

#
# simulator library
#

set (LIB_TARGET lsim)

set (LIB_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/basic.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/circuit.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/circuit_library.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/error.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/extra.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/gate.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/load_logisim.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/property.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/serialize.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/simulator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/visual_component.cpp"
)

set (LIB_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/basic.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/circuit.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/circuit_library.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/error.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/extra.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/gate.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/load_logisim.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/lsim_context.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/property.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/serialize.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/simulator.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/visual_component.h"
)

add_library(${LIB_TARGET} STATIC ${LIB_SOURCES} ${LIB_HEADERS})
target_include_directories(${LIB_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs ${PUGIXML_INCLUDE})
target_compile_definitions(${LIB_TARGET} PRIVATE ${PLATFORM_DEF})

#
# python bindings
#

set (PYTHON_TARGET lsimpy)

set (PYTHON_SOURCES ${LIB_SOURCES})
list (APPEND PYTHON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/python.cpp")

add_subdirectory(libs/pybind11)
pybind11_add_module(${PYTHON_TARGET} ${PYTHON_SOURCES})
target_include_directories(${PYTHON_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs ${PUGIXML_INCLUDE})
target_link_libraries(${PYTHON_TARGET} PRIVATE ${EXTRA_LIBS})

#
# MAIN executable
#

set(APP_TARGET lsim_gui)

set(APP_SOURCES
	"src/gui/gl3w/GL/gl3w.c"
	"src/gui/imgui/imgui_draw.cpp"
	"src/gui/imgui/imgui_impl_sdl_gl3.cpp"
	"src/gui/imgui/imgui.cpp"
	"src/gui/algebra.cpp"
	"src/gui/component_std.cpp"
	"src/gui/component_ui.cpp"
	"src/gui/imgui_ex.cpp"
	"src/gui/main.cpp"
	"src/gui/main_gui.cpp"
)

set(APP_HEADERS
	"src/gui/gl3w/GL/gl3w.h"
	"src/gui/gl3w/GL/glcorearb.h"
	"src/gui/imgui/imgui_impl_sdl_gl3.h"
	"src/gui/imgui/imgui_internal.h"
	"src/gui/imgui/imgui.h"
	"src/gui/imgui/stb_rect_pack.h"
	"src/gui/imgui/stb_textedit.h"
	"src/gui/imgui/stb_truetype.h"
	"src/gui/algebra.h"
	"src/gui/component_std.h"
	"src/gui/component_ui.h"
	"src/gui/imgui_ex.h"
	"src/gui/shapes.h"
)

add_executable(${APP_TARGET} ${APP_SOURCES} ${APP_HEADERS})
target_include_directories(${APP_TARGET} PRIVATE libs src src/gui/gl3w ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
target_compile_definitions(${APP_TARGET} PRIVATE ${PLATFORM_DEF})
target_link_libraries(${APP_TARGET} ${LIB_TARGET} ${EXTRA_LIBS} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})

#
# Unit tests
#

enable_testing()

set (TEST_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/test_gate.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/test_extra.cpp"
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
