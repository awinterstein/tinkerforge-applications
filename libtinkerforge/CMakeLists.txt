project (TinkerforgeLib)

set (CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE HEADER_FILES "include/*.h")

add_library (tinkerforge SHARED
    ${HEADER_FILES}

    bindings/ip_connection.c
    bindings/bricklet_ambient_light.c
    bindings/bricklet_distance_ir.c
    bindings/bricklet_humidity.c
    bindings/bricklet_lcd_20x4.c
    bindings/bricklet_temperature.c

    AbstractSensor.cpp
    Bricklet.cpp
    BrickletAmbientLight.cpp
    BrickletDistanceIr.cpp
    BrickletHumidity.cpp
    BrickletTemperature.cpp
    ConnectionHandler.cpp
    Lcd.cpp)

target_include_directories(tinkerforge
    PUBLIC include
    PRIVATE include/tinkerforge/bindings)
