import socket
import signal
import sys

#This is included in case we are using wsl and there is no direct inet access

# Signal handler to ensure the server closes the socket properly
def signal_handler(sig, frame):
    print('Interrupt received, shutting down...')
    if server_socket:
        server_socket.close()
    sys.exit(0)

# Register the signal handler
signal.signal(signal.SIGINT, signal_handler)

def start_server(host='localhost', port=12345):
    global server_socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((host, port))
        server_socket.listen()
        print(f"Server started at {host}:{port}")
        
        while True:
            client_socket, client_address = server_socket.accept()
            with client_socket:
                print(f"Connection from {client_address}")
                data = client_socket.recv(1024).strip()
                if data:
                    clean_data = data.decode().replace('\r', '').replace('\n', '')
                    print(f"Received data: {clean_data}")
                    with open("logfile.txt", "a") as f:
                        f.write(f"{client_address} wrote: {clean_data}\n")
                client_socket.sendall(b"Data received")

if __name__ == "__main__":
    start_server()
