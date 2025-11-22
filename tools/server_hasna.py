#!/usr/bin/env python3
"""
esp32_emulator_tcp.py
---------------------

This script emulates an ESP32 device that sends sensor-like data (telemetry)
over a TCP connection. You can run it either as a SERVER (default) or as a CLIENT.

- In server mode: it waits for a connection, then sends telemetry data.
- In client mode: it connects to a remote server and sends telemetry data.

Usage examples:
  Server (listens for incoming connection):
    python esp32_emulator_tcp.py --host 0.0.0.0 --port 7000

  Client (connects to a server at a given IP):
    python esp32_emulator_tcp.py --client --host 192.168.1.5 --port 7000
"""

import socket      # Networking (TCP/UDP communication)
import time        # For delays and timestamps
import json        # To send structured data (JSON format)
import argparse    # To handle command-line arguments
import random      # To simulate random sensor readings


# -------------------------------------------------------------------
# Function: build_telemetry
# Purpose:  Generates a JSON string with fake ESP32-like sensor data
# -------------------------------------------------------------------
def build_telemetry(counter):
    # Create a dictionary representing sensor data
    """
    data = {
        "device": "esp32-emulator-tcp",       # name/type of the device
        "ts": int(time.time()),               # current timestamp (Unix time)
        "counter": counter,                   # packet counter, increments each send
        "temperature_c": round(20 + random.uniform(-2, 2), 2),  # random temp
        "humidity_pct": round(50 + random.uniform(-5, 5), 1)    # random humidity
    }
    """
    data = "Hello World"
    # Convert dictionary to JSON and add newline so each record is on its own line
    return json.dumps(data) + "\n"


# -------------------------------------------------------------------
# Function: server_mode
# Purpose:  Acts like a TCP server (the ESP32 emulator waits for a client)
# -------------------------------------------------------------------
def server_mode(host, port, interval):
    # Create a TCP socket using IPv4
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Bind socket to address and port
        s.bind((host, port))
        # Start listening for incoming connections (allow 1 queued connection)
        s.listen(1)
        print(f"Listening on {host}:{port} — waiting for client...")

        # Accept a connection (blocking until a client connects)
        conn, addr = s.accept()
        # Use "with" to ensure connection closes cleanly on exit
        with conn:
            print("Client connected:", addr)
            counter = 0
            try:
                # Continuous loop: generate and send telemetry every 'interval' seconds
                while True:
                    # Generate fake telemetry data
                    line = build_telemetry(counter).encode("utf-8")
                    # Send it over the socket
                    conn.sendall(line)
                    # Print what we sent to the console (for debugging)
                    print("->", line.decode().strip())
                    counter += 1
                    # Wait for the next transmission
                    time.sleep(interval)

            # Handle connection loss
            except BrokenPipeError:
                print("Client disconnected.")
            # Handle Ctrl+C (manual stop)
            except KeyboardInterrupt:
                print("Stopped by user.")


# -------------------------------------------------------------------
# Function: client_mode
# Purpose:  Acts like a TCP client (connects to a server and sends data)
# -------------------------------------------------------------------
def client_mode(host, port, interval):
    # Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        print(f"Connecting to {host}:{port} ...")
        # Connect to the specified server
        s.connect((host, port))
        print("Connected.")
        counter = 0
        try:
            # Same loop as server: generate and send telemetry repeatedly
            while True:
                line = build_telemetry(counter).encode("utf-8")
                s.sendall(line)
                print("->", line.decode().strip())
                counter += 1
                time.sleep(interval)

        except BrokenPipeError:
            print("Server disconnected.")
        except KeyboardInterrupt:
            print("Stopped by user.")


# -------------------------------------------------------------------
# Function: main
# Purpose:  Entry point — parses command-line arguments and decides mode
# -------------------------------------------------------------------
def main():
    # Create an argument parser for command-line options
    p = argparse.ArgumentParser(description="ESP32 TCP emulator")

    # Add arguments
    p.add_argument("--host", default="0.0.0.0",
                   help="Host IP to bind or connect to (default: 0.0.0.0 for server)")
    p.add_argument("--port", type=int, default=7000,
                   help="Port number to use (default: 7000)")
    p.add_argument("--interval", type=float, default=1.0,
                   help="Seconds between telemetry sends (default: 1.0)")
    p.add_argument("--client", action="store_true",
                   help="Run in client mode instead of server mode")

    # Parse command-line arguments
    args = p.parse_args()

    # Decide which mode to run based on --client flag
    if args.client:
        client_mode(args.host, args.port, args.interval)
    else:
        server_mode(args.host, args.port, args.interval)


# -------------------------------------------------------------------
# Standard Python entry point
# -------------------------------------------------------------------
if __name__ == "__main__":
    main()
