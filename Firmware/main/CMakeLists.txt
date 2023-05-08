# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.cpp settings.c         # list the source files of this component
    EMBED_TXTFILES howsmyssl_com_root_cert.pem
                    postman_root_cert.pem
    INCLUDE_DIRS ./opencv
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)

add_prebuilt_library(opencv_imgcodecs "opencv/libopencv_imgcodecs.a")
add_prebuilt_library(libpng "opencv/3rdparty/liblibpng.a")
add_prebuilt_library(libzlib "opencv/3rdparty/libzlib.a")
add_prebuilt_library(opencv_imgproc "opencv/libopencv_imgproc.a")
add_prebuilt_library(opencv_core "opencv/libopencv_core.a")
add_prebuilt_library(ade "opencv/libade.a")

set(IMGCODEC_DEP libpng libzlib)
target_link_libraries(opencv_imgcodecs INTERFACE ${IMGCODEC_DEP})

set(CORE_DEP libzlib)
target_link_libraries(opencv_core INTERFACE ${CORE_DEP})

set(OPENCV_DEP opencv_imgcodecs opencv_imgproc opencv_core)
target_link_libraries(${COMPONENT_LIB} ${OPENCV_DEP})