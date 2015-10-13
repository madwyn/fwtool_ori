project(minizip)

if(NOT ZLIB_ROOT_DIR)
    message(FATAL_ERROR "ZLIB_ROOT_DIR must be set to the location of zlib sources.")
else()
    message(STATUS "zlib sources in ${ZLIB_ROOT_DIR}")
endif()

set(MINIZIP_ROOT_DIR ${ZLIB_ROOT_DIR}/contrib/minizip)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# set header files
set(minizip_HDRS
        ${MINIZIP_ROOT_DIR}/zip.h
)

# set source files
set(minizip_SRCS
        ${MINIZIP_ROOT_DIR}/ioapi.c
#        ${MINIZIP_ROOT_DIR}/iowin32.c
        ${MINIZIP_ROOT_DIR}/miniunz.c
        ${MINIZIP_ROOT_DIR}/minizip.c
        ${MINIZIP_ROOT_DIR}/mztools.c
        ${MINIZIP_ROOT_DIR}/unzip.c
        ${MINIZIP_ROOT_DIR}/zip.c
)

# set lib
add_library(minizip ${minizip_SRCS})

target_link_libraries(minizip zlib)

target_include_directories(minizip PUBLIC "${MINIZIP_ROOT_DIR}")

set(INS_TARGETS
        minizip
)

# Install
install(TARGETS ${INS_TARGETS}
        RUNTIME DESTINATION bin COMPONENT applications
        LIBRARY DESTINATION lib COMPONENT libraries
        ARCHIVE DESTINATION lib COMPONENT libraries
)