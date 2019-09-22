cmake_minimum_required(VERSION 3.13)

project(lsim)

# options
option(PYTHON_BINDINGS "Enable Python bindings (pybind11)" OFF)

# force C++14 for all targets
set(CMAKE_CXX_STANDARD 14)

# platform detection
string(TOUPPER ${CMAKE_SYSTEM_NAME} PLATFORM_NAME)
string(CONCAT PLATFORM_DEF "PLATFORM_" ${PLATFORM_NAME})

#
# helper functions
#

function (lsim_source_group p_target p_dir)
	get_target_property(SOURCES_LIST ${p_target} SOURCES)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/${p_dir} FILES ${SOURCES_LIST})
endfunction()


#
# external dependencies
#

# pugixml
add_subdirectory("libs/pugixml")
set_property(TARGET pugixml PROPERTY POSITION_INDEPENDENT_CODE ON)
set (PUGIXML_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/libs/pugixml/src)

# cute
set(CUTE_TARGET cute)
set (CUTE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/libs/cute)
add_library(${CUTE_TARGET} STATIC)
target_sources(${CUTE_TARGET} PRIVATE 
		libs/cute/cute_files.cpp
		libs/cute/cute_files.h
)

# SDL2 / OpenGL
if (NOT EMSCRIPTEN)
	find_package(SDL2 REQUIRED)
	find_package(OpenGL REQUIRED)
endif()
	
#
# simulator library
#

set (LIB_TARGET lsim)

add_library(${LIB_TARGET} STATIC)
target_sources(${LIB_TARGET} 
	PRIVATE
		src/algebra.cpp
		src/algebra.h
		src/error.cpp
		src/error.h
		src/load_logisim.cpp
		src/load_logisim.h
		src/lsim_context.cpp
		src/lsim_context.h
		src/model_circuit.cpp
		src/model_circuit.h
		src/model_circuit_library.cpp
		src/model_circuit_library.h
		src/model_component.cpp
		src/model_component.h
		src/model_wire.cpp
		src/model_wire.h
		src/model_property.cpp
		src/model_property.h
		src/serialize.cpp
		src/serialize.h
		src/simulator.cpp
		src/simulator.h
		src/sim_component.cpp
		src/sim_component.h
		src/sim_circuit.cpp
		src/sim_circuit.h
		src/sim_functions.cpp
		src/sim_functions.h
		src/sim_gates.cpp
		src/sim_various.cpp
		src/sim_types.h
		src/std_helper.h
)
target_include_directories(${LIB_TARGET} PRIVATE ${PUGIXML_INCLUDE})
target_compile_definitions(${LIB_TARGET} PRIVATE ${PLATFORM_DEF})
target_link_libraries(${LIB_TARGET} PUBLIC pugixml)
set_property(TARGET ${LIB_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

lsim_source_group(${LIB_TARGET} src)

#
# python bindings
#

if (PYTHON_BINDINGS) 
	set (PYTHON_TARGET lsimpy)

	add_subdirectory(libs/pybind11)
	pybind11_add_module(${PYTHON_TARGET} src/python.cpp)
	target_include_directories(${PYTHON_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs ${PUGIXML_INCLUDE})
	target_link_libraries(${PYTHON_TARGET} PRIVATE ${LIB_TARGET})
endif()

#
# MAIN executable
#

set(APP_TARGET lsim_gui)

add_executable(${APP_TARGET} WIN32)
target_sources(${APP_TARGET} 
	PRIVATE
		src/gui/imgui/imgui_draw.cpp
		src/gui/imgui/imgui_internal.h
		src/gui/imgui/imgui.cpp
		src/gui/imgui/imgui.h
		src/gui/imgui/imgui_impl_sdl.cpp
		src/gui/imgui/imgui_impl_sdl.h
		src/gui/imgui/imgui_impl_opengl3.cpp
		src/gui/imgui/imgui_impl_opengl3.h
		src/gui/imgui/imgui_widgets.cpp
		src/gui/imgui/stb_rect_pack.h
		src/gui/imgui/stb_textedit.h
		src/gui/imgui/stb_truetype.h
		src/gui/component_draw.cpp
		src/gui/component_draw.h
		src/gui/component_icon.cpp
		src/gui/component_icon.h
		src/gui/component_ui.cpp
		src/gui/component_ui.h
		src/gui/component_widget.cpp
		src/gui/component_widget.h
		src/gui/configuration.h
		src/gui/file_selector.cpp
		src/gui/file_selector.h
		src/gui/imgui_ex.cpp
		src/gui/imgui_ex.h
		src/gui/main_gui.cpp
		src/gui/shapes.h
)

target_link_libraries(${APP_TARGET} PRIVATE ${LIB_TARGET} ${CUTE_TARGET} ${CMAKE_DL_LIBS})
target_include_directories(${APP_TARGET} PRIVATE src ${CUTE_INCLUDE})
target_compile_definitions(${APP_TARGET} PRIVATE ${PLATFORM_DEF})

if (NOT EMSCRIPTEN)
	target_sources(${APP_TARGET} PRIVATE
		src/gui/gl3w/GL/gl3w.c
		src/gui/gl3w/GL/gl3w.h
		src/gui/gl3w/GL/glcorearb.h
		src/gui/main.cpp
	)
	target_include_directories(${APP_TARGET} PRIVATE ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} src/gui/gl3w)
	target_link_libraries(${APP_TARGET} PRIVATE ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

	if (APPLE) 
		target_link_libraries(${APP_TARGET} "-framework CoreFoundation")
	endif (APPLE)
else()
	target_sources(${APP_TARGET} PRIVATE
		src/gui/main_emscripten.cpp
	)

	target_compile_options(${APP_TARGET} PRIVATE -s USE_SDL=2)
	target_link_libraries(${APP_TARGET} PRIVATE "-s USE_SDL=2")
	target_link_libraries(${APP_TARGET} PRIVATE 
		"-s WASM=1" 
		"-s ALLOW_MEMORY_GROWTH=1" 
		"-s BINARYEN_TRAP_MODE=clamp"
		"--no-heap-copy"
	)
	target_link_libraries(${APP_TARGET} PRIVATE 
		"--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/shell_minimal.html"
		"-o lsim.html"
	)
	target_link_libraries(${APP_TARGET} PRIVATE 
		"--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/examples@/examples --exclude-file *.circ"
	)
endif()

lsim_source_group(${APP_TARGET} src/gui)


#
# speedtest
#

set(SPEED_TARGET speedtest)

add_executable(${SPEED_TARGET})
target_sources(${SPEED_TARGET} PRIVATE src/tools/speedtest/speedtest_main.cpp)

target_include_directories(${SPEED_TARGET} PRIVATE src) 
target_compile_definitions(${SPEED_TARGET} PRIVATE ${PLATFORM_DEF})
target_link_libraries(${SPEED_TARGET} PRIVATE ${LIB_TARGET} ${CMAKE_DL_LIBS})

#
# Unit tests
#

enable_testing()

add_executable(test_runner)
target_sources(test_runner 
	PRIVATE
		tests/catch.hpp
		tests/test_main.cpp
		tests/test_gate.cpp
		tests/test_extra.cpp
		tests/test_circuit.cpp
		tests/test_logisim.cpp
)
target_include_directories(test_runner PRIVATE src)
target_link_libraries(test_runner PRIVATE ${LIB_TARGET}) 
add_test(NAME unittests COMMAND test_runner)

if (NOT EMSCRIPTEN)
	set (TEST_CMD ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> --output-on-failures)
else()
	set (TEST_CMD node test_runner)
endif()

add_custom_command(
     TARGET test_runner
     COMMENT "Run tests"
     POST_BUILD 
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${TEST_CMD}
)
