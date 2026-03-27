if(SN_ENV_BUILD_SHARED)
    add_library(snenv SHARED)
    target_compile_definitions(snenv PRIVATE SN_EXPORT)
else()
    add_library(snenv STATIC)
    target_compile_definitions(snenv PUBLIC SN_ENV_STATIC)
endif()

target_include_directories(snenv PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(snenv PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(snenv PRIVATE sn_env_configs)

add_subdirectory(src)
