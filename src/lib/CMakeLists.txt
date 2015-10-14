project(libfwtool)

set(libfwtool_SRCS
        aes.c
        csum.c
        endian.c
        fdat_cipher.c
        fdat_cipher_aes.c
        fdat_cipher_sha1.c
        fdat_crypt.c
        fdat_image.c
        fwd_chunks.c
        fwd_pack.c
        fwt_util.c
        fwt_uxbrowse.c
        lz77_inflate.c
        lzpt_io.c
        sha1.c
        tarfile.c
        zipfile.c
        )

include_directories(
        ${libarchive_SOURCE_DIR}/libarchive
        ${zlib_SOURCE_DIR}
)

add_library(libfwtool ${libfwtool_SRCS})

# specify the libs to be linked
target_link_libraries(libfwtool
        archive_static
        zlibstatic
        minizip
        )
