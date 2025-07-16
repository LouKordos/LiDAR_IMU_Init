#include <common_lib.h>
#include <rclcpp/rclcpp.hpp>

double get_time_sec(const builtin_interfaces::msg::Time &time)
{
    return rclcpp::Time(time).seconds();
}

rclcpp::Time get_ros_time(double timestamp)
{
    int32_t sec = std::floor(timestamp);
    auto nanosec_d = (timestamp - std::floor(timestamp)) * 1e9;
    uint32_t nanosec = nanosec_d;
    return rclcpp::Time(sec, nanosec);
}