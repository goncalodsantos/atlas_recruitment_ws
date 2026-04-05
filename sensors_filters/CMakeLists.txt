cmake_minimum_required(VERSION 3.8)
project(sensors_filters)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
#find_package(pcl_conversions REQUIRED)
#find_package(pcl_ros REQUIRED)

#find_package(PCL REQUIRED)
#include_directories(${PCL_INCLUDE_DIRS})
#link_directories(${PCL_LIBRARY_DIRS})
#add_definitions(${PCL_DEFINITIONS})

# Add the executable of the laser scan filter node
add_executable(lidar2d_filter src/lidar2d_filter.cpp)
ament_target_dependencies(lidar2d_filter rclcpp sensor_msgs)

install(TARGETS lidar2d_filter
  DESTINATION lib/${PROJECT_NAME})

# Install the launch files
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY launch rviz
  DESTINATION share/${PROJECT_NAME}
)

# Add the executable of the PointCloud filter node
#add_executable(pointcloud_filter_node src/pointcloud_filter_node.cpp)
#ament_target_dependencies(pointcloud_filter_node rclcpp sensor_msgs pcl_conversions pcl_ros) # Use pcl for voxel grid filtering
#target_link_libraries(pointcloud_filter_node ${PCL_LIBRARIES})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
