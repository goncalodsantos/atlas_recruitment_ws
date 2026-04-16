import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    # packages paths
    pkg_turtlebot3_gazebo = get_package_share_directory('turtlebot3_gazebo')
    pkg_sensors_filters = get_package_share_directory('sensors_filters')

    # Launch of Gazebo (TurtleBot3 World)
    gazebo_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_turtlebot3_gazebo, 'launch', 'turtlebot3_world.launch.py')
        )
    )

    # RViz2 node with the config file
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', os.path.join(pkg_sensors_filters, 'rviz', 'scan_filter.rviz')],
        output='screen'
    )

    imu_filter_node = Node(
        package='sensors_filters',
        executable='imu_lowpass_filter',
        name='imu_lowpass_filter',
        parameters=[{'alpha': 0.2}],
        output='screen'
    )

    ekf_localization_node = Node(
        package='robot_localization',
        executable='ekf_node',
        name='ekf_filter_node',
        parameters=[os.path.join(pkg_sensors_filters, 'config', 'ekf_config.yaml')],
        output='screen'
    )

    return LaunchDescription([
        gazebo_launch,
        imu_filter_node,
        ekf_localization_node,
        rviz_node
    ])