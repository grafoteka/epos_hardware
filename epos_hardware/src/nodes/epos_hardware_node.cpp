#include <ros/ros.h>
#include <ros/spinner.h>
#include "epos_hardware/epos_hardware.h"
#include <controller_manager/controller_manager.h>
#include <vector>

int main(int argc, char** argv) {
  ros::init(argc, argv, "epos_velocity_hardware");
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");
  ROS_INFO("%d", argc);
//  std::string motor1 = "motor1";
//  std::string motor2 = "motor2";
//  std::string motor3 = "motor3";
  std::vector<std::string> motor_names;
  //motor_names = [motor1, motor2, motor3];

  for(int i = 0; i < argc-1; ++i) {
    motor_names.push_back(argv[i+1]);
  }

//  for (int i = 0; i < motor_names.size(); i++) {
//    ROS_INFO("%s", motor_names[i].c_str());
//  }

  epos_hardware::EposHardware robot(nh, pnh, motor_names);
  controller_manager::ControllerManager cm(&robot, nh);

  ros::AsyncSpinner spinner(1);
  spinner.start();

  ROS_INFO("Initializing Motors");
  if(!robot.init()) {
    ROS_FATAL("Failed to initialize motors");
    return 1;
  }
  ROS_INFO("Motors Initialized");

  ros::Rate controller_rate(50);
  ros::Time last = ros::Time::now();
  while (ros::ok()) {
    robot.read();
    ros::Time now = ros::Time::now();
    cm.update(now, now-last);
    robot.write();
    last = now;
    robot.update_diagnostics();
    controller_rate.sleep();
  }

}
