import socket
from threading import Thread

hostname = socket.gethostname()
node_name = "gyhpy1"
my_ip = "0.0.0.0"# socket.gethostbyname(hostname)
my_port = "8894"

s_ADDRESS = (my_ip, int(my_port))
c_ADDRESS = ('127.0.0.1', 8893)
g_socket_server = None
g_conn_pool = []

def ServerInit():
    global g_socket_server
    g_socket_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    g_socket_server.bind(s_ADDRESS)
    g_socket_server.listen(30)
    print('server start...')

def accept_client():
    print('wait connect...')
    while True:
        client, _ = g_socket_server.accept()
        g_conn_pool.append(client)
        t = Thread(target=message_handle, args=(client,))
        t.setDaemon(True)
        t.start()

def message_handle(client):
    client.sendall('connect success!'.encode(encoding = 'utf-8'))
    while True:
        bytes = client.recv(1024)
        print('message:', bytes.decode(encoding = 'utf-8'))
        if len(bytes) == 0:
            client.close()
            g_conn_pool.remove(client)
            print('remove client...')
            break

def CreateClient(s):
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(c_ADDRESS)
    client.sendall(s.encode())
    client.close()

def reg():
    global node_name, my_ip, my_port
    s = "1[name:" + node_name + ";ip:" + my_ip + ";port:" + my_port + ";]"
    CreateClient(s)

def sub(sub_name):
    global node_name
    s = "2[name:" + node_name + ";sub:" + sub_name + ";]"
    CreateClient(s)

def pub(pub_name):
    global node_name
    s = "3[name:" + node_name + ";pub:" + pub_name + ";]"
    CreateClient(s)

if __name__ == '__main__':
    ServerInit()
    t = Thread(target=accept_client)
    t.setDaemon(True)
    t.start()
    # reg()
    # sub("shit")
    # pub("shit2")
    while True:
        pass