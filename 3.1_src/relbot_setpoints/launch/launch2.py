from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
from launch_ros.actions import Node
def generate_launch_description():
    adapter_node = Node(
        package="relbot_adapter",
        executable="relbot_adapter",
        name="relbot_adapter",
        parameters = [
            {"use_twist_cmd" : True},
            {"max_speed_mps" : 5.0},
        ],
        remappings=[
            ( "/output/motor_cmd" , "/input/motor_cmd"),
            ( "/input/twist", "/keyboard/twist")
        ]
    )

    relbot_simulator = Node(
        package="relbot_simulator",
        executable="relbot_simulator",
        name="relbot_sim",
        parameters=[
            {"throttle_rate": 50000000.0},
        ],
    )
    relbot_turtle = Node(
        package="turtlesim",
        executable="turtlesim_node",
        name="robotturtle"
    )
    return LaunchDescription([
        adapter_node,
        relbot_simulator,
        relbot_turtle
    ])