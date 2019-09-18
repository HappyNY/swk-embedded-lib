cmake_minimum_required(VERSION 2.8)

project(uembedded_c)

set(SRC_C_INC_DIR "src/include/")
set(DEST_DIR_NAME "uembedded_c")

include_directories(${SRC_C_INC_DIR})
include_directories(${SRC_CPP_INC_DIR})

aux_source_directory("src" SRC_C) 
aux_source_directory(testcase TESTCASE)

# Core library build.
add_library(uembedded_c STATIC ${SRC_C}) 

# Testcase build.
include_directories("./third/Catch2/single_include/")
add_executable(testcase ${TESTCASE})
add_dependencies(testcase uembedded_c)
target_link_libraries(testcase PUBLIC uembedded_c)

# Install settings
# Install libraries
install( 
	TARGETS uembedded_c
	RUNTIME DESTINATION ${DEST_DIR_NAME}/bin
	LIBRARY DESTINATION ${DEST_DIR_NAME}/lib
	ARCHIVE DESTINATION ${DEST_DIR_NAME}/lib
)

# Install includes 
install( DIRECTORY ${SRC_C_INC_DIR} DESTINATION ${DEST_DIR_NAME}/include ) 

# Install tests
install(
	TARGETS testcase
	RUNTIME DESTINATION ${DEST_DIR_NAME}/test
)

# testapp
aux_source_directory(testapp SRC_TESTAPP)
add_executable(testapp ${SRC_TESTAPP})
add_dependencies(testapp uembedded_c)
target_link_libraries(testapp PUBLIC uembedded_c)
