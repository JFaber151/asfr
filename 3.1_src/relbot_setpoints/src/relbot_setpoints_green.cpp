#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "example_interfaces/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

using std::placeholders::_1;

class MinimalPublisher : public rclcpp::Node{
  public:
    MinimalPublisher()
    : Node("relbot_setpoints"){
      //We initialize publishers for the setpoint velocities for each wheel, a subscriber for the coordinates of the green pixels
      //and the default param value for width
      publisherRight_ = this->create_publisher<example_interfaces::msg::Float64>("input/right_motor/setpoint_vel", 10);

      publisherLeft_ = this->create_publisher<example_interfaces::msg::Float64>("input/left_motor/setpoint_vel", 10);

      subscription_ = this->create_subscription<std_msgs::msg::String>(
        "where_green", 10, std::bind(&MinimalPublisher::find_green_callback, this, _1));
      this->declare_parameter("width", 320); //Web<cam width is 320 pixels, but relbot cam might not be. So we leave this as a param
    }

  private:

    void find_green_callback(const std_msgs::msg::String & msg){
      //We have received some coordinates in the from of "x,y"
      //However, we don't really care about the y coordinate. There's no wings on the relbot, for now
      //It would be smarter & faster to send a message to the topic that contains two integers rather than a string. 
      int pos = msg.data.find(",");
      int camWidth = this->get_parameter("width").as_int();
      //int xCord = std::__cxx11::stoi(msg.data.substr(0,pos).c_str()); //use casting
      printf("%s\n", msg.data.substr(0,pos).c_str());
      int xCord = std::stoi(msg.data.substr(0,pos).c_str()); //use stoi
      publish_setpoints(float(xCord - (camWidth/2))/camWidth);
    }

    void publish_setpoints(float distance){
        //We take a relative distance (-0.50 to + 0.50) from the centerpoint of the camera to the location of the green object CoM
        
        float vel = 50; //default velocity
        if(distance < 0.1 && distance > -0.1){ //if the green is centered (enough) we go straight
          auto message = example_interfaces::msg::Float64();
          message.data = vel;
          publisherRight_->publish(message);
          message.data = message.data * -1;
          publisherLeft_->publish(message);
          RCLCPP_INFO(this->get_logger(), "%f , %f", vel, vel * -1);
          return;
        }
        vel = vel * (distance * 2);  //normalized such that +/- 0.5 means 'max' speed turn
        RCLCPP_INFO(this->get_logger(), "%f , %f", vel, vel * -1);
        
        //We make a turn, 
        auto message = example_interfaces::msg::Float64();
        message.data = vel * -1; //multiplying by -1 makes us turn left when the green is on the left side of the cam
        publisherRight_->publish(message);
        publisherLeft_->publish(message);
        
    }

    rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr publisherRight_;
    rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr publisherLeft_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[]){
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}