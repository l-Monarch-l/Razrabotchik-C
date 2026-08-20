set(SOURCES
    main.cpp
)

add_executable(logger_app ${SOURCES})

target_link_libraries(logger_app
    logger
)

target_compile_features(logger_app PRIVATE cxx_std_17)