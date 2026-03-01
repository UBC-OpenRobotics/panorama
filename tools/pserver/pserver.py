#!/usr/bin/env python3
import socket
import sys
import threading
from putils import *
from pstreamer import PStreamer
from pstream_json import PStreamJSON

class PServer:
    def __init__(self):
        self.verbose = 0
        self._streaming = False

    def _read_commands(self, client_socket, stop_event):
        """Note: This runs in a separate thread"""
        buffer = ""
        try:
            while not stop_event.is_set():
                try:
                    data = client_socket.recv(1024)
                except OSError:
                    break
                if not data:
                    break
                buffer += data.decode(errors="replace")
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    line = line.strip().upper()
                    if not line:
                        continue
                    if line.startswith("START"):
                        pinfo("PServer", "Received START command")
                        self._streaming = True
                    elif line.startswith("STOP"):
                        pinfo("PServer", "Received STOP command")
                        self._streaming = False
                    else:
                        pinfo("PServer", f"Unknown command: {line}")
        except Exception as e:
            pinfo("PServer", f"Command reader error: {e}")
        finally:
            stop_event.set()

    def send_stream(self, client_socket, client_address, streamer: PStreamer):
        """
        Send data from the streamer to the connected client.
        Waits for a START command before sending data.

        Args:
            client_socket: Socket connected to the client
            client_address: Address of the client
            streamer: PStreamer instance providing data
        """
        print(f"[PServer] Client connected from {client_address}")
        pinfo("PServer", "Waiting for START command...")

        stop_event = threading.Event()
        cmd_thread = threading.Thread(
            target=self._read_commands,
            args=(client_socket, stop_event),
            daemon=True
        )
        cmd_thread.start()

        try:
            counter = 0
            while not stop_event.is_set():
                if not self._streaming:
                    stop_event.wait(timeout=0.1)
                    continue

                data = streamer.get_data(timeout=2.0)
                if data is None:
                    pwarning("PServer", "No data available from streamer")
                    continue

                client_socket.sendall(data)
                counter += 1

        except (BrokenPipeError, ConnectionResetError) as e:
            pinfo("PServer", f"Connection closed: {e}")
        except KeyboardInterrupt:
            pinfo("PServer", "Connection interrupted")
        finally:
            stop_event.set()
            client_socket.close()

def main():
    host = '127.0.0.1'
    port = 3000

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
