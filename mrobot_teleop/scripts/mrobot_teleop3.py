#!/usr/bin/env python
# -*- coding: utf-8 -*-
import rospy
from geometry_msgs.msg import Twist
import sys, select, termios, tty

def getKey():
	tty.setraw(sys.stdin.fileno())
	select.select([sys.stdin], [], [], 0)
	key = sys.stdin.read(1)
	termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
	return key

#init ros things
rospy.init_node('control')

pub = rospy.Publisher('/cmd_vel', Twist, queue_size=5)


r = rospy.Rate(50)


while not rospy.is_shutdown():
    try:
        key = getKey()
        print("key:  ",key)
    except:
        key = 100
    twist = Twist()
    # move forward
    if key == 'w':
        twist.linear.x = 0.1; 
        twist.linear.y = 0;
        twist.linear.z = 0
        twist.angular.x = 0; 
        twist.angular.y = 0; 
        twist.angular.z = 0
        pub.publish(twist)
    # move back
    if key == 's':
        twist.linear.x = -0.05; 
        twist.linear.y = 0;
        twist.linear.z = 0
        twist.angular.x = 0; 
        twist.angular.y = 0; 
        twist.angular.z = 0
        pub.publish(twist)

    # move left
    if key == 'a':
        twist.linear.x = 0; 
        twist.linear.y = 0.05;
        twist.linear.z = 0
        twist.angular.x = 0; 
        twist.angular.y = 0; 
        twist.angular.z = 0
        pub.publish(twist)
   # move right
    if key == 'd':
        twist.linear.x = 0; 
        twist.linear.y = -0.05;
        twist.linear.z = 0
        twist.angular.x = 0; 
        twist.angular.y = 0; 
        twist.angular.z = 0
        pub.publish(twist)    

   #  left spin
    if key == 'q':
        twist.linear.x = 0; 
        twist.linear.y = 0;
        twist.linear.z = 0
        twist.angular.x = 0; 
        twist.angular.y = 0; 
        twist.angular.z = 0.2
        pub.publish(twist)    
  # spin
    if key == 'q':
        twist.linear.x = 0; 
        twist.linear.y = 0;
        twist.linear.z = 0
        twist.angular.x = 0; 
        twist.angular.y = 0; 
        twist.angular.z = -0.2
        pub.publish(twist)    



    r.sleep()
