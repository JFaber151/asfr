#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <std_msgs/msg/bool.hpp>

#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <memory>
#include <string>
#include <cmath>

//classes
//inherit from rclcpp::Node
class BrightnessNode : public rclcpp::Node
{
public:
  BrightnessNode()
  : Node("brightness_node")
  {
    //decclare node and name it
    //name is given above in Node(brightness_node)

    //parameter declaration
    this->declare_parameter<double>("threshold", 120.0);
    this->declare_parameter<double>("log_period_s", 1.0);

    //publisher and Subscriber
    //we publish on /light_on and subscribe to /image with a que of 1
    pub_ = this->create_publisher<std_msgs::msg::Bool>("/light_on", 1);

    sub_ = this->create_subscription<sensor_msgs::msg::Image>("/image",1,
      std::bind(&BrightnessNode::cb, this, std::placeholders::_1)
    );

  RCLCPP_INFO(this->get_logger(), "brightness_node: /image -> /light_on");  }

private:
  //callback function, image handling to determine if our light is on or off
  void cb(const sensor_msgs::msg::Image::SharedPtr msg)
  {
    //we convert ros message to an opencv img array
    cv_bridge::CvImageConstPtr cv_ptr;
    try {
      cv_ptr = cv_bridge::toCvShare(msg, "bgr8");
    } catch (const cv_bridge::Exception & e) {
      RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
      return;
    }

    const cv::Mat & frame = cv_ptr->image;

    //convert to grey scale to evaluate
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    //we calc the mean value
    const double mean_val = cv::mean(gray)[0];

    //reading current value of thershold
    const double thresh = this->get_parameter("threshold").as_double();

    //logic for light on value
    const bool light_on = (mean_val >= thresh);

    //message type is of type bool
    std_msgs::msg::Bool output;
    output.data = light_on;

    //we now pubslih our message on our topic /light_out
    pub_->publish(output);

    //console output
    const double log_period_s = this->get_parameter("log_period_s").as_double();
    const int64_t period_ms = static_cast<int64_t>(std::llround(log_period_s * 1000.0));
    
    RCLCPP_INFO_THROTTLE(
      this->get_logger(),
      *this->get_clock(),
      period_ms,
      "mean=%.1f, thr=%.1f, light_on=%s",
      mean_val,
      thresh,
      light_on ? "true" : "false"
    );
  }

  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr pub_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BrightnessNode>());
  rclcpp::shutdown();
  return 0;
}