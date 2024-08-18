include_directories( ${CMAKE_SOURCE_DIR}/src )

set(OpenCV_STATIC on)
set(OpenCV_DIR $ENV{OPENCV_ANDROID}/sdk/native/jni)
find_package( OpenCV REQUIRED )

set( LIB_SOURCES webcam.cpp)

add_library(webcam STATIC ${LIB_SOURCES} )
target_link_libraries(webcam detection ${OpenCV_LIBS})
target_include_directories(webcam PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(webcam PUBLIC {CMAKE_SOURCE_DIR}/src)