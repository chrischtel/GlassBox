cmake_minimum_required(VERSION 3.15)

project(GlassBox LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build GlassBox as a shared library" OFF)

# Library target
add_library(GlassBox
    GlassBox.cpp
    GlassBox.h
)

target_compile_features(GlassBox PUBLIC cxx_std_23)

add_subdirectory(examples)


# Install rules (optional)
install(TARGETS GlassBox
    EXPORT GlassBoxTargets
    DESTINATION lib
)
install(FILES GlassBox.h DESTINATION include)
