import socket
import sys

HOST = "127.0.0.1"
PORT = 8080

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect((HOST, PORT))
    print("Connected to server!")

    commands = sys.argv[1:]

    while True:
        if commands:
            query = commands.pop(0)
        else:
            try:
                query = input("redis> ").strip()
            except EOFError:
                print()
                break

        if not query:
            continue

        if query.lower() in {"quit", "exit"}:
            break

        client.sendall(query.encode("utf-8"))

        try:
            response = client.recv(4096)
        except ConnectionResetError:
            print("Server closed the connection.")
            break

        if not response:
            print("Server closed the connection.")
            break

        print("Server:", response.decode("utf-8"))