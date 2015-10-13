project(deps)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libarchive)
add_subdirectory(zlib)

set(ZLIB_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/zlib)
add_subdirectory(minizip-cmake)