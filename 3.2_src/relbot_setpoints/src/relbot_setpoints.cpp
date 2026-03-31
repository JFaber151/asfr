#include <chrono>
#include <functional>
#include <memory>
#include <string>


#include "example_interfaces/msg/float64.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("relbot_setpoints")
    {
      count_ = 0;
      publisherRight_ = this->create_publisher<example_interfaces::msg::Float64>("input/right_motor/setpoint_vel", 10);
      timerRight_ = this->create_wall_timer(
      500ms, std::bind(&MinimalPublisher::timer_callback_right, this));
      publisherLeft_ = this->create_publisher<example_interfaces::msg::Float64>("input/left_motor/setpoint_vel", 10);
      timerLeft_ = this->create_wall_timer(
      500ms, std::bind(&MinimalPublisher::timer_callback_left, this));
    }

  private:

    int count_;
    void timer_callback_right()
    {
      auto message = example_interfaces::msg::Float64();
      message.data = 10;
      count_++;
      if((count_/20)%2 == 0){
        message.data = message.data * -1;
      }
      //RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
      publisherRight_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timerRight_;
    rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr publisherRight_;
    
    void timer_callback_left()
    {
      auto message = example_interfaces::msg::Float64();
      message.data = -10;
      if((count_/20)%2 == 0){
        message.data = message.data * -1;
      }
      //RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
      publisherLeft_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timerLeft_;
    rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr publisherLeft_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}