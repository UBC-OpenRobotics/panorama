import requests
import numpy as np
import matplotlib.pyplot as plt

def noisy_data(start_val, end_val, num_points, noise_standard_deviation):
    time_steps = np.arange(num_points)
    true_signal = np.linspace(start_val, end_val, num_points)

    np.random.seed(42)
    noise = np.random.normal(loc=0.0, scale = noise_standard_deviation, size = num_points)

    noisy_data = true_signal + noise

    return time_steps, true_signal, noisy_data

time, true_signal, noisy_graph = noisy_data(0, 50, 100, 2)

# plt.figure(figsize = (12,6))

# plt.scatter(time, noisy_graph, label = 'Noisy Measurements', alpha = 0.6, s = 15, color = 'gray')

# plt.title('Noisy Data - to be denoised')
# plt.xlabel('Time Step ($k$)')
# plt.ylabel('Value')
# plt.legend()
# plt.grid(True)
# plt.show() 

response = requests.get("http127.0.0.1:8080")

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
