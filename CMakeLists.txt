cmake_minimum_required(VERSION 3.16)
project(Motor_Net_CAN_SDK)

set(CMAKE_CXX_STANDARD 14)
aux_source_directory(. SOURCE)
include_directories(.)
add_executable(Motor_Net_CAN_SDK ${SOURCE})
# find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
target_link_libraries(Motor_Net_CAN_SDK pthread)