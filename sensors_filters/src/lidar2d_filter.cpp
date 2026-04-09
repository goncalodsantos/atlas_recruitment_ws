#include <memory>
#include <vector>
#include <cmath>
#include <limits>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using std::placeholders::_1;

class Lidar_2D_filter : public rclcpp::Node
{
  public:
    Lidar_2D_filter() : Node("lidar_2d_filter")
    { 
      // declare parameters
      this->declare_parameter<int>("decimation_factor", 1); // parameter to control the decimation of the scan data, default is 1 (no decimation)
      // subscribe to the raw lidar scan topic and publish the filtered scan topic
      subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", 10, std::bind(&Lidar_2D_filter::laser_callback, this, _1)); 
      
      publisher_ = this->create_publisher<sensor_msgs::msg::LaserScan>("scan_filtered", 10);
    }

  private:

    void laser_callback(const sensor_msgs::msg::LaserScan & msg) {
      int step = this->get_parameter("decimation_factor").as_int();   // get the decimation factor from the parameter server
      if (step < 1) step = 1;                                         // protect against invalid decimation factor values
      
      auto filtered_scan = std::make_shared<sensor_msgs::msg::LaserScan>(); // create a new LaserScan message to store the filtered data

      // copy the header and other metadata from the original message to the filtered message
      filtered_scan->header = msg.header;
      filtered_scan->angle_min = msg.angle_min;
      filtered_scan->range_min = msg.range_min;
      filtered_scan->range_max = msg.range_max;
      filtered_scan->scan_time = msg.scan_time;
      filtered_scan->time_increment = msg.time_increment * step;    // adjust the time increment based on the decimation factor
      filtered_scan->angle_increment = msg.angle_increment * step;  // adjust the angle increment based on the decimation factor
      
      // remove NaN and Infs and filter outliers from the ranges and intensities vectors
      for (size_t i = 0; i < msg.ranges.size(); i += step) { // iterate through the ranges vector with a step defined by the decimation factor
        float range = msg.ranges[i];        // get the range value at index i

        // check if the range value is NaN, Inf, or outside the valid range defined by the sensor (outliers)
        if (std::isnan(range) || std::isinf(range) || range <= msg.range_min || range >= msg.range_max) {
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
      // recalculate the angle_max based on the new number of ranges and the adjusted angle increment
      filtered_scan->angle_max = filtered_scan->angle_min + (filtered_scan->ranges.size() - 1) * filtered_scan->angle_increment;
      publisher_->publish(*filtered_scan); // publish the filtered scan message to the "scan_filtered" topic
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