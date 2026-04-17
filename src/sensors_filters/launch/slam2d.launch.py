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
        arguments=['-d', os.path.join(pkg_sensors_filters, 'rviz', 'ekf.rviz')],
        output='screen'
    )

    # lidar 2d scan filter node
    lidar2d_node = Node(
        package='sensors_filters',
        executable='lidar2d_filter',
        name='lidar_2d_filter',
        parameters=[{'decimation_factor': 2}, {'use_sim_time': True}], # usar o clock do Gazebo para sincronizar os dados de sensores e o tempo de simulação
        output='screen'
    )

    # imu low pass filter node
    imu_filter_node = Node(
        package='sensors_filters',
        executable='imu_lowpass_filter',
        name='imu_lowpass_filter',
        parameters=[{'alpha': 0.2}, {'use_sim_time': True}],
        output='screen'
    )

    # EKF localization node from robot_localization package with the config file
    ekf_localization_node = Node(
        package='robot_localization',
        executable='ekf_node',
        name='ekf_filter_node',
        parameters=[os.path.join(pkg_sensors_filters, 'config', 'ekf_config.yaml'), {'use_sim_time': True}],
        output='screen'
    )

    slamtoolbox_node = Node(
        package='slam_toolbox',
        executable='async_slam_toolbox_node',
        name='slam_toolbox_node',
        parameters=[
            os.path.join(pkg_sensors_filters, 'config', 'slam_toolbox_config.yaml'),
            {'use_sim_time': True} 
        ],
        output='screen'
    )

    return LaunchDescription([
        gazebo_launch,
        imu_filter_node,
        lidar2d_node,
        ekf_localization_node,
        slamtoolbox_node,
        rviz_node
    ])