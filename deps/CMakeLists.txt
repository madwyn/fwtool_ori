project(deps)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libarchive)
add_subdirectory(zlib)

# for the command line UI
set(CARGO_BUILD_STATIC_LIB TRUE)
set(CARGO_TEST FALSE)
add_subdirectory(cargo)

set(ZLIB_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/zlib)
add_subdirectory(minizip-cmake)