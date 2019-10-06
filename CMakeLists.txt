cmake_minimum_required(VERSION 2.8)

project(uembedded_c)

set(DEST_DIR_NAME "uembedded_c") 
set(SRC_C_INC_DIR "src")

include_directories(${SRC_C_INC_DIR})
include_directories(${SRC_CPP_INC_DIR})

aux_source_directory("src/uEmbedded" SRC_C) 
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
install( FILES "src/uEmbedded/*.h" DESTINATION ${DEST_DIR_NAME}/include ) 
install( FILES "src/uEmbedded/*.hpp" DESTINATION ${DEST_DIR_NAME}/include ) 

# Install tests
install(
	TARGETS testcase
	RUNTIME DESTINATION ${DEST_DIR_NAME}/test
)
