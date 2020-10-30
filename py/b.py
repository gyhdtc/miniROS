#! /usr/bin/env python
# encoding:utf-8

import serial
import time
import matplotlib.pyplot as plt
from multiprocessing import Process, Value, Array, Lock
import socket
#import rospy

# def GetBlueData(shareValue,shareArray,indexArray,lock):
#     CommandList = [
#         [1,4,40000,40000,40000,40000,40000,2], 
#         [1,3,0,0,0,0,0,2], 
#         [1,5,0,0,0,0,0,2], 
#         [1,7,46861,23560,35278,36667,22108,2], 
#         [1,7,20000,20000,20000,20000,20000,2]]
 
#     ser = serial.Serial('com3', 115200, timeout=0.5)
#     f = open("test.txt","w")

#     print("write 串口状态:" + str(ser.is_open))

#     while ser.isOpen() and shareValue.value != -1:
#         if shareValue.value == 0:
#             if (ser.in_waiting > 20):
#                 buffer = ser.read(20)
#                 for i in range(10):
#                     x = buffer[i*2] + buffer[i*2+1]*256
#                     #print(x)
#                     f.write(str(x)+" ")
#                     with lock:
#                         shareArray[indexArray.value] = x
#                     indexArray.value = indexArray.value + 1
#                 if indexArray.value % 10 != 0:
#                     indexArray.value = (int(indexArray.value/10)+1)*10
#                 if indexArray.value >= 1000:
#                     indexArray.value = 0
#                 f.write("\n")
#         else:
#             print("write...")
#             for x in CommandList[int(shareValue.value)-1]:
#                 command_bytes = int.to_bytes(x,2,byteorder="little")
#                 ser.write(command_bytes)
#                 print("done")
#             shareValue.value = 0
#     f.close()
    
def reg():
    # HOST = '115.157.195.140'
    # HOST = '49.123.118.159'
    HOST = '127.0.0.1'
    PORT = 8888

    #myip = "0.0.0.0"
    myip = "49.123.92.180"
    myport = "8892"

    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)       #定义socket类型，网络通信，TCP
    s.connect((HOST,PORT))        #要连接的IP与端口
    cmd = "1" + "[name:winblue;ip:" + myip + ";port:" + myport + ";]"        #与人交互，输入命令
    s.sendall(cmd.encode())       #把命令发送给对端
    # data = s.recv( 1024 )      #把接收的数据定义为变量
    # print(data)          #输出变量
    s.close()    #关闭连接

    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)       #定义socket类型，网络通信，TCP
    s.connect((HOST,PORT))        #要连接的IP与端口
    cmd = "2" + "[name:winblue;sub:bluedata;]"        #与人交互，输入命令
    s.sendall(cmd.encode())       #把命令发送给对端
    # data = s.recv( 1024 )      #把接收的数据定义为变量
    # print(data)          #输出变量
    s.close()    #关闭连接

def server():
	# myip = "49.123.92.180"
    myip = "0.0.0.0"
    myport = 8892
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)    #定义socket类型，网络通信，TCP
    s.bind((myip,myport))    #套接字绑定的IP与端口
    s.listen(10)          #开始TCP监听

    CommandList=[[1,4,40000,40000,40000,40000,40000,2],[1,3,0,0,0,0,0,2],[1,5,0,0,0,0,0,2],[1,7,46861,23560,35278,36667,22108,2],[1,7,20000,20000,20000,20000,20000,2]]
	# ser = serial.Serial('com3', 115200, timeout=0.5)
    flag = 0
	
	# print("write 串口状态:" + str(ser.is_open))
	
	# while ser.isOpen() and flag != -1:
    while flag != -1:
        print("wait for connect")
        conn,addr = s.accept()    #接受TCP连接，并返回新的套接字与IP地址
        print('Connected by' ,addr)     #输出客户端的IP地址
        data = conn.recv( 1024 )     #把接收的数据实例化
        print(data)
        flag = int(data)
		# print(flag)
		# if flag != 0:
		# 	print("write...")
		# 	for x in CommandList[flag-1]:
		# 		command_bytes = int.to_bytes(x,2,byteorder="little")
		# 		ser.write(command_bytes)
		# 		print("done")
		# conn.close()      #关闭连接

def ShowBlueData(shareValue,shareArray,lock):
    position = [[0],[0],[0],[0],[0]]
    current = [[0],[0],[0],[0],[0]]
    x = []
    plt.ion()
    for i in range(5):
        for j in range(99):
            position[i].append(0)
            current[i].append(0)
    for i in range(100):
        x.append(i)

    while  shareValue.value != -1:
        with lock:
            for i in range(5):
                for j in range(100):
                    position[i][j] = shareArray[j*10]
                    current[i][j] = shareArray[j*10+5]    
        for i in range(5):
            plt.plot(x, current[i], label = "pos " + str(i))
        plt.ioff()
        plt.pause(1)
        plt.cla()
        plt.show()

if __name__ == '__main__':
    process_list = []
    shareValue = Value('i', 0)
    indexArray = Value('i', 0)
    shareArray = Array('i', 1000)
    lock = Lock()
    for i in range(1000):
        shareArray[i] = 0

    #p1 = Process(target=GetBlueData, args=(shareValue,shareArray,indexArray,lock,))
    p2 = Process(target=server)
    p3 = Process(target=reg)
    #p1.start()
    p2.start()
    p3.start()
    #process_list.append(p1)
    process_list.append(p2)
    process_list.append(p3)

    #while shareValue.value != -1:
     #   shareValue.value = int(input("shareValue.value = "))
    
    for p in process_list:
        p.join()
    
    print("end")
