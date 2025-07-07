cmake_minimum_required(VERSION 3.10)
project(AlgorithmImplementation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include header path
include_directories(include)

# Core static library (image I/O and algorithms)
add_library(core STATIC
    src/ImageIO.cpp
    src/ImageIntensityTransformation.cpp
    # You can add more source files here as you create them:
    # src/ImageFilter.cpp
    # src/ImageMorphology.cpp
)

# Test or CLI executable
add_executable(mainApp main.cpp)
target_link_libraries(mainApp core)
