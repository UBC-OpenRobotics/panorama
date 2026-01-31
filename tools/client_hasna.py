#!/usr/bin/env python3
"""
esp32_client.py
---------------
A simple TCP client that connects to the ESP32 emulator (server)
and prints telemetry data as it is received.

Usage:
  python esp32_client.py --host localhost --port 7000
"""

import socket
import argparse

def main():
    # Set up command-line arguments
    parser = argparse.ArgumentParser(description="Simple ESP32 telemetry client")
    parser.add_argument("--host", default="localhost", help="Server host to connect to")
    parser.add_argument("--port", type=int, default=7000, help="Server port to connect to")
    args = parser.parse_args()

    # Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        print(f"Connecting to {args.host}:{args.port} ...")
        s.connect((args.host, args.port))
        print("Connected. Listening for telemetry...\n")

        try:
            # Keep receiving data from the server
            buffer = b""
            while True:
                data = s.recv(1024)  # Read up to 1024 bytes
                if not data:
                    print("Connection closed by server.")
                    break
                buffer += data

                # Split incoming data by newline (each JSON record ends with \n)
                while b"\n" in buffer:
                    line, buffer = buffer.split(b"\n", 1)
                    print("<-", line.decode().strip())  # Print decoded JSON line

        except KeyboardInterrupt:
            print("\nStopped by user.")
        except ConnectionResetError:
            print("Server disconnected unexpectedly.")

if __name__ == "__main__":
    main()
