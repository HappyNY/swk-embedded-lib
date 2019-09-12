cmake_minimum_required(VERSION 2.8)

project(swk_c_container)

set(SRC_INC_DIR "src/include")
include_directories(${SRC_INC_DIR})

aux_source_directory(src SRC)
aux_source_directory(testcase TESTCASE)

# Core library build.
add_library(swk_c_container STATIC ${SRC})

# Testcase build.
add_executable(testcase ${TESTCASE})
add_dependencies(testcase swk_c_container)
target_link_libraries(testcase 
	PUBLIC swk_c_container
)
target_include_directories(${SRC_INC_DIR})

# Install settings
# @todo