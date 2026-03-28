set(SRCS
    test.c
)

add_executable(sn_env_test)

target_sources(sn_env_test PRIVATE ${SRCS})

target_link_libraries(sn_env_test PRIVATE snenv)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND SN_ENV_BUILD_SHARED)
    add_custom_target(copy_dlls ALL
        COMMENT "Copy the dlls"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_RUNTIME_DLLS:sn_env_test>" "$<TARGET_FILE_DIR:sn_env_test>"
        COMMAND_EXPAND_LISTS
    )

    add_dependencies(copy_dlls sn_env_test)
endif()

