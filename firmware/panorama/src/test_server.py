import socket
import time
import json

ESP_IP = "192.168.4.1"  # ESP32 softAP IP (printed in Serial Monitor)
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

        # start JSON streaming at 5 Hz
        s.sendall(b"START 5\n")
        start = time.time()

        try:
            while time.time() - start < RUN_SECONDS:
                data = s.recv(1024)
                if not data:
                    print("Connection closed by ESP32")
                    break

                # ESP32 sends one JSON object per line; ignore debug text (e.g. "Sensor detected at GPIO 5")
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
            # tell ESP32 to stop streaming
            try:
                s.sendall(b"STOP\n")
            except OSError:
                pass
            print("Sent STOP")


if __name__ == "__main__":
    main()