cmake_minimum_required (VERSION 3.10)
project(gbolt VERSION 1.0)

option(GBOLT_BUILD_BIN "Whether to build the test executable or not." ON)
option(GBOLT_PERFORMANCE "Performance Mode" OFF)
option(GBOLT_SERIAL "Serial Mode" OFF)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
			"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
			FORCE)
endif()

include(GNUInstallDirs)

add_library(libgbolt STATIC
		"src/history.cc"
		"src/database.cc"
		"src/output.cc"
		"src/gbolt_count.cc"
		"src/gbolt_execute.cc"
		"src/gbolt_extend.cc"
		"src/gbolt_mine.cc"

		"include/gbolt/common.h"
		"include/gbolt/config.h"
		"include/gbolt/database.h"
		"include/gbolt/gbolt.h"
		"include/gbolt/graph.h"
		"include/gbolt/history.h"
		"include/gbolt/output.h"
		"include/gbolt/path.h")
set_target_properties(libgbolt PROPERTIES
		POSITION_INDEPENDENT_CODE ON
		OUTPUT_NAME "gbolt")

if(GBOLT_PERFORMANCE)
	target_compile_definitions(libgbolt -DGBOLT_PERFORMANCE)
endif()
if(GBOLT_SERIAL)
	target_compile_definitions(libgbolt -DGBOLT_SERIAL)
else()
	find_package(OpenMP)
	if(OPENMP_FOUND)
		target_link_libraries(libgbolt PRIVATE OpenMP::OpenMP_CXX)
	endif()
endif()

target_include_directories(libgbolt
		PUBLIC
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(TARGETS libgbolt
		EXPORT PROJECT_exports
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT gbolt_lib
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT gbolt_lib
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT gbolt_lib
		)

install(DIRECTORY include/gbolt
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		COMPONENT gbolt_dev)


if(GBOLT_BUILD_BIN)
	add_executable(gbolt "src/gbolt.cc" )
	target_include_directories(gbolt
			PRIVATE extern/cxxopts/include)

	target_link_libraries(gbolt libgbolt)

	find_library(JEMALLOC NAMES jemalloc PATHS ${JEMALLOC_DIR} PATH_SUFFIXES lib NO_DEFAULT_PATH)
	if(JEMALLOC)
		target_link_libraries(gbolt ${JEMALLOC})
	endif()

	install(TARGETS gbolt
			DESTINATION ${CMAKE_INSTALL_BINDIR}
			COMPONENT gbolt_run)
endif()