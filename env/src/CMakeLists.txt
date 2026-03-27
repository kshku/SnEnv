set(HEADERFILES
    platform.h
    defines.h
    snenv.h
)

set(SRCS
    snenv.c
)

set(INCLUDE_BASE "${PROJECT_SOURCE_DIR}/env/include/snenv")

list(TRANSFORM HEADERFILES PREPEND "${INCLUDE_BASE}/")

target_sources(snenv PRIVATE ${HEADERFILES} ${SRCS})
