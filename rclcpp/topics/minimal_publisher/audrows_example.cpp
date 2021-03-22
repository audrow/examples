#include <chrono>
#include <string>
#include <memory>
#include <iostream>

#include "rclcpp/any_subscription_callback.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/type_adapter.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

template <>
struct rclcpp::TypeAdapter<std::string, std_msgs::msg::String>
{
  using is_specialized = std::true_type;
  using custom_type = std::string;
  using ros_message_type = std_msgs::msg::String;

  static void
  convert_to_ros_message(
      const custom_type &source,
      ros_message_type &destination)
  {
    destination.data = source;
  }

  static void
  convert_to_custom(
      const ros_message_type &source,
      custom_type &destination)
  {
    destination = source.data;
  }
};

int main(int argc, char * argv[])
{
  #if 0
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("minimal_publisher");

    using AdaptedType = rclcpp::adapt_type<std::string>::as<std_msgs::msg::String>;

    auto publisher = node->create_publisher<std_msgs::msg::String>("topic", 10);
    //auto publisher = node->create_publisher<AdaptedType>("topic", 10);
    auto subscription_ = node->create_subscription<std_msgs::msg::String>(
        "topic",
        10,
        [node](std_msgs::msg::String::UniquePtr msg) {
          RCLCPP_INFO(node->get_logger(), "I heard: '%s'", msg->data.c_str());
        });

    std_msgs::msg::String message;
    rclcpp::WallRate loop_rate(500ms);

    auto publish_count = 0;
    while (rclcpp::ok()) {
      message.data = "Hello, world! " + std::to_string(publish_count++);
      RCLCPP_INFO(node->get_logger(), "Publishing: '%s'", message.data.c_str());
      try {
        publisher->publish(message);
        rclcpp::spin_some(node);
      } catch (const rclcpp::exceptions::RCLError & e) {
        RCLCPP_ERROR(
          node->get_logger(),
          "unexpectedly failed with %s",
          e.what());
      }
      loop_rate.sleep();
    }
    rclcpp::shutdown();
    return 0;
  #endif

  using MessageT = std_msgs::msg::String;
  using AllocatorT = std::allocator<void>;

  auto alloc = std::make_shared<std::allocator<void>>();
  rclcpp::AnySubscriptionCallback<MessageT, AllocatorT> callback(alloc);

  using SharedPtrCallback = std::function<void (const std::shared_ptr<MessageT>)>;
  SharedPtrCallback shared_cb = [](const std::shared_ptr<MessageT> msg) {std::cout << "Calling with data -> " << msg->data << std::endl;};

  auto msg = std::make_shared<MessageT>();
  msg->data = "Hi world";
  callback.set(shared_cb);
  rclcpp::MessageInfo message_info;
  callback.dispatch(msg, message_info);
}
