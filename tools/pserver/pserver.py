#!/usr/bin/env python3
import socket
import sys
from putils import *
from pstreamer import PStreamer
from pstream_json import PStreamJSON

class PServer:
    def __init__(self):
        self.verbose = 0

    def send_stream(self, client_socket, client_address, streamer: PStreamer):
        """
        Send data from the streamer to the connected client.

        Args:
            client_socket: Socket connected to the client
            client_address: Address of the client
            streamer: PStreamer instance providing data
        """
        print(f"[PServer] Client connected from {client_address}")

        try:
            counter = 0
            while True:
                data = streamer.get_data(timeout=2.0)
                if data is None:
                    pwarning("PServer", "No data available from streamer")
                    continue
                
                client_socket.sendall(data)
                #pinfo("PServer", f"Sent: {data.decode('utf-8').strip()}")
                counter += 1

        except (BrokenPipeError, ConnectionResetError) as e:
            pinfo("PServer", f"Connection closed: {e}")
        except KeyboardInterrupt:
            pinfo("PServer", "Connection interrupted")
        finally:
            client_socket.close()

def main():
    host = '127.0.0.1'
    port = 4000

    if len(sys.argv) > 1:
        port = int(sys.argv[1])

    server = PServer()

    streamer = PStreamer()
    streamer.build_stream(PStreamJSON()).set_interval(0.4)

    streamer.start()

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        server_socket.bind((host, port))
        server_socket.listen(5)

        pinfo("PServer", f"PServer listening on {host}:{port}")
        pinfo("PServer", "Waiting for client connection...")
        pinfo("PServer", "Press Ctrl+C to stop")

        client_socket, client_address = server_socket.accept()
        server.send_stream(client_socket, client_address, streamer)

    except KeyboardInterrupt:
        pinfo("PServer", "Shutting down server...")
    finally:
        streamer.stop()
        server_socket.close()

if __name__ == "__main__":
    main()
