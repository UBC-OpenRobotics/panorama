import argparse
import json
import socket
import time


ESP_IP = "192.168.4.1"
PORT = 9000
RUN_SECONDS = 30
BUFFER_SIZE = 1024


def print_json_lines(payload):
    for line in payload.decode(errors="ignore").splitlines():
        line = line.strip()
        if not line or not line.startswith("{"):
            continue
        try:
            print("JSON:", json.loads(line))
        except json.JSONDecodeError:
            print("Bad JSON:", line)


def run_tcp():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((ESP_IP, PORT))
        print(f"Connected to {ESP_IP}:{PORT} over TCP")

        try:
            status = sock.recv(BUFFER_SIZE).decode().strip()
            if status:
                print("Status from ESP32:", status)
        except OSError:
            pass

        sock.sendall(b"MODE TCP\n")
        sock.sendall(b"START 5\n")

        start = time.time()
        try:
            while time.time() - start < RUN_SECONDS:
                payload = sock.recv(BUFFER_SIZE)
                if not payload:
                    print("TCP connection closed by ESP32")
                    break
                print_json_lines(payload)
        finally:
            try:
                sock.sendall(b"STOP\n")
                sock.sendall(b"END\n")
            except OSError:
                pass
            print("Sent STOP + END over TCP")


def run_udp():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.settimeout(2.0)
        print(f"Talking to {ESP_IP}:{PORT} over UDP")

        sock.sendto(b"MODE UDP\n", (ESP_IP, PORT))
        try:
            payload, addr = sock.recvfrom(BUFFER_SIZE)
            print(f"Status from ESP32 {addr}: {payload.decode().strip()}")
        except socket.timeout:
            print("No UDP MODE ack received")

        sock.sendto(b"START 5\n", (ESP_IP, PORT))

        start = time.time()
        try:
            while time.time() - start < RUN_SECONDS:
                try:
                    payload, addr = sock.recvfrom(BUFFER_SIZE)
                except socket.timeout:
                    continue
                print(f"Packet from {addr}")
                print_json_lines(payload)
        finally:
            sock.sendto(b"STOP\n", (ESP_IP, PORT))
            sock.sendto(b"END\n", (ESP_IP, PORT))
            print("Sent STOP + END over UDP")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--transport", choices=["tcp", "udp"], default="tcp")
    args = parser.parse_args()

    if args.transport == "udp":
        run_udp()
        return
    run_tcp()


if __name__ == "__main__":
    main()
