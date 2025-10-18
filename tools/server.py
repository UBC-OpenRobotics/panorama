import socket      # Built-in module for network communication (TCP/IP)
import time        # For controlling how often data is sent
import random      # To generate random “fake” data valuesgit 

def generate_cluster(size=5):
    """
    Generate a list (cluster) of random floating-point numbers.
    This represents one "packet" of raw data coming from the ESB.

    Parameters:
        size (int): how many numbers are in each data cluster.
                    For example, size=5 might mean 5 sensor readings.

    Returns:
        list[float]: random numbers between 0 and 100, rounded to 2 decimals.
    """
    return [round(random.uniform(0, 100), 2) for _ in range(size)]


def start_mock_esb(host="127.0.0.1", port=5000, interval=1):
    """
    Start a fake ESB (Enterprise Service Bus) server that continuously sends
    clusters of fake data to any client (like your future C++ program).

    Parameters:
        host (str): the IP address to listen on. "127.0.0.1" means local machine.
        port (int): the port number where clients will connect.
        interval (int or float): seconds to wait between sending each data packet.
    """

    # Create a TCP socket (AF_INET = IPv4, SOCK_STREAM = TCP)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:

        # Allow immediate reuse of the port after the program closes
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        # Bind the socket to a host (IP) and port (e.g. localhost:5000)
        server.bind((host, port))

        # Listen for incoming connections — "1" means one queued client max
        server.listen(1)
        print(f"✅ Mock ESB started on {host}:{port}. Waiting for a client...")

        # Block here until a client connects (like your C++ script)
        conn, addr = server.accept()
        print(f"📡 Client connected from {addr}")

        # Once connected, we’ll send data continuously through this connection
        with conn:
            try:
                while True:
                    # Generate a random cluster of fake numeric data
                    cluster = generate_cluster()

                    # Convert the list [12.3, 45.6, 78.9] → "12.3 45.6 78.9\n"
                    # The newline ('\n') is important — it tells the receiver
                    # where one message ends and the next begins.
                    message = " ".join(map(str, cluster)) + "\n"

                    # Send the bytes over the network to the connected client
                    conn.sendall(message.encode("utf-8"))

                    # Log what we just sent to the console (for debugging)
                    print(f"Sent: {message.strip()}")

                    # Wait a bit before sending the next cluster
                    time.sleep(interval)

            except (BrokenPipeError, ConnectionResetError):
                # If the client disconnects or crashes, this handles it gracefully
                print("⚠️ Client disconnected.")

            finally:
                # Always close the connection when done
                conn.close()


# Run the mock ESB when this script is executed directly
if __name__ == "__main__":
    start_mock_esb()
