cmake_minimum_required(VERSION 3.8)
project(green_checker)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

# Executable
add_executable(green_checker_node src/green_checker_node.cpp)

ament_target_dependencies(green_checker_node
  rclcpp
  std_msgs
  sensor_msgs
)

install(TARGETS
  green_checker_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()