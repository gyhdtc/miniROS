import socket
HOST = '115.157.195.140'
PORT = 8888

myip = "192.168.137.1"
myport = "8888"

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

for i in range(100):
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)       #定义socket类型，网络通信，TCP
    s.connect((HOST,PORT))        #要连接的IP与端口
    cmd = "4" + "[name:winblue;pub:bluedata;" + str(i) + ",]"        #与人交互，输入命令
    s.sendall(cmd.encode())       #把命令发送给对端
    data = s.recv( 1024 )      #把接收的数据定义为变量
    print(data)          #输出变量
    s.close()    #关闭连接
