import socket
import threading
import SocketServer
import time
import struct

DATA = "a"*1024*1024*16

class ThroughputTestHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        print "Connected!"
        self.request.setblocking(0)
        timeout = 10

        while timeout > 0:
            now = time.time()
            try:
                self.request.send(DATA)
            except:
                pass
            timeout -= time.time() - now
            print timeout

        timeout = 15

        while timeout > 0:
            now = time.time()
            try:
                _ = self.request.recv(1024*1024*16)
            except:
                pass
            timeout -= time.time() - now
            print timeout

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass

def start_server():
    # Port 0 means to select an arbitrary unused port
    HOST, PORT = "0.0.0.0", 0

    server = ThreadedTCPServer((HOST, PORT), ThroughputTestHandler)
    server_thread = threading.Thread(target=server.serve_forever)

    # Exit the server thread when the main thread terminates
    server_thread.daemon = True
    server_thread.start()
    ip, port = server.server_address

    return ip, str(port)

if __name__ == "__main__":
    start_server()
    _ = raw_input()
