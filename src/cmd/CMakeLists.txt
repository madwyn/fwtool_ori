project(fwtool)

set(fwtool_SRCS
        fwtool.c
        )

include_directories(
        ${libfwtool_SOURCE_DIR}
)

add_executable(fwtool ${fwtool_SRCS})

# specify the libs to be linked
target_link_libraries(fwtool
        libfwtool
        )
