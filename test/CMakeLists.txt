cmake_minimum_required(VERSION 3.22)

include(../cmake/flags.cmake)
include(../cmake/gtest.cmake)

function (new_test source name)
  add_executable(${name} ${source})

  target_compile_features(${name} PRIVATE cxx_std_20)

  target_link_libraries(${name} PRIVATE
    ${PROJECT_NAME}
    gtest
    gtest_main
  )

  set_flags(${name})
  setup_linker(${name})

  # Tests should always be build with sanitizers
  set_sanitizers(${name})

  include(GoogleTest)

  gtest_add_tests(TARGET ${name})

endfunction()

new_test(FlatBufferTests.cpp FlatBufferTests)
