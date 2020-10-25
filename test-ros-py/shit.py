#! /usr/bin/env python
# encoding:utf-8

import serial
import time
import matplotlib.pyplot as plt
from multiprocessing import Process, Value, Array, Lock
import time
from sensor_msgs.msg import Image
import rospy
import cv2
from cv_bridge import CvBridge, CvBridgeError
import sys
import os
import socket

class GetColorImageInfo:
    def __init__(self, topic):
        self.topic = topic
        self.bridge = CvBridge()
        self.sub = rospy.Subscriber(topic, Image, self.ImageCallBack)
    
    def ImageCallBack(self, data):
        try:
            cv_image = self.bridge.imgmsg_to_cv2(data, data.encoding)
            pix = (data.width/2, data.height/2)
            sys.stdout.write('%s: color at center(%d, %d): %s \r' % (self.topic, pix[0], pix[1], cv_image[pix[1], pix[0]]))
            # print '%s: Depth at center(%d, %d): %f(mm)\r' % (self.topic, pix[0], pix[1], cv_image[pix[1], pix[0]])
            # for i in range(0, 12):
            #     for j in range(0, 16):
            #         sys.stdout.write('%f ' % (cv_image[i*40, j*40]))
            #         # sys.stdout.write('%d %d ' % (i, j))
            #     sys.stdout.write('\n')
            # time.sleep(10)
            sys.stdout.flush()
        except CvBridgeError as e:
            print(e)
            return

    def Show(self, cv_DepthData):
        # assert isinstance(DepthData, Image)
        # depth = self.bridge.imgmsg_to_cv2(DepthData, DepthData.encoding)
        cv2.imshow('depth', cv_DepthData)
        cv2.waitKey(0)
        time.sleep(1)
        return

def main():
    node_name = os.path.basename(sys.argv[0]).split('.')[0]
    rospy.init_node(node_name)
    topic = '/camera/color/image_raw'
    listener = GetColorImageInfo(topic)
    rospy.loginfo("hello")
    rospy.spin()

def reg():
    # HOST = '115.157.195.140'
    HOST = '127.0.0.1'
    PORT = 8888

    myip = "0.0.0.0"
    myport = "8892"

    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)       #定义socket类型，网络通信，TCP
    s.connect((HOST,PORT))        #要连接的IP与端口
    cmd = "1" + "[name:winblue;ip:" + myip + ";port:" + myport + ";]"        #与人交互，输入命令
    s.sendall(cmd.encode())       #把命令发送给对端
    data = s.recv( 1024 )      #把接收的数据定义为变量
    print(data)          #输出变量
    s.close()    #关闭连接

    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)       #定义socket类型，网络通信，TCP
    s.connect((HOST,PORT))        #要连接的IP与端口
    cmd = "3" + "[name:winblue;pub:bluedata;]"        #与人交互，输入命令
    s.sendall(cmd.encode())       #把命令发送给对端
    data = s.recv( 1024 )      #把接收的数据定义为变量
    print(data)          #输出变量
    s.close()    #关闭连接

    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)       #定义socket类型，网络通信，TCP
    s.connect((HOST,PORT))        #要连接的IP与端口
    cmd = "4" + "[name:winblue;pub:bluedata;" + str(5) + ",]"        #与人交互，输入命令
    s.sendall(cmd.encode())       #把命令发送给对端
    data = s.recv( 1024 )      #把接收的数据定义为变量
    print(data)          #输出变量
    s.close()    #关闭连接

if __name__ == "__main__":
    value = 0
    process_list = []
    p1 = Process(target=main)
    p2 = Process(target=reg)
    p1.start()
    p2.start()
    process_list.append(p1)
    process_list.append(p2)
    while value != -1:
        value = int(input("value = "))
    
    for p in process_list:
        p.join()
