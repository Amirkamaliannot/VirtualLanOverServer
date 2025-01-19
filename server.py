import socket
import threading

# Function to handle client connections
def handle_client(client_socket, client_address):
    print(f"[+] New connection from {client_address}")
    try:
        while True:
            # Receive data from the client
            data = client_socket.recv(1024)
            if not data:
                break  # If no data is received, close the connection

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