project(llvm2c)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set(FILES core/Func.h core/Func.cpp core/Block.h core/Block.cpp core/Program.h core/Program.cpp type/Type.h type/Type.cpp type/TypeHandler.h type/TypeHandler.cpp expr/Expr.h expr/Expr.cpp expr/BinaryExpr.h expr/BinaryExpr.cpp expr/UnaryExpr.h expr/UnaryExpr.cpp parser/ProgramParser.h parser/cfunc.h parser/passes.h parser/allocas.cpp parser/blocks.cpp parser/declarations.cpp parser/expressions.cpp parser/functionParameters.cpp parser/functions.cpp parser/globalVars.cpp parser/includes.cpp parser/metadataNames.cpp parser/metadataTypes.cpp parser/structs.cpp parser/nameFunctions.cpp parser/breaks.cpp parser/phis.cpp parser/constval.cpp parser/inlinable-blocks.cpp parser/ref-deref.cpp parser/fix-main-parameters.cpp parser/add-sign-casts.cpp parser/ProgramParser.cpp writer/CWriter.cpp writer/Writer.cpp writer/ExprWriter.cpp)
add_executable(llvm2c ${SRC_LIST} ${FILES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -fpermissive")

find_package(LLVM REQUIRED CONFIG)
if (${LLVM_PACKAGE_VERSION} VERSION_GREATER "3.4")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_DIR}")
  include(LLVMConfig)
endif()
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

if (LLVM_SRC_PATH)
  include_directories(${LLVM_SRC_PATH}/include)
  SET(LLVM_INCLUDE_DIRS ${LLVM_SRC_PATH}/include)
  message(STATUS "Got LLVM_SRC_PATH ${LLVM_SRC_PATH}")
endif(LLVM_SRC_PATH)
if (LLVM_BUILD_PATH)
  include_directories(${LLVM_BUILD_PATH}/include)
  link_directories(${LLVM_BUILD_PATH}/lib)
  SET(LLVM_LIBRARY_DIRS ${LLVM_BUILD_PATH}/lib)
  message(STATUS "Got LLVM_BUILD_PATH ${LLVM_BUILD_PATH}")
endif(LLVM_BUILD_PATH)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM include dir: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM libraries dir: ${LLVM_LIBRARY_DIRS}")
message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")

add_definitions(-DHAVE_LLVM)

# Find the libraries that correspond to the LLVM components
# that we wish to use
if (${LLVM_PACKAGE_VERSION} VERSION_GREATER "3.4")
  llvm_map_components_to_libnames(llvm_libs support core irreader bitwriter linker)
else()
  llvm_map_components_to_libraries(llvm_libs support core irreader bitwriter linker)
endif()

target_link_libraries(llvm2c ${llvm_libs})
install(TARGETS llvm2c RUNTIME DESTINATION bin)
