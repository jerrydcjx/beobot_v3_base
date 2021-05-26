#include <unistd.h>
#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <time.h>
#include <string.h>

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdbool.h>
#include <pigpio.h>

#include "ros/ros.h"
#include "std_msgs/Float64.h"
#include "std_msgs/Bool.h"

extern "C" {
   #include "beobot_v3_base/Constants.h"
   #include "beobot_v3_base/DAC8532.h"
}

float MAX_VOLTAGE = 1.400;
float MIN_VOLTAGE = 1.1800; //STOP VOLTAGE
float RANGE = MAX_VOLTAGE - MIN_VOLTAGE;
float left_spd = 0;
float right_spd = 0;

/*
 *  motor controller node
 *  subscribe to vel topic in % speed, from 0 ro 1.
 *  convert speed to out put
 */

void leftMotorCallback(const std_msgs::Float64::ConstPtr& msg)
{
    ROS_INFO("Left Motor Velocity recieved: [%f]", msg->data);
    float spd = msg->data;
    if(spd < 0 && left_spd >= 0){
        DAC8532_Out_Voltage(channel_A, 0);
        gpioWrite(24, 1);
	usleep(100000);
	gpioWrite(20, 1);  //Set Left motor to reverse
	usleep(100000);
        gpioWrite(24, 0);
    }
    else if(spd >= 0 && left_spd < 0){
        DAC8532_Out_Voltage(channel_A, 0);
        gpioWrite(24, 1);
	usleep(100000);
	gpioWrite(20, 0);  //Set Left motor to reverse
	usleep(100000);
        gpioWrite(24, 0);
    }
    left_spd = spd;
    if(abs(spd) > 1){spd = 1;}
    DAC8532_Out_Voltage(channel_A, RANGE*abs(spd)+MIN_VOLTAGE);
    ROS_INFO("Left Motor Velocity excuted, last spd is %f", left_spd);
}

void rightMotorCallback(const std_msgs::Float64::ConstPtr& msg)
{
    ROS_INFO("Right Motor Velocity recieved: [%f]", msg->data);    
    float spd = msg->data;
    if(spd < 0 && right_spd >= 0){
        DAC8532_Out_Voltage(channel_B, 0);
        gpioWrite(16, 1);
	usleep(100000);
        gpioWrite(22, 1);  //Set Left motor to reverse
        usleep(100000);
        gpioWrite(16, 0);
    }
    else if(spd >= 0 && right_spd < 0){
        DAC8532_Out_Voltage(channel_B, 0);
        gpioWrite(16, 1);
	usleep(100000);
        gpioWrite(22, 0);  //Set Left motor to reverse
        usleep(100000);
        gpioWrite(16, 0);
    }
    right_spd = spd;
    if(abs(spd) > 1){spd = 1;}
    DAC8532_Out_Voltage(channel_B, RANGE*abs(spd)+MIN_VOLTAGE);
    ROS_INFO("Right Motor Velocity excuted,last spd is %f", right_spd);
}

void brakeCallback(const std_msgs::Bool::ConstPtr& msg)
{   
    if(msg->data){
        ROS_INFO("Brake");
        gpioWrite(16, 1);
        gpioWrite(24, 1);
    }
    else{
        ROS_INFO("Brake Release");
        gpioWrite(16, 0);
        gpioWrite(24, 0);
    }
}

int main(int argc, char **argv)
{
    DEV_ModuleInit();
    gpioInitialise();

    gpioSetMode(22, PI_OUTPUT); //right R 
    gpioSetMode(24, PI_OUTPUT); //left R

    gpioSetMode(16, PI_OUTPUT); //Right B
    gpioSetMode(20, PI_OUTPUT); //Left B

    printf("16 %d", gpioWrite(16, 0)); //1 B 0 No B
    printf("20 %d", gpioWrite(20, 0));

    ROS_INFO("Motor Driver Initiated.");

    ros::init(argc, argv, "Motor_Controller");
    ros::NodeHandle l;
    ros::NodeHandle r;
    ros::NodeHandle b;
    ROS_INFO("Motor Controller Node Initiated.");
    
    ros::Subscriber sub_left = l.subscribe("left_vel", 1000, leftMotorCallback);
    ros::Subscriber sub_right = r.subscribe("right_vel", 1000, rightMotorCallback);
    ros::Subscriber sub_brake = b.subscribe("brake", 1000, brakeCallback);

    ros::spin();

    return 0;
}
