set(SOURCES main.cpp)
add_executable(stats_app ${SOURCES})
target_compile_features(stats_app PRIVATE cxx_std_17)