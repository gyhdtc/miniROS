#! /usr/bin/env python
# encoding:utf-8

import serial
import time
import matplotlib.pyplot as plt
from multiprocessing import Process, Value, Array, Lock
import rospy

def to_bytes(n, length):
    return bytes( (n >> i*8) & 0xff for i in reversed(range(length)))

def GetBlueData(shareValue,shareArray,indexArray,lock):
    CommandList = [
        [1,4,40000,40000,40000,40000,40000,2], 
        [1,3,0,0,0,0,0,2], 
        [1,5,0,0,0,0,0,2], 
        [1,7,46861,23560,35278,36667,22108,2], 
        [1,7,20000,20000,20000,20000,20000,2]]
 
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0.5)
    # f = open("test.txt","w")

    print("write 串口状态:" + str(ser.is_open))

    while ser.isOpen() and shareValue.value != -1:
        if shareValue.value == 0:
            # if (ser.in_waiting > 20):
            #     buffer = ser.read(20)
            #     for i in range(10):
            #         x = buffer[i*2] + buffer[i*2+1]*256
            #         # print(x)
            #         # f.write(str(x)+" ")
            #         with lock:
            #             shareArray[indexArray.value] = x
            #         indexArray.value = indexArray.value + 1
            #     if indexArray.value % 10 != 0:
            #         indexArray.value = (int(indexArray.value/10)+1)*10
            #     if indexArray.value >= 1000:
            #         indexArray.value = 0
            #     # f.write("\n")
            pass
        else:
            print("write...")
            for x in CommandList[int(shareValue.value)-1]:
                print(to_bytes(x,2))
                command_bytes = to_bytes(x, 2)
                print(command_bytes)
                ser.write(command_bytes)
                print("done")
            shareValue.value = 0
    
    # f.close()

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

    p1 = Process(target=GetBlueData, args=(shareValue,shareArray,indexArray,lock,))
    p2 = Process(target=ShowBlueData, args=(shareValue,shareArray,lock,))
    p1.start()
    p2.start()
    process_list.append(p1)
    process_list.append(p2)
    while shareValue.value != -1:
        shareValue.value = int(input("shareValue.value = "))
    
    for p in process_list:
        p.join()
    
    print("end")
