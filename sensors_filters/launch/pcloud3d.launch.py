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
        output='screen'
    )

    pcloud_downsample_node = Node(
        package='sensors_filters',
        executable='pcloud_downsample',
        name='pcloud_downsample',
        output='screen'
    )

    return LaunchDescription([
        rviz_node,
        pcloud_downsample_node,
        imu_filter_node
    ])