set(HEADERFILES
    platform.h
    defines.h
    snenv.h
)

set(SRCS
)

set(SPECIFIC_SRCS
    snenv.c
)

set(SUBDIRS
    win32
    nix
)

set(INCLUDE_BASE "${PROJECT_SOURCE_DIR}/env/include/snenv")

list(TRANSFORM HEADERFILES PREPEND "${INCLUDE_BASE}/")

target_sources(snenv PRIVATE ${HEADERFILES} ${SRCS})

foreach(dir IN LISTS SUBDIRS)
    foreach(src IN LISTS SPECIFIC_SRCS)
        target_sources(snenv PRIVATE "${dir}/${src}")
    endforeach()
endforeach()
