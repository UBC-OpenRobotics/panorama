import socket
import time
import json

ESP_IP = "192.168.4.1"  # ESP32 softAP IP
PORT = 9000
RUN_SECONDS = 30        # how long to receive data


def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((ESP_IP, PORT))
        print(f"Connected to {ESP_IP}:{PORT}")

        # read optional status line from ESP32
        try:
            status = s.recv(1024).decode().strip()
            if status:
                print("Status from ESP32:", status)
        except OSError:
            pass

        # select transport and start JSON streaming at 5 Hz
        s.sendall(b"MODE TCP\n")
        s.sendall(b"START 5\n")
        start = time.time()

        try:
            while time.time() - start < RUN_SECONDS:
                data = s.recv(1024)
                if not data:
                    print("Connection closed by ESP32")
                    break

                # ESP32 sends one JSON object per line
                for line in data.decode().splitlines():
                    line = line.strip()
                    if not line or not line.startswith("{"):
                        continue  # skip non-JSON lines
                    try:
                        msg = json.loads(line)
                        print("JSON:", msg)
                    except json.JSONDecodeError:
                        print("Bad JSON:", line)
        finally:
            # tell ESP32 to stop streaming and end session
            try:
                s.sendall(b"STOP\n")
                s.sendall(b"END\n")
            except OSError:
                pass
            print("Sent STOP + END")


if __name__ == "__main__":
    main()
