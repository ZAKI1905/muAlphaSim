cmake_minimum_required(VERSION 3.16)

# Automatically detect the macOS SDK path.
execute_process(
    COMMAND xcrun --sdk macosx --show-sdk-path
    OUTPUT_VARIABLE MACOS_SDK_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set the sysroot to the detected path. This forces CMake to use that SDK.
set(CMAKE_OSX_SYSROOT "${MACOS_SDK_PATH}" CACHE STRING "macOS SDK path" FORCE)

# Use Homebrew LLVM compilers consistently.
set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")


project(muAlphaSim)

find_package(Geant4 REQUIRED COMPONENTS ui_all vis_all)


include(${Geant4_USE_FILE})

file(GLOB sources src/*.cc)
file(GLOB headers include/*.hh)

# Only enable on Clang/GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(
    -fsanitize=address
    -fno-omit-frame-pointer   # better backtraces
    -g                         # include debug info
  )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

add_executable(main 
	${sources} 
	${headers})
target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(main ${Geant4_LIBRARIES})