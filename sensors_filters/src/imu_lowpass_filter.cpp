#include <memory>
#include <vector>
#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"

using std::placeholders::_1;

class Imu_LowPassFilter : public rclcpp::Node
{
  public:
    Imu_LowPassFilter() : Node("imu_lowpass_filter")
    { 
      // subscribe to the raw imu topic and publish the filtered imu topic
      subscription_ = this->create_subscription<sensor_msgs::msg::Imu>(
      "imu", 10, std::bind(&Imu_LowPassFilter::imu_callback, this, _1)); 
      
      publisher_ = this->create_publisher<sensor_msgs::msg::Imu>("imu_filtered", 10);
    }

  private:
    // variables to store the last filtered values of linear acceleration and angular velocity for the low-pass filter
    double last_accel_x = 0.0, last_accel_y = 0.0, last_accel_z = 0.0;
    double last_angular_vel_x = 0.0, last_angular_vel_y = 0.0, last_angular_vel_z = 0.0;

    void imu_callback(const sensor_msgs::msg::Imu & msg) {
        auto filtered_imu = std::make_shared<sensor_msgs::msg::Imu>(msg); // create a new message to store the filtered data
        double alpha = 0.2; // smoothing factor for the low-pass filter (0 < alpha < 1)

        // moving average low-pass filter: y[n] = alpha*x[n] + (1-alpha)*y[n-1]
        // low-pass filter for linear acceleration
        filtered_imu->linear_acceleration.x = alpha * msg.linear_acceleration.x + (1 - alpha) * last_accel_x; 
        filtered_imu->linear_acceleration.y = alpha * msg.linear_acceleration.y + (1 - alpha) * last_accel_y; 
        filtered_imu->linear_acceleration.z = alpha * msg.linear_acceleration.z + (1 - alpha) * last_accel_z; 
        last_accel_x = filtered_imu->linear_acceleration.x; // update the last acceleration value for the next iteration
        last_accel_y = filtered_imu->linear_acceleration.y;
        last_accel_z = filtered_imu->linear_acceleration.z;

        // low-pass filter for angular velocity
        filtered_imu->angular_velocity.x = alpha * msg.angular_velocity.x + (1 - alpha) * last_angular_vel_x;
        filtered_imu->angular_velocity.y = alpha * msg.angular_velocity.y + (1 - alpha) * last_angular_vel_y;
        filtered_imu->angular_velocity.z = alpha * msg.angular_velocity.z + (1 - alpha) * last_angular_vel_z;
        last_angular_vel_x = filtered_imu->angular_velocity.x;
        last_angular_vel_y = filtered_imu->angular_velocity.y;
        last_angular_vel_z = filtered_imu->angular_velocity.z;

        publisher_->publish(*filtered_imu); // publish the filtered imu
    }
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr subscription_; 
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Imu_LowPassFilter>());
  rclcpp::shutdown();
  return 0;
}