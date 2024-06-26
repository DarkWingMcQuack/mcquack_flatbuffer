function(setup_linker target)
  if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    find_program(LLD lld)
    if(LLD)
      message(STATUS "found lld, using it as linker")
      target_link_options(${target} PUBLIC -fuse-ld=lld)
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
      message(FATAL_ERROR "Gold linker must be installed for building in Release mode.")
    else()
      message(STATUS "LLD not available, using the system linker for target ${target}")
    endif()
  endif()

  if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    find_program(GNU_GOLD NAMES gold ld.gold DOC "path to gold")
    if(GNU_GOLD)
      message(STATUS "found GNU gold linker, using it as linker for target ${target}")
      target_link_options(${target} PUBLIC -fuse-ld=gold;LINKER:--threads,--thread-count=${HOST_PROC_COUNT})
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
      message(FATAL_ERROR "Gold linker must be installed for building in Release mode.")
    else()
      message(STATUS "Gold not available, using the system linker for target ${target}")
    endif()
  endif()
endfunction()

function(set_flags target)
  target_compile_options(${target} PUBLIC
    -Wall -Werror=format-security -pipe -march=native -g
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:DEBUG>:-g3>
    $<$<CXX_COMPILER_ID:Clang>:-ferror-limit=2>
    # $<$<CXX_COMPILER_ID:GNU>:-fanalyzer>
    $<$<CONFIG:DEBUG>:-fno-optimize-sibling-calls>
    $<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>
    $<$<CONFIG:DEBUG>:-Wfloat-equal>
    $<$<CONFIG:DEBUG>:-Wpointer-arith>
    $<$<CONFIG:DEBUG>:-Wformat-nonliteral>
    $<$<CONFIG:DEBUG>:-Winit-self>
    $<$<CONFIG:DEBUG>:-ggdb>
    $<$<CONFIG:RELEASE>:-O3>
    $<$<CONFIG:RELEASE>:-march=native>
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-fipa-pta>
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-flto=auto>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:GNU>>,$<CONFIG:RELEASE>>:-flto>
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-fdevirtualize-at-ltrans>
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-fdevirtualize-speculatively>
  )

endfunction()


function(set_sanitizers target)
  target_compile_options(${target} PRIVATE
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined
    -fsanitize=pointer-subtract
    -fsanitize=pointer-compare
    )

  target_link_options(${target} PRIVATE
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined
    -fsanitize=pointer-subtract
    -fsanitize=pointer-compare
    )
endfunction()
