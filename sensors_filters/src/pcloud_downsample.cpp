#include <memory>
#include <vector>
#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/passthrough.h>

using std::placeholders::_1;

class PCloud_Downsample : public rclcpp::Node
{
  public:
    PCloud_Downsample() : Node("pcloud_downsample_node")
    { 
      this->declare_parameter<double>("x_min_distance", 0.3); // declare a parameter for the minimum distance of points to keep along the x axis with a default value of -5.0m
      this->declare_parameter<double>("x_max_distance", 20.0); // declare a parameter for the maximum distance of points to keep along the x axis with a default value of 5.0m
      this->declare_parameter<double>("z_min_distance", 0.1); // declare a parameter for the minimum distance of points to keep along the z axis with a default value of 0.1m
      this->declare_parameter<double>("z_max_distance", 5.0); // declare a parameter for the maximum distance of points to keep along the z axis with a default value of 5.0m
      this->declare_parameter<double>("leaf_size", 0.1); // declare a parameter for the leaf size of the voxel grid filter with a default value of 0.1 (10cm)
      
      // subscribe to the raw pointcloud topic and publish the filtered pointcloud topic
      subscription_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
      "/oakd/rgb/preview/depth/points", 10, std::bind(&PCloud_Downsample::cloud_callback, this, _1)); 
      
      publisher_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("pcloud_downsampled", 10);
    }

  private:

    void cloud_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
    double leaf_size = this->get_parameter("leaf_size").as_double(); // get the leaf size from the parameter server
    double x_min_distance = this->get_parameter("x_min_distance").as_double(); // get the minimum distance from the parameter server
    double x_max_distance = this->get_parameter("x_max_distance").as_double(); // get the maximum distance from the parameter server
    double z_min_distance = this->get_parameter("z_min_distance").as_double(); // get the minimum distance from the parameter server
    double z_max_distance = this->get_parameter("z_max_distance").as_double(); // get the maximum distance from the parameter server

    // Converter ROS pointcloud msg to PCL binary pointcloud msg
    pcl::PCLPointCloud2::Ptr pcl_pc2(new pcl::PCLPointCloud2());
    pcl_conversions::toPCL(*msg, *pcl_pc2);
    
    // PassThrough filter to remove points along the z axis (height)
    pcl::PCLPointCloud2::Ptr cloud_passthrough_z(new pcl::PCLPointCloud2());
    pcl::PassThrough<pcl::PCLPointCloud2> pass_z;
    pass_z.setInputCloud(pcl_pc2);
    pass_z.setFilterFieldName("z"); // along the z axis (height)
    pass_z.setFilterLimits(z_min_distance, z_max_distance); // ignora o chão e o teto
    pass_z.filter(*cloud_passthrough_z);

     // PassThrough filter to remove points along the x axis (width)
    pcl::PCLPointCloud2::Ptr cloud_passthrough_x(new pcl::PCLPointCloud2());
    pcl::PassThrough<pcl::PCLPointCloud2> pass_x;
    pass_x.setInputCloud(cloud_passthrough_z);
    pass_x.setFilterFieldName("x"); // along the x axis (width)
    pass_x.setFilterLimits(x_min_distance, x_max_distance);
    pass_x.filter(*cloud_passthrough_x);

    // Voxel Grid
    pcl::PCLPointCloud2::Ptr cloud_voxel(new pcl::PCLPointCloud2());
    pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
    sor.setInputCloud(cloud_passthrough_x);
    sor.setLeafSize(static_cast<float>(leaf_size), static_cast<float>(leaf_size), static_cast<float>(leaf_size)); // Set the leaf size for the voxel grid filter using the parameter value
    sor.filter(*cloud_voxel);

    // convert filtered PCL msg back to ROS msg and publish
    sensor_msgs::msg::PointCloud2 output;
    pcl_conversions::fromPCL(*cloud_voxel, output);
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