import socket
import threading

# class Client:
#     def __init__(ip, socket):
#         self.ip = ip
#         self.socket = socket




lobby_list = {}


def is_valid_ipv4(ip: str) -> bool:
    # Split the string by dots
    parts = ip.split(".")

    # IPv4 must have exactly 4 parts
    if (len(parts) != 4  or parts[0] != "10"):
        return False

    for part in parts:
        # Each part must be a digit and have a valid length
        if not part.isdigit() or not 0 <= len(part) <= 3:
            return False

        # Convert the part to an integer and check the range
        num = int(part)
        if not 0 <= num <= 255:
            return False
    return True



def handle_command(command, socket):
    parsed = command.split()
    if(parsed[1] == "join"):

        if(is_valid_ipv4(parsed[2]) and parsed[2] not in lobby_list):

            lobby_list[parsed[2]] = socket
            socket.send(b"1")
        else:
            socket.send(b"0")
    
    if(parsed[1] == "getList"):

        if(is_valid_ipv4(parsed[2]) and parsed[2] in lobby_list):
            a = ""
            for i in lobby_list:
                a+= i + "\n"
            socket.send(a.encode())

    if(parsed[1] == "leave"):

        if(is_valid_ipv4(parsed[2]) and parsed[2] in lobby_list):

            del lobby_list[parsed[2]]
            socket.send(b"1")






# Function to handle client connections
def handle_client(client_socket, client_address):
    print(f"[+] New connection from {client_address}")
    try:
        while True:
            # Receive data from the client
            data = client_socket.recv(1024)
            if not data:
                break  # If no data is received, close the connection

            if(data.decode('utf-8').startswith("command")):
                handle_command(data.decode('utf-8'), client_socket)
            print(f"Received from {client_address}: {data.decode('utf-8')}")

            # Send a response back to the client
            client_socket.send(b"Message received by server!")
    except Exception as e:
        print(f"Error handling client {client_address}: {e}")
    finally:
        # Close the client connection
        client_socket.close()
        print(f"[-] Connection from {client_address} closed")




# Function to start the server
def start_server(host='0.0.0.0', port=5001):
    # Create a TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(5)
    print(f"[*] Server started on {host}:{port}. Waiting for connections...")

    try:
        while True:
            # Accept a new client connection
            client_socket, client_address = server_socket.accept()

            # Start a new thread to handle the client
            client_thread = threading.Thread(target=handle_client, args=(client_socket, client_address))
            client_thread.start()

            print(f"[+] Active connections: {threading.active_count() - 1}")
    except KeyboardInterrupt:
        print("\n[*] Server is shutting down...")
    finally:
        server_socket.close()
        print("[*] Server closed.")

if __name__ == "__main__":
    start_server()