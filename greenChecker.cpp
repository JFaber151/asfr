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
    : Node("green_checker_node")
    {
      RCLCPP_INFO(this->get_logger(), "I'm alive");
      subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
        "image", 10, std::bind(&MinimalSubscriber::find_green_callback, this, _1));  
      publisher_ = this->create_publisher<std_msgs::msg::String>(
        "where_green", 10);
      this->declare_parameter("tolerance", 50); //went over colors with an rgb tool, 50 seems good
    }

  private:
    void find_green_callback(const sensor_msgs::msg::Image & msg)
    {
      uint totalX = 0, totalY = 0;
      int found = 0;
      for(uint i = 0; i <  msg.height; i++){
        for(uint j = 0; j < msg.width * 3; j+= 3){
          //Technically, openCV uses bgr instead of rgb. But we ignore this for now

          uint r = msg.data[j + (i * msg.step)];
          uint g = msg.data[j + (i * msg.step) + 1];
          uint b = msg.data[j + (i * msg.step) + 2];
          int tolerance = this->get_parameter("tolerance").as_int();
          if(g > r + tolerance && g > b + tolerance){
            //we found a pixel that falls within tolerance to be counted as green
            //increment # of green pixels found, so we know our division to get the CoM of green pixels
            found += 1;

            //add x position to total
            totalX += j/3;
            //add y position to total
            totalY += i;
            }
        }
      }
      uint CoM_x = 0, CoM_y = 0;
      if(found){
        CoM_x = (int) totalX/found;
        CoM_y = (int) totalY/found;
      }
      if(found){
         RCLCPP_INFO(this->get_logger(), "GREEN! found at:(%d, %d)", CoM_x, CoM_y);
      }else{
        RCLCPP_INFO(this->get_logger(), "not green :(");
      }
      //RCLCPP_INFO(this->get_logger(), "BGR at (0,0): %d %d %d", msg.data[0], msg.data[1], msg.data[2]);
      //calculate the CoM
      //publish_green_info(brightness);
    }
    void publish_green_info(uint CoM_x, uint CoM_y){
      auto message = std_msgs::msg::String();
      message.data = std::string("The green is at: ") + std::to_string(CoM_x) + "," + std::to_string(CoM_y);
      publisher_->publish(message);
      RCLCPP_INFO(this->get_logger(), "%s", message.data.c_str());
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