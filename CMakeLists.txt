cmake_minimum_required(VERSION 3.15) # bump to modern baseline

project(GlassBox LANGUAGES CXX)

# Allow the user to pick static vs shared library
option(BUILD_SHARED_LIBS "Build GlassBox as a shared library" ON)

# Add library target
add_library(GlassBox
    GlassBox.cpp
    GlassBox.h
)

# Set C++ standard
target_compile_features(GlassBox PUBLIC cxx_std_23)

# Installation (optional for OSS)
install(TARGETS GlassBox
    EXPORT GlassBoxTargets
    DESTINATION lib
)
install(FILES GlassBox.h DESTINATION include)

# Example (optional, can be in examples/)
