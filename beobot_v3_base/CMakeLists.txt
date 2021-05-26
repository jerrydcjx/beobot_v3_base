cmake_minimum_required(VERSION 3.0.2)
project(beobot_v3_base)

find_package(catkin REQUIRED COMPONENTS
	roscpp 
	rospy 
	std_msgs
	geometry_msgs)

catkin_package()
add_compile_options(-lbcm2835 -lm -lpigpio -lpthread)

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(Motor_controller src/DAC8532.c src/Motor_controller.cpp src/DEV_Config.c)
target_compile_options(Motor_controller PRIVATE -g -O0 -Wall -lbcm2835 -lm -lpigpio)
target_link_libraries(Motor_controller ${catkin_LIBRARIES} bcm2835 pigpio)
add_library(Driver src/DAC8532.c src/DEV_Config.c)
target_link_libraries(Motor_controller Driver bcm2835)

add_executable(Diff_node src/Diff_node.cpp)
target_link_libraries(Diff_node ${catkin_LIBRARIES})




