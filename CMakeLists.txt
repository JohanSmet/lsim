cmake_minimum_required(VERSION 3.1)
if (POLICY CMP0072)
	# prefer GLVND (note: this is the default for cmake >= 3.11)
	cmake_policy (SET CMP0072 NEW)
endif()

project(lsim)

# options
option(PYTHON_BINDINGS "Enable Python bindings (pybind11)" OFF)

# force C++14 for all targets
set(CMAKE_CXX_STANDARD 14)

# platform detection
string(TOUPPER ${CMAKE_SYSTEM_NAME} PLATFORM_NAME)
string(CONCAT PLATFORM_DEF "PLATFORM_" ${PLATFORM_NAME})

#
# external dependencies
#

add_subdirectory("libs/pugixml")
set_property(TARGET pugixml PROPERTY POSITION_INDEPENDENT_CODE ON)
set (PUGIXML_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/libs/pugixml/src)
list (APPEND EXTRA_LIBS pugixml)

set (CUTE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/libs/cute)

if (NOT EMSCRIPTEN)
	find_package(SDL2 REQUIRED)
	find_package(OpenGL REQUIRED)
endif()
	
#
# simulator library
#

set (LIB_TARGET lsim)

set (LIB_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/algebra.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/error.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/load_logisim.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/lsim_context.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_circuit.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_circuit_library.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_component.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_wire.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_property.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/serialize.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/simulator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_component.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_circuit.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_functions.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_gates.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_various.cpp"
)

set (LIB_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/algebra.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/error.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/load_logisim.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/lsim_context.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_circuit.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_circuit_library.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_component.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_wire.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/model_property.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/serialize.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/simulator.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_component.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_circuit.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_functions.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sim_types.h"
)

add_library(${LIB_TARGET} STATIC ${LIB_SOURCES} ${LIB_HEADERS})
target_include_directories(${LIB_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs ${PUGIXML_INCLUDE})
target_compile_definitions(${LIB_TARGET} PRIVATE ${PLATFORM_DEF})

#
# python bindings
#
if (PYTHON_BINDINGS) 
	set (PYTHON_TARGET lsimpy)

	set (PYTHON_SOURCES ${LIB_SOURCES})
	list (APPEND PYTHON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/python.cpp")

	add_subdirectory(libs/pybind11)
	pybind11_add_module(${PYTHON_TARGET} ${PYTHON_SOURCES})
	target_include_directories(${PYTHON_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs ${PUGIXML_INCLUDE})
	target_link_libraries(${PYTHON_TARGET} PRIVATE ${EXTRA_LIBS})
endif()

#
# MAIN executable
#

set(APP_TARGET lsim_gui)

set(APP_SOURCES
	"libs/cute/cute_files.cpp"
	"src/gui/imgui/imgui_draw.cpp"
	"src/gui/imgui/imgui.cpp"
	"src/gui/imgui/imgui_widgets.cpp"
	"src/gui/imgui/imgui_impl_sdl.cpp"
	"src/gui/imgui/imgui_impl_opengl3.cpp"
	"src/gui/component_std.cpp"
	"src/gui/component_ui.cpp"
	"src/gui/file_selector.cpp"
	"src/gui/imgui_ex.cpp"
	"src/gui/main_gui.cpp"
)

set(APP_HEADERS
	"libs/cute/cute_files.h"
	"src/gui/imgui/imgui_internal.h"
	"src/gui/imgui/imgui.h"
	"src/gui/imgui/imgui_impl_sdl.h"
	"src/gui/imgui/imgui_impl_opengl3.h"
	"src/gui/imgui/stb_rect_pack.h"
	"src/gui/imgui/stb_textedit.h"
	"src/gui/imgui/stb_truetype.h"
	"src/gui/component_std.h"
	"src/gui/component_ui.h"
	"src/gui/file_selector.h"
	"src/gui/imgui_ex.h"
	"src/gui/shapes.h"
)

if (NOT EMSCRIPTEN)
	set(APP_INCLUDES ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${CUTE_INCLUDE})
	set(APP_LIBS ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
	list(APPEND APP_SOURCES 
		"src/gui/gl3w/GL/gl3w.c"
		"src/gui/main.cpp"
	)
	list(APPEND APP_HEADERS
		"src/gui/gl3w/GL/gl3w.h"
		"src/gui/gl3w/GL/glcorearb.h"
	)
else()
	set(APP_INCLUDES ${CUTE_INCLUDE})
	list (APPEND APP_LIBS 
		"-s USE_SDL=2"
		"-s WASM=1" 
		"-s ALLOW_MEMORY_GROWTH=1"
		"-s BINARYEN_TRAP_MODE=clamp"
		"--no-heap-copy"
		"--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/shell_minimal.html"
		"-o lsim.html"
		"--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/examples@/examples --exclude-file *.circ"
	)
	list (APPEND APP_COMPILE_FLAGS -s USE_SDL=2)
	list(APPEND APP_SOURCES 
		"src/gui/main_emscripten.cpp"
	)
endif()

add_executable(${APP_TARGET} WIN32 ${APP_SOURCES} ${APP_HEADERS})
target_include_directories(${APP_TARGET} PRIVATE libs src src/gui/gl3w ${APP_INCLUDES})
target_compile_definitions(${APP_TARGET} PRIVATE ${PLATFORM_DEF})
target_compile_options(${APP_TARGET} PRIVATE ${APP_COMPILE_FLAGS})
target_link_libraries(${APP_TARGET} ${LIB_TARGET} ${EXTRA_LIBS} ${APP_LIBS} ${CMAKE_DL_LIBS})

if (APPLE) 
	target_link_libraries(${APP_TARGET} "-framework CoreFoundation")
endif (APPLE)

#
# speedtest
#

set(SPEED_TARGET speedtest)

set(SPEED_SOURCES
		"src/tools/speedtest/speedtest_main.cpp"
)

add_executable(${SPEED_TARGET} ${SPEED_SOURCES})
target_include_directories(${SPEED_TARGET} PRIVATE libs src) 
target_compile_definitions(${SPEED_TARGET} PRIVATE ${PLATFORM_DEF})
target_link_libraries(${SPEED_TARGET} ${LIB_TARGET} ${EXTRA_LIBS} ${CMAKE_DL_LIBS})

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
