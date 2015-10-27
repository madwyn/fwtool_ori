project(fwtool)

set(fwtool_SRCS
        main.c
        cmd.h)

find_package(gflags REQUIRED)

include_directories(
        ${libfwtool_SOURCE_DIR}
        ${cargo_SOURCE_DIR}
)

add_executable(fwtool ${fwtool_SRCS})

# specify the libs to be linked
target_link_libraries(fwtool
        cargo
        libfwtool
        )
