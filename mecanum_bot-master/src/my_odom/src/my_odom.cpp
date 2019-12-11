#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <rosgraph_msgs/Clock.h>

  double x = 0.0;
  double y = 0.0;
  double th = 0.0;

  double vx = 0;
  double vy = 0;
  double vth = 0;
ros::Time sim_clock;
void updatevelocity(const geometry_msgs::Twist& msg){
  vx = msg.linear.x;
  vy = msg.linear.y;
  vth = msg.angular.z;
}
void updateclock(const rosgraph_msgs::Clock &msg){
  sim_clock = msg.clock;
}
int main(int argc, char** argv){
  ros::init(argc, argv, "odometry_publisher");

  ros::NodeHandle n;
  ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 1000);
  ros::Subscriber vel_sub = n.subscribe("velocity",1000,&updatevelocity);
  ros::Subscriber clock_sub = n.subscribe("clock",1000,&updateclock);
  tf::TransformBroadcaster odom_broadcaster;

  ros::Time current_time, last_time;
  current_time = ros::Time::now();
  last_time = ros::Time::now();

  ros::Rate r(10.0);
  while(n.ok()){

    ros::spinOnce();               // check for incoming messages
    current_time = ros::Time::now();

    //compute odometry in a typical way given the velocities of the robot
    double dt = (current_time - last_time).toSec();
    double delta_x = (vx * cos(th) - vy * sin(th)) * dt;
    double delta_y = (vx * sin(th) + vy * cos(th)) * dt;
    double delta_th = vth * dt;

    x += delta_x;
    y += delta_y;
    th += delta_th;

    //since all odometry is 6DOF we'll need a quaternion created from yaw
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

    //first, we'll publish the transform over tf
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = current_time;
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    odom_trans.transform.translation.x = x;
    odom_trans.transform.translation.y = y;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = odom_quat;

    //send the transform
    odom_broadcaster.sendTransform(odom_trans);

    //next, we'll publish the odometry message over ROS
    nav_msgs::Odometry odom;
    odom.header.stamp = current_time;
    odom.header.frame_id = "odom";
    odom.child_frame_id = "base_link";

    //set the position
    odom.pose.pose.position.x = x;
    odom.pose.pose.position.y = y;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation = odom_quat;

    //set the velocity
    odom.child_frame_id = "base_link";
    odom.twist.twist.linear.x = vx;
    odom.twist.twist.linear.y = vy;
    odom.twist.twist.angular.z = vth;
    /* tf::Quaternion qt(odom_quat.x,odom_quat.y,odom_quat.z,odom_quat.w);
    tf::Vector3 vt(x,y,0);
    tf::Transform base_link_to_odom(qt,vt);
    odom_broadcaster.sendTransform(tf::StampedTransform(base_link_to_odom,current_time,"odom","base_link")); */
    //publish the message
    odom_pub.publish(odom);

    last_time = current_time;
    r.sleep();
  }
}