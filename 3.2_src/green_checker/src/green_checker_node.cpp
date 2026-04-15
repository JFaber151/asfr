//=============================================================================
// Authors : Joran Faber & Deborah Schrag
// Group : 
// License : LGPL open source license
//
// Brief : A node that 
//
//=============================================================================

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <stdint.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui.hpp>
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
      this->declare_parameter("tolerance", 50); //Manually tested and set the tolerance to 50
    }

  private:
    void find_green_callback(const sensor_msgs::msg::Image & msg)
    {
      uint64_t totalX = 0, totalY = 0;
      int found = 0, tolerance = this->get_parameter("tolerance").as_int();

      auto cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
      cv::Mat &img = cv_ptr->image;

      for(uint i = 0; i < msg.height; i++){
        for(uint j = 0; j < msg.width * 3; j+= 3){

          int index = j + (i * msg.step);

          uint b = msg.data[index];
          uint g = msg.data[index + 1];
          uint r = msg.data[index + 2];

          if(g > r + tolerance && g > b + tolerance){
            found += 1;
            totalX += j/3;
            totalY += i;

            // Highlight pixel in red
            img.at<cv::Vec3b>(i, j/3) = cv::Vec3b(0, 0, 255);
          }else{
            img.at<cv::Vec3b>(i, j/3) = cv::Vec3b(0, 0, 0);
          }
        }
      }

      uint CoM_x = 0, CoM_y = 0;
      if(found){
        CoM_x = totalX / found;
        CoM_y = totalY / found;

        cv::circle(img, cv::Point(CoM_x, CoM_y), 10, cv::Scalar(255, 0, 0), 2);
      } else {
        RCLCPP_INFO(this->get_logger(), "not green :(");
      }

      cv::imshow("Green Debug View", img);
      cv::waitKey(1);

      publish_green_info(CoM_x, CoM_y);
    }

    void publish_green_info(uint CoM_x, uint CoM_y){
      auto message = std_msgs::msg::String();
      message.data = std::to_string(CoM_x) + "," + std::to_string(CoM_y);
      publisher_->publish(message);
      RCLCPP_INFO(this->get_logger(), "Publishing: %s", message.data.c_str());
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