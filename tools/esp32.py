#!/usr/bin/env python3
import socket
import threading
import sys

def handle_client(client_socket, client_address):
    print(f"Client connected from {client_address}")
    
    try:
        while True:
            msg = input("Enter message to send (or ':q' to disconnect): ") # Read input from console
            if msg.lower() == ':q':
                break
            
            client_socket.sendall((msg + '\n').encode('utf-8'))
            print(f"Sent: {msg}")
            
    except (BrokenPipeError, ConnectionResetError, KeyboardInterrupt):
        print("Connection closed")
    finally:
        client_socket.close()

def main():
    host = '127.0.0.1'
    port = 3000
    
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server_socket.bind((host, port))
        server_socket.listen(5)
        print(f"Server listening on {host}:{port}")
        print("Waiting for client connection...")
        
        client_socket, client_address = server_socket.accept()
        handle_client(client_socket, client_address)
        
    except KeyboardInterrupt:
        print("\nShutting down server...")
    finally:
        server_socket.close()

if __name__ == "__main__":
    main()