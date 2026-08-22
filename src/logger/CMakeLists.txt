set(SOURCES
    Logger.cpp
    FileSink.cpp
    SocketSink.cpp
)

add_library(logger ${SOURCES})

target_include_directories(logger PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(logger PUBLIC cxx_std_17)