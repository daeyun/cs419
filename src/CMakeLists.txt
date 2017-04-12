project(raytracing)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)


set(HEADER_FILES
        common.h
        geom3d.h
        object.h
        ray.h
        file_io.h
        sampling.h
        benchmark.h
        viewing.h
        scene.h
        camera.h
        matrix_utils.h
        )

set(SOURCE_FILES
        geom3d.cc
        object.cc
        ray.cc
        file_io.cc
        sampling.cc
        benchmark.cc
        viewing.cc
        scene.cc
        common.cc
        camera.cc
        matrix_utils.cc)


add_library(raytracing STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(raytracing PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_link_libraries(raytracing ${OpenCV_LIBS})
