# src/CMakeLists.txt
add_library(YANGBOY_CORE STATIC
    Bus/IBus.h
    Bus/IBus.cpp
    Bus/Bus.h
    Bus/Bus.cpp
    Cpu/SM83.h
    Cpu/SM83.cpp
    Ram/Ram.h
    Ram/Ram.cpp
    IO/IO.h
    IO/IO.cpp
    Cartridge/Cartridge.h
    Cartridge/Cartridge.cpp
    Timer/Timer.h
    Timer/Timer.cpp
    Engine/Engine.h
    Engine/Engine.cpp
    Ppu/Ppu.h
    Ppu/Ppu.cpp
)

target_include_directories(YANGBOY_CORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(YANGBOY_CORE
    PRIVATE SDL2::SDL2
)

add_executable(YANGBoy main.cpp)
target_link_libraries(YANGBoy PRIVATE YANGBOY_CORE SDL2::SDL2main)