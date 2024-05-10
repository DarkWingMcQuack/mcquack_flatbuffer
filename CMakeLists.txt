cmake_minimum_required(VERSION 3.16)

# Set your project name here
project(mcquack_flatbuffer VERSION 0.1.0)

# Add an INTERFACE pseudo-library target
add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# Specify include directories
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# 'make install' to the correct locations
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match)
install(FILES cmake/${PROJECT_NAME}Config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# This makes the project importable from the build directory
export(EXPORT ${PROJECT_NAME}Targets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
)

# Include all the .h files in the include directory
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "include/*.hpp")
add_custom_target(${PROJECT_NAME}_IDE SOURCES ${HEADER_LIST})

if (MQ_FLATBUFFER_BUILD_TESTS)
  message(STATUS "Build tests: yes")
  enable_testing()
  add_subdirectory(test)
else()
  message(STATUS "Build tests: no")
endif (MQ_FLATBUFFER_BUILD_TESTS)
