cmake_minimum_required (VERSION 3.16)
project(typecheck LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Loads all of the options
include(cmake/options.cmake)
if (TYPECHECK_ENABLE_COVERAGE)
	# Only require lcov if code coverage enabled
	include(cmake/CodeCoverage.cmake)
	set(TYPECHECK_LCOV_ARGS -q --rc lcov_branch_coverage=1 --no-external)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)

	append_coverage_compiler_flags()
endif()

if (TYPECHECK_ENABLE_ASAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if (TYPECHECK_ENABLE_UBSAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)
endif()

set(TYPECHECK_EXTRA_FLAGS "")
if (default_if_in_dir)
	# Don't add the extra flags if not building this project directly.
	include(cmake/utils.cmake) # Defines: add_supported_cxx_flags
	if (MSVC)
		add_supported_cxx_flags("/Wall")
		add_supported_cxx_flags("/W3")
	else()
		if (CMAKE_BUILD_TYPE STREQUAL DEBUG)
			add_supported_cxx_flags("-fPIC")
		endif()

		add_supported_cxx_flags("-Wall")
		add_supported_cxx_flags("-Wextra")
		add_supported_cxx_flags("-Wconversion")
		add_supported_cxx_flags("-Wunreachable-code")
		add_supported_cxx_flags("-Wuninitialized")
		add_supported_cxx_flags("-pedantic")
		add_supported_cxx_flags("-pedantic-errors")
		add_supported_cxx_flags("-Wno-error=unused-variable")
		add_supported_cxx_flags("-Wshadow")
		add_supported_cxx_flags("-Wfloat-equal")
		add_supported_cxx_flags("-Wduplicated-cond")
		add_supported_cxx_flags("-Wno-error=duplicated-branches")
		add_supported_cxx_flags("-Wlogical-op")
		add_supported_cxx_flags("-Wrestrict")
		add_supported_cxx_flags("-Wnull-dereference")
		add_supported_cxx_flags("-Wuseless-cast")
		add_supported_cxx_flags("-Wno-error=double-promotion")
		add_supported_cxx_flags("-Wformat=2")
		add_supported_cxx_flags("-Wformat-truncation")
		add_supported_cxx_flags("-Wformat-overflow")
		add_supported_cxx_flags("-Wshift-overflow")
		add_supported_cxx_flags("-Wundef")
		add_supported_cxx_flags("-fno-common")
		add_supported_cxx_flags("-Wswitch-enum")
		add_supported_cxx_flags("-Wno-error=effc++")
		add_supported_cxx_flags("-fanalyzer")
	endif()

	if (TYPECHECK_WERROR)
		message("Enabling WError")
		if (MSVC)
			add_supported_cxx_flags("/WX")
		else()
			set(TYPECHECK_EXTRA_FLAGS ${TYPECHECK_EXTRA_FLAGS} -Werror)
		endif()
	endif()
endif()

if (NOT TARGET constraint)
	message(STATUS "Adding constraint from typecheck")
	add_subdirectory(third_party/constraint EXCLUDE_FROM_ALL)
endif()

if (NOT TARGET cppnotstdlib)
	message(STATUS "Adding cppnotstdlib from typecheck")
	add_subdirectory(third_party/cppnotstdlib EXCLUDE_FROM_ALL)
endif()

if (NOT TARGET magic_enum)
	message(STATUS "Adding magic_enum from typecheck")
	add_subdirectory(third_party/magic_enum EXCLUDE_FROM_ALL)
endif()

if (NOT TARGET fmt)
	message(STATUS "Adding fmt from typecheck")
	add_subdirectory(third_party/fmt EXCLUDE_FROM_ALL)
endif()

if (TYPECHECK_ENABLE_IWYU)
	find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
	message("Include-What-You-Use Enabled: ${iwyu_path}")
	set(iwhy_path_and_options "${iwyu_path} --mapping_file=${CMAKE_CURRENT_BINARY_DIR}/typecheck.imp")
endif()

if (TYPECHECK_ENABLE_LWYU)
	message("Link-What-You-Use Enabled")
	set(CMAKE_LINK_WHAT_YOU_USE CACHE BOOL ON)
endif()

if (TYPECHECK_ENABLE_CPP_CHECK)
    set(CMAKE_CXX_CLANG_TIDY "")

	find_program(CMAKE_CXX_CPPCHECK cppcheck REQUIRED)
	if (${CMAKE_CXX_CPPCHECK} STREQUAL "cppcheck_path-NOTFOUND")
		message(FATAL_ERROR "cppcheck not found")
	endif()
	list(APPEND CMAKE_CXX_CPPCHECK
		"--enable=warning"
		"--force"
        "--suppress=*:${CMAKE_BINARY_DIR}/_deps/*") # Exclude all external dependencies
	message("CPPCheck Enabled: ${CMAKE_CXX_CPPCHECK}")
endif()

if (TYPECHECK_ENABLE_CLANG_TIDY)
    find_program(clang_tidy_path clang-tidy REQUIRED)
    if (${clang_tidy_path} STREQUAL "clang_tidy_path-NOTFOUND")
        message(FATAL_ERROR "clang-tidy not found")
    endif()
    set(CMAKE_CXX_CLANG_TIDY "${clang_tidy_path}")
    list(APPEND CMAKE_CXX_CLANG_TIDY
        "-header-filter=\"${CMAKE_SOURCE_DIRECTORY}\""
        "-checks=\"-*,clang-analyzer-*,modernize-*,clang-analyzer-*,cppcoreguidelines-*,cert-*,performance-*,portability-*,readability-*\""
	"-extra-arg=-Wno-unknown-warning-option")

    if (TYPECHECK_FIX_ERRORS)
        list(APPEND CMAKE_CXX_CLANG_TIDY "--fix")
    endif()
	message("Clang-Tidy Enabled: ${CMAKE_CXX_CLANG_TIDY}")
endif()

file(GLOB_RECURSE TXT_FILES include/*.hpp src/*.hpp)
file(GLOB_RECURSE INC_FILES include/*.hpp src/*.hpp)
add_library(typecheck STATIC ${INC_FILES})
add_subdirectory(src)
target_include_directories(typecheck PUBLIC include)
target_link_libraries(typecheck PRIVATE constraint cppnotstdlib fmt magic_enum::magic_enum)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${INC_FILES} ${SRC_FILES} ${TXT_FILES})

if (TYPECHECK_ENABLE_CPP_CHECK)
	set_property(TARGET typecheck PROPERTY CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK})
endif()

if (ENABLE_IWYU)
	set_property(TARGET typecheck PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options})
endif()

if (TYPECHECK_ENABLE_CLANG_TIDY)
	set_property(TARGET typecheck PROPERTY CXX_CLANG_TIDY ${CMAKE_CXX_CLANG_TIDY})
endif()

target_compile_options(typecheck PRIVATE "${TYPECHECK_EXTRA_FLAGS}")
target_compile_options(typecheck PRIVATE -Werror -Wextra -Wall -pedantic)

if (TYPECHECK_PRINT_DEBUG_CONSTRAINTS)
	message(STATUS "Typecheck: Debug Constraints Enabled")
    target_compile_definitions(typecheck PUBLIC "-DTYPECHECK_PRINT_DEBUG_CONSTRAINTS=0")
	if (TYPECHECK_PRINT_SHORT_DEBUG)
		message(STATUS "Typecheck: Short Debug Enabled")
    	target_compile_definitions(typecheck PUBLIC "-DTYPECHECK_PRINT_SHORT_DEBUG=0")
	endif()
endif()

target_compile_definitions(typecheck PUBLIC "$<$<CONFIG:Debug>:DEBUG>")
target_compile_definitions(typecheck PUBLIC "$<$<CONFIG:Release>:RELEASE>")
target_compile_definitions(typecheck PUBLIC "$<$<CONFIG:RelWithDebInfo>:DEBUG>")
target_compile_definitions(typecheck PUBLIC "$<$<CONFIG:MinSizeRel>:RELEASE>")

if (TYPECHECK_BUILD_TESTS)
	file(GLOB_RECURSE TEST_SRC_FILES src/*.test.cpp)
	if (RUNTIME_BUILD_TESTS)
		cpptest_configure_target(
			TARGET typecheck
			TEST_SRC_FILES ${TEST_SRC_FILES}
		)
	endif()
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TEST_SRC_FILES})
endif()
