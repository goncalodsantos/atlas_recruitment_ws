#include <memory>
#include <vector>
#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using std::placeholders::_1;

class Lidar_2D_filter : public rclcpp::Node
{
  public:
    Lidar_2D_filter() : Node("lidar_2d_filter")
    { 
      // subscribe to the raw lidar scan topic and publish the filtered scan topic
      subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", 10, std::bind(&Lidar_2D_filter::topic_callback, this, _1)); 
      
      publisher_ = this->create_publisher<sensor_msgs::msg::LaserScan>("scan_filtered", 10);
    }

  private:

    void topic_callback(const sensor_msgs::msg::LaserScan & msg) {
      auto filtered_scan = std::make_shared<sensor_msgs::msg::LaserScan>(msg); // create a new message to store the filtered data

      // clear the ranges and intensities vectors before filling them with valid values
      filtered_scan->ranges.clear();       
      if (!msg.intensities.empty()) {
        filtered_scan->intensities.clear();
      }
      
      // remove NaN and Infs and filter outliers from the ranges and intensities vectors
      for (size_t i = 0; i < msg.ranges.size(); ++i) {
        float range = msg.ranges[i];        // get the range value at index i

        // check if the range value is NaN, Inf, or outside the valid range defined by the sensor
        if (std::isnan(range) || std::isinf(range) || range <= 0.15 || range >= msg.range_max) {
          filtered_scan->ranges.push_back(std::numeric_limits<float>::quiet_NaN()); // add NaN to the filtered message for invalid range
          if (!msg.intensities.empty()) { 
              filtered_scan->intensities.push_back(0.0); // add 0 intensity for invalid range if the original message has intensity data
          }
        } else {
            filtered_scan->ranges.push_back(range); // add valid range to the filtered message
            // Only add intensity if the original message has intensity data
            if (!msg.intensities.empty()) {
              filtered_scan->intensities.push_back(msg.intensities[i]); // add corresponding intensity to the filtered message
            }
          }
      }
      publisher_->publish(*filtered_scan); // publish the filtered scan
    }
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_; 
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Lidar_2D_filter>());
  rclcpp::shutdown();
  return 0;
}