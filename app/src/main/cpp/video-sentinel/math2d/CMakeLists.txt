include_directories( ${CMAKE_SOURCE_DIR}/video-sentinel )

add_library( math2d STATIC math2d.cpp )
target_include_directories(math2d PUBLIC {CMAKE_SOURCE_DIR}/video-sentinel)