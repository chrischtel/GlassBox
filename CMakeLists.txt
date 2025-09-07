cmake_minimum_required(VERSION 3.15)

project(GlassBox LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build GlassBox as a shared library" OFF)

# Library target
add_library(GlassBox
    GlassBox.cpp
    GlassBox.h
)

target_compile_features(GlassBox PUBLIC cxx_std_23)

# Example program
add_executable(GlassBoxExample main.cpp)
target_link_libraries(GlassBoxExample PRIVATE GlassBox)
target_compile_features(GlassBoxExample PRIVATE cxx_std_23)

# Install rules (optional)
install(TARGETS GlassBox
    EXPORT GlassBoxTargets
    DESTINATION lib
)
install(FILES GlassBox.h DESTINATION include)
