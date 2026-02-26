cmake_minimum_required(VERSION 3.8)
project(brightness_adjuster)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(brightness_node src/brightness_adjuster.cpp)

ament_target_dependencies(brightness_node
  rclcpp
  sensor_msgs
  std_msgs
  cv_bridge
)

target_link_libraries(brightness_node ${OpenCV_LIBS})

install(TARGETS brightness_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()