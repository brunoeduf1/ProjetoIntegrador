# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.cpp settings.c main.py         # list the source files of this component
    EMBED_TXTFILES howsmyssl_com_root_cert.pem
    INCLUDE_DIRS
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
