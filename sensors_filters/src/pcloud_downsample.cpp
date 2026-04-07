#include <memory>
#include <vector>
#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/voxel_grid.h>

using std::placeholders::_1;

class PCloud_Downsample : public rclcpp::Node
{
  public:
    PCloud_Downsample() : Node("pcloud_downsample_node")
    { 
      // subscribe to the raw pointcloud topic and publish the filtered pointcloud topic
      subscription_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
      "world/warehouse/model/turtlebot4/link/oakd_rgb_camera_frame/sensor/rgbd_camera/points", 10, std::bind(&PCloud_Downsample::cloud_callback, this, _1)); 
      
      publisher_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("pcloud_downsampled", 10);
    }

  private:

    void cloud_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
    // Converter ROS msg to PCL msg
    pcl::PCLPointCloud2::Ptr pcl_pc2(new pcl::PCLPointCloud2());
    pcl_conversions::toPCL(*msg, *pcl_pc2);

    // Voxel Grid
    pcl::PCLPointCloud2::Ptr cloud_filtered(new pcl::PCLPointCloud2());
    pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
    sor.setInputCloud(pcl_pc2);
    sor.setLeafSize(0.1f, 0.1f, 0.1f); // Tamanho do voxel (10cm)
    sor.filter(*cloud_filtered);

    // convert filtered PCL msg back to ROS msg and publish
    sensor_msgs::msg::PointCloud2 output;
    pcl_conversions::fromPCL(*cloud_filtered, output);
    output.header = msg->header;
    publisher_->publish(output);
    }
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscription_; 
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PCloud_Downsample>());
  rclcpp::shutdown();
  return 0;
}