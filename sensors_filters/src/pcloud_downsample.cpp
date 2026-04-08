#include <memory>
#include <vector>
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
      this->declare_parameter<double>("leaf_size", 0.1); // declare a parameter for the leaf size of the voxel grid filter with a default value of 0.1 (10cm)
      // subscribe to the raw pointcloud topic and publish the filtered pointcloud topic
      subscription_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
      "/oakd/rgb/preview/depth/points", 10, std::bind(&PCloud_Downsample::cloud_callback, this, _1)); 
      
      publisher_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("pcloud_downsampled", 10);
    }

  private:

    void cloud_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
    double leaf_size = this->get_parameter("leaf_size").as_double(); // get the leaf size from the parameter server
    // Converter ROS pointcloud msg to PCL binary pointcloud msg
    pcl::PCLPointCloud2::Ptr pcl_pc2(new pcl::PCLPointCloud2());
    pcl_conversions::toPCL(*msg, *pcl_pc2);

    // Voxel Grid
    pcl::PCLPointCloud2::Ptr cloud_filtered(new pcl::PCLPointCloud2());
    pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
    sor.setInputCloud(pcl_pc2);
    sor.setLeafSize(static_cast<float>(leaf_size), static_cast<float>(leaf_size), static_cast<float>(leaf_size)); // Set the leaf size for the voxel grid filter using the parameter value
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