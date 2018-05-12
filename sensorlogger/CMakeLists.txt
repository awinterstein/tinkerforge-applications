project (SensorLogger LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable (sensorlogger SensorLogger MqttClient main)
target_include_directories(sensorlogger PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries (sensorlogger tinkerforge pthread mosquitto ${Boost_LIBRARIES})
