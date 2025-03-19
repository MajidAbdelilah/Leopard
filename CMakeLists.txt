cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Leopard VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Source files
set(SOURCES
    src/main.cpp
    # Add more source files here
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link TBB library
target_link_libraries(${PROJECT_NAME} PRIVATE )

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -lstdc++)
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
