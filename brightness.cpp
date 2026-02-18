#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <stdint.h>
using std::placeholders::_1;

class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber()
    : Node("brightness_node")
    {
      RCLCPP_INFO(this->get_logger(), "I'm alive");
      subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
        "image", 10, std::bind(&MinimalSubscriber::frame_brightness_callback, this, _1));  
      publisher_ = this->create_publisher<std_msgs::msg::String>(
        "light", 10);
      this->declare_parameter("threshold", 110);
    }

  private:
    void frame_brightness_callback(const sensor_msgs::msg::Image & msg)
    {
      uint brightness = 0;
      for(uint i = 0; i < msg.width * 3; i+= 3){
        for(uint j = 0; j < msg.height; j++){
          //Technically, openCV uses bgr instead of rgb. But we ignore this for now
          uint r = msg.data[j*msg.step + i];
          uint g = msg.data[j*msg.step + i + 1];
          uint b = msg.data[j*msg.step + i + 2];
          brightness += perceivedBrightness(r,g,b);
          //total += basicBrightness(r, g, b);
        }
      }
      brightness = brightness/(msg.width * msg.height);
      publish_light_info(brightness);
    }
    void publish_light_info(uint brightness){
      auto message = std_msgs::msg::String();
      uint threshold = this->get_parameter("threshold").as_int();
      message.data = std::string("The light is turned ") + (brightness < threshold ? "off" : "on");
      publisher_->publish(message);
      RCLCPP_INFO(this->get_logger(), "%s", message.data.c_str());
    }

    int basicBrightness(uint r, uint g, uint b){
      //This function calculates brightness by just taking the average color values
      return (r+g+b)/3;
    }
    int perceivedBrightness(uint r, uint g, uint b){
      //This function calculates the perceived brightness by putting different weights on each color
      return (0.2*r) + (0.7*g) + (0.1*b);
    }
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}