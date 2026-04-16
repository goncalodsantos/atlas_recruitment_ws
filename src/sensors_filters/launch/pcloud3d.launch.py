import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    # packages paths
    pkg_sensors_filters = get_package_share_directory('sensors_filters')
    pkg_turtlebot4_ignition = get_package_share_directory('turtlebot4_ignition_bringup')

    gazebo_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_turtlebot4_ignition, 'launch', 'turtlebot4_ignition.launch.py')
        ),
        launch_arguments={'world': 'warehouse'}.items()
    )

    # RViz2 node with the config file
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', os.path.join(pkg_sensors_filters, 'rviz', 'pcloud3d_filter.rviz')],
        output='screen'
    )

    imu_filter_node = Node(
        package='sensors_filters',
        executable='imu_lowpass_filter',
        name='imu_lowpass_filter',
        parameters=[{'alpha': 0.2}],
        output='screen'
    )

    pcloud_downsample_node = Node(
        package='sensors_filters',
        executable='pcloud_downsample',
        parameters=[{'leaf_size': 0.1}, {'x_min_distance': 0.3}, {'x_max_distance': 20.0}, {'z_min_distance': 0.1}, {'z_max_distance': 5.0}],
        name='pcloud_downsample',
        output='screen'
    )

    return LaunchDescription([
        rviz_node,
        pcloud_downsample_node,
        imu_filter_node,
        gazebo_launch
    ])