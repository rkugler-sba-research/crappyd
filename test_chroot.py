import socket
import sys

def retrieve_file(path):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect("/tmp/filebroker.sock")
    s.send(path + '\n')
    out = ''
    while True:
        data = s.recv(512)
        out += data
        if not data: break
    s.close()
    print('Received ' + repr(out))

p_file = sys.argv[1]

retrieve_file(p_file)

