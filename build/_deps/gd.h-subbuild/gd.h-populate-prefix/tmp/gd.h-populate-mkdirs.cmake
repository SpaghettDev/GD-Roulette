# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Projects/GD-Roulette/build/_deps/gd.h-src"
  "D:/Projects/GD-Roulette/build/_deps/gd.h-build"
  "D:/Projects/GD-Roulette/build/_deps/gd.h-subbuild/gd.h-populate-prefix"
  "D:/Projects/GD-Roulette/build/_deps/gd.h-subbuild/gd.h-populate-prefix/tmp"
  "D:/Projects/GD-Roulette/build/_deps/gd.h-subbuild/gd.h-populate-prefix/src/gd.h-populate-stamp"
  "D:/Projects/GD-Roulette/build/_deps/gd.h-subbuild/gd.h-populate-prefix/src"
  "D:/Projects/GD-Roulette/build/_deps/gd.h-subbuild/gd.h-populate-prefix/src/gd.h-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Projects/GD-Roulette/build/_deps/gd.h-subbuild/gd.h-populate-prefix/src/gd.h-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Projects/GD-Roulette/build/_deps/gd.h-subbuild/gd.h-populate-prefix/src/gd.h-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
