project(raytracing_tests)

## Google Test
# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/googletest)
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif ()
# ---

include_directories(../)

add_executable(runAllTests
        intersection_test.cc
        camera_test.cc
        triangle_test.cc)

target_link_libraries(runAllTests gtest gtest_main)
target_link_libraries(runAllTests raytracing)
