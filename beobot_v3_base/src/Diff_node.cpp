#include <stdlib.h>     //exit()
#include <signal.h>     //signal()

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ros/ros.h"
#include "std_msgs/Float64.h"
#include "geometry_msgs/Twist.h"


ros::Publisher right_pub;
ros::Publisher left_pub;

void spdCallback(const geometry_msgs::Twist::ConstPtr& msg)
{   
    float radSpd = msg->angular.z;
    float linear = msg->linear.x;
    float left_spd = (linear - radSpd);
    float right_spd = (linear + radSpd);
    std_msgs::Float64 left_msg;
    std_msgs::Float64 right_msg;
    left_msg.data = left_spd;
    right_msg.data = right_spd;
    left_pub.publish(left_msg);
    right_pub.publish(right_msg);
    ROS_INFO("Publishing Left[%f], right[%f]", left_spd, right_spd);
}

int main(int argc, char **argv)
{
    ROS_INFO("Diff Driver Initiated.");

    ros::init(argc, argv, "Diff_node");
    ros::NodeHandle n;
    
    left_pub = n.advertise<std_msgs::Float64>("left_spd_cmd", 1000);
    right_pub = n.advertise<std_msgs::Float64>("right_spd_cmd", 1000);

    ros::Subscriber sub_left = n.subscribe("cmd_vel", 1000, spdCallback);


    ros::spin();

    return 0;
}
