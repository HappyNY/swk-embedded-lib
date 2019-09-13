cmake_minimum_required(VERSION 2.8)

project(swk_c_container)

set(SRC_C_INC_DIR "src-embedded-clib/include/")
set(SRC_CPP_INC_DIR "src-embedded-cpplib/include/")
set(DEST_DIR_NAME "swk-embedded-lib")

include_directories(${SRC_C_INC_DIR})
include_directories(${SRC_CPP_INC_DIR})

aux_source_directory("src-embedded-clib" SRC_C)
aux_source_directory("src-embedded-cpplib" SRC_CPP)
aux_source_directory(testcase TESTCASE)

# Core library build.
add_library(swk_embedded_c STATIC ${SRC_C})
add_library(swk_embedded_cpp STATIC ${SRC_CPP})

# Testcase build.
include_directories("./third/Catch2/single_include/")
add_executable(testcase ${TESTCASE})
add_dependencies(testcase swk_embedded_c swk_embedded_cpp)
target_link_libraries(testcase PUBLIC swk_embedded_c swk_embedded_cpp)

# Install settings
# Install libraries
install( 
	TARGETS swk_embedded_c swk_embedded_cpp
	RUNTIME DESTINATION ${DEST_DIR_NAME}/bin
	LIBRARY DESTINATION ${DEST_DIR_NAME}/lib
	ARCHIVE DESTINATION ${DEST_DIR_NAME}/lib
)

# Install includes 
install( DIRECTORY ${SRC_C_INC_DIR} DESTINATION ${DEST_DIR_NAME}/include )
install( DIRECTORY ${SRC_CPP_INC_DIR} DESTINATION ${DEST_DIR_NAME}/include )

# Install tests
install(
	TARGETS testcase
	RUNTIME DESTINATION ${DEST_DIR_NAME}/test
)

# testapp
aux_source_directory(testapp SRC_TESTAPP)
add_executable(testapp ${SRC_TESTAPP})
add_dependencies(testapp swk_embedded_c swk_embedded_cpp)
target_link_libraries(testapp PUBLIC swk_embedded_c swk_embedded_cpp)
