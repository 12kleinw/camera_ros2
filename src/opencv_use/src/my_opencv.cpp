#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "cv_bridge/cv_bridge.h"
#include "opencv2/opencv.hpp"

using namespace std::chrono_literals;

class ImageSubscriber : public rclcpp::Node
{
public:
  ImageSubscriber()
  : Node("opencv_image_subscriber")
  {
    // 创建订阅者，订阅图片消息
    image_subscriber_ = this->create_subscription<sensor_msgs::msg::Image>(
      "/camera/color/image_raw", 10, std::bind(&ImageSubscriber::image_callback, this, std::placeholders::_1));

    // 创建 OpenCV 窗口，只需创建一次
   // cv::namedWindow("Received Image", cv::WINDOW_NORMAL);
    cv::namedWindow("Received Image", 1);

    std::cout << "00000" << std::endl;
  }

  ~ImageSubscriber()
  {
    // 销毁窗口，防止程序退出时泄露资源
    cv::destroyWindow("Received Image");
  }

private:
  void image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
  {
    std::cout << msg << std::endl;
    try
    {
      // 将ROS的图像消息转换为OpenCV格式
      cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
      std::cout<< cv_ptr->image.rows << " " << cv_ptr->image.cols << std::endl;
      // 获取OpenCV图像
      cv::Mat &image = cv_ptr->image;

      // 显示图像
      cv::imshow("Received Image", image);
      cv::waitKey(1);  // 非阻塞，适合实时图像流
    }
    catch (cv_bridge::Exception &e)
    {
      RCLCPP_ERROR(this->get_logger(), "Could not convert image: %s", e.what());
    }
  }

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_subscriber_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  // 使用多线程执行器以支持更高的并发
  // rclcpp::executors::MultiThreadedExecutor exec;
  // exec.add_node(std::make_shared<ImageSubscriber>());
  ImageSubscriber sub_camera;
  rclcpp::spin(sub_camera.get_node_base_interface());

  rclcpp::shutdown();
  return 0;
}
