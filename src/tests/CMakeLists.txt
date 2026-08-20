set(SOURCES
    test_logger.cpp
)

add_executable(logger_tests ${SOURCES})

target_link_libraries(logger_tests
    logger
)

enable_testing()
add_test(NAME logger_tests COMMAND logger_tests)

target_compile_features(logger_tests PRIVATE cxx_std_17)