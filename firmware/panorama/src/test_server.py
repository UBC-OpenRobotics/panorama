import socket
import sys
import json
import threading

ESP_IP = "192.168.4.1"  # ESP32 softAP IP (printed in Serial Monitor)
PORT = 9000

# Commands the ESP32 understands (sent as one line, newline-terminated):
#   START [freq]   - start streaming at freq Hz (default 5); e.g. "START 10"
#   STOP           - stop streaming
# Type QUIT or EXIT to close the connection and exit.


def read_commands(sock, stop_event):
    """Read lines from stdin and send them as commands to the ESP32."""
    try:
        while not stop_event.is_set():
            line = sys.stdin.readline()
            if not line:
                break
            line = line.strip()
            if not line:
                continue
            if line.upper() in ("QUIT", "EXIT", "Q"):
                stop_event.set()
                break
            # Send command to ESP32 (one line, newline-terminated)
            try:
                sock.sendall((line + "\n").encode())
                print(f"[sent] {line}")
            except OSError as e:
                print(f"[error] send failed: {e}", file=sys.stderr)
                stop_event.set()
                break
    except (KeyboardInterrupt, EOFError):
        stop_event.set()


def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((ESP_IP, PORT))
        print(f"Connected to {ESP_IP}:{PORT}")
        print("Send commands: START [freq], STOP. Type QUIT to exit.\n")

        # Read optional status line from ESP32
        s.settimeout(0.5)
        try:
            status = s.recv(1024).decode().strip()
            if status:
                print("Status from ESP32:", status)
        except socket.timeout:
            pass
        except OSError:
            pass
        s.settimeout(None)

        stop_event = threading.Event()
        cmd_thread = threading.Thread(target=read_commands, args=(s, stop_event), daemon=True)
        cmd_thread.start()

        try:
            buffer = ""
            while not stop_event.is_set():
                try:
                    data = s.recv(1024)
                except (OSError, socket.timeout):
                    continue
                if not data:
                    print("Connection closed by ESP32")
                    break

                buffer += data.decode(errors="replace")
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    line = line.strip()
                    if not line:
                        continue
                    if not line.startswith("{"):
                        # Status or debug line
                        print("ESP32:", line)
                        continue
                    try:
                        msg = json.loads(line)
                        print("JSON:", msg)
                    except json.JSONDecodeError:
                        print("Bad JSON:", line)
        finally:
            stop_event.set()
            try:
                s.sendall(b"STOP\n")
            except OSError:
                pass
            print("Sent STOP; exiting.")


if __name__ == "__main__":
    main()
