import socket
from threading import Thread
s_ADDRESS = ('127.0.0.1', 8712)
c_ADDRESS = ('192.168.137.142', 8888)
g_socket_server = None
g_conn_pool = []
def ServerInit():
    global g_socket_server
    g_socket_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    g_socket_server.bind(s_ADDRESS)
    g_socket_server.listen(30)
    print('server start...')
def accept_client():
    while True:
        client, _ = g_socket_server.accept()
        g_conn_pool.append(client)
        t = Thread(target=message_handle, args=(client,))
        t.setDaemon(True)
        t.start()
def message_handle(client):
    # client.sendall('connect success!'.encode(encoding = 'utf-8'))
    while True:
        bytes = client.recv(1024)
        print('client message:', bytes.decode(encoding = 'utf-8'))
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
if __name__ == '__main__':
    ServerInit()
    t = Thread(target=accept_client)
    t.setDaemon(True)
    t.start()
    CreateClient("1[name:gyh4;ip:0.0.0.0;port:8888;]")
    while True:

        pass