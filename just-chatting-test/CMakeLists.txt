cmake_minimum_required(VERSION 3.24...3.30)
project(just-chatting-test LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- gRPC / Protobuf ----
# Configure with:
#   cmake -S . -B build -DCMAKE_PREFIX_PATH=$HOME/grpc-install
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)

# Let build-time tools (protoc/grpc_cpp_plugin) and your binary find libs in ~/grpc-install/lib
set(_GRPC_LIBDIR "$ENV{HOME}/grpc-install/lib")
set(CMAKE_BUILD_RPATH "${_GRPC_LIBDIR}")
set(CMAKE_INSTALL_RPATH "${_GRPC_LIBDIR}")

set(PROTOC_EXECUTABLE $<TARGET_FILE:protobuf::protoc>)
set(GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

function(generate_grpc_cpp TARGET)
  foreach(PROTO_FILE ${ARGN})
    get_filename_component(PROTO_DIR  ${PROTO_FILE} DIRECTORY)
    get_filename_component(PROTO_BASE ${PROTO_FILE} NAME_WE)  # myservice
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME)     # myservice.proto

    set(GEN_SRC
      ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_BASE}.pb.cc
      ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_BASE}.grpc.pb.cc
    )
    set(GEN_HDR
      ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_BASE}.pb.h
      ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_BASE}.grpc.pb.h
    )

    add_custom_command(
      OUTPUT ${GEN_SRC} ${GEN_HDR}
      COMMAND ${PROTOC_EXECUTABLE}
      ARGS
        --proto_path=${PROTO_DIR}
        --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
        --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
        ${PROTO_NAME}
      DEPENDS ${PROTO_FILE}
      COMMENT "Generating gRPC sources from ${PROTO_FILE}"
      VERBATIM
    )

    target_sources(${TARGET} PRIVATE ${GEN_SRC} ${GEN_HDR})
  endforeach()
endfunction()

# ---- raylib ----
set(RAYLIB_VERSION 5.5)
FetchContent_Declare(
  raylib
  DOWNLOAD_EXTRACT_TIMESTAMP OFF
  URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(raylib)

# Our Project
# IMPORTANT: create the executable in src/ (with sources), not here.
add_subdirectory(src)
