include(CheckIncludeFile)
include(CheckIncludeFileCXX)

function(check_headers group_name prefix)
  # Get the list variable contents by name
  set(headers_list "${${group_name}}")

  foreach(hdr IN LISTS headers_list)
    string(TOUPPER "${hdr}" upper)
    string(REPLACE "." "_" upper ${upper})
    string(REPLACE "/" "_" upper ${upper})
    set(var "ANVILOCK__HAS_${upper}")
    if(${prefix} STREQUAL "C")
      check_include_file(${hdr} ${var})
    else()
      check_include_file_cxx(${hdr} ${var})
    endif()
  endforeach()
endfunction()

set(STANDARD_C_HEADERS
  assert.h ctype.h errno.h fcntl.h float.h inttypes.h limits.h math.h setjmp.h
  signal.h stdarg.h stddef.h stdio.h stdlib.h string.h time.h unistd.h stdbool.h
  fenv.h sys/types.h stdint.h
)

set(STANDARD_CXX_HEADERS
  # I/O
  iostream fstream sstream syncstream print

  # Containers
  vector array list deque forward_list span
  map unordered_map multimap unordered_multimap
  set unordered_set multiset unordered_multiset
  queue stack

  # Algorithms and numeric
  algorithm numeric iterator execution

  # Strings and text
  string string_view charconv format regex

  # Utilities
  memory functional utility type_traits typeindex
  any optional variant tuple bitset bit
  limits cstddef cstdint climits cfloat

  # Error handling
  exception stdexcept system_error

  # Multithreading
  thread mutex shared_mutex future condition_variable
  atomic stop_token

  # Time
  chrono

  # Random
  random

  # Coroutines
  coroutine

  # Modern C++ concepts and meta
  concepts version source_location

  # Filesystem
  filesystem

  # Math
  cmath numbers

  # New in C++23 (conditionally supported)
  expected stacktrace
)

set(SYSTEM_HEADERS
  windows.h sys/stat.h sys/mman.h dirent.h pthread.h dlfcn.h complex
)
