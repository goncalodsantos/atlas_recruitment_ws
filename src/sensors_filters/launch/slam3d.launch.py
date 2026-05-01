import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node

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
        arguments=['-d', os.path.join(pkg_sensors_filters, 'rviz', 'rtabmap_3d.rviz')],
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    imu_filter_node = Node(
        package='sensors_filters',
        executable='imu_lowpass_filter',
        name='imu_lowpass_filter',
        parameters=[{'alpha': 0.2}, {'use_sim_time': True}],
        output='screen'
    )

    pcloud_downsample_node = Node(
        package='sensors_filters',
        executable='pcloud_downsample',
        parameters=[{
            'leaf_size': 0.15,
            'x_min': 0.3,
            'x_max': 20.0,
            'y_min': -10.0,
            'y_max': 11.0,
            'z_min': 0.1,
            'z_max': 5.0
        }, {'use_sim_time': True}],
        name='pcloud_downsample',
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

    rtabmap_node = Node(
        package='rtabmap_slam',
        executable='rtabmap',
        name='rtabmap',
        output='screen',
        parameters=[
            os.path.join(pkg_sensors_filters, 'config', 'rtabmap_config.yaml'),
            {'use_sim_time': True}
        ],
        remappings=[
            ('rgb/image', '/oakd/rgb/preview/image_raw'),
            ('rgb/camera_info', '/oakd/rgb/preview/camera_info'),
            ('depth/image', '/oakd/rgb/preview/depth'), 
            ('odom', '/odometry/filtered'),
        ]
    )

    rtabmap_viz_node = Node(
        package='rtabmap_viz',
        executable='rtabmap_viz',
        name='rtabmap_viz',
        output='screen',
        parameters=[
            os.path.join(pkg_sensors_filters, 'config', 'rtabmap_config.yaml'),
            {'use_sim_time': True}
        ],
        remappings=[
            ('rgb/image', '/oakd/rgb/preview/image_raw'),
            ('rgb/camera_info', '/oakd/rgb/preview/camera_info'),
            ('depth/image', '/oakd/rgb/preview/depth'), 
            ('odom', '/odometry/filtered'),
        ]
    )

    return LaunchDescription([
        gazebo_launch,
        imu_filter_node,
        pcloud_downsample_node,
        ekf_localization_node,
        rtabmap_node,
        rtabmap_viz_node,
        rviz_node
    ])