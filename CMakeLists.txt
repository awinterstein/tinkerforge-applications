cmake_minimum_required(VERSION 3.5)
project (tinkerforge-applications LANGUAGES CXX)

add_definitions(-DSPDLOG_ENABLE_SYSLOG=1)

add_subdirectory (libtinkerforge)
add_subdirectory (sensorlogger)
