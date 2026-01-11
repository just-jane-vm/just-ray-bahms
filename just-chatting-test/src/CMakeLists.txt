file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS *.h)

add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

# Generate gRPC/protobuf sources from your proto
# Put your proto at: <repo>/proto/myservice.proto (recommended)
generate_grpc_cpp(${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/proto/twapi.proto
)

# Generated headers will be in build/src (this subdir's binary dir)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    raylib
    gRPC::grpc++
    protobuf::libprotobuf
)

# Abseil: some installs need explicit linkage
set(_ABSL_TARGETS
  absl::base
  absl::strings
  absl::synchronization
  absl::time
  absl::log
  absl::log_initialize
  absl::log_globals
  absl::absl_check
  absl::die_if_null
)
foreach(t IN LISTS _ABSL_TARGETS)
  if(TARGET ${t})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${t})
  endif()
endforeach()

# Output dir (same as your template)
set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
  $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Web Configurations (kept from template)
if ("${PLATFORM}" STREQUAL "Web")
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
  target_link_options(${PROJECT_NAME} PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file resources)
endif()

# macOS frameworks (kept from template)
if (APPLE)
  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      "-framework IOKit"
      "-framework Cocoa"
      "-framework OpenGL"
  )
endif()
