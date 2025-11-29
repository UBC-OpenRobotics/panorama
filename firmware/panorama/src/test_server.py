import socket, time

ESP_IP = "192.168.4.1"
PORT = 9000

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((ESP_IP, PORT))
    print(s.recv(1024).decode())
    # signal ON at 5 Hz
    s.sendall(b"START 5\n")
    start = time.time()
    while time.time() - start < 5:  # run for 5 sec
        data = s.recv(1024)
        if data:
            print(data.decode().strip())
    # stop
    s.sendall(b"STOP\n")
