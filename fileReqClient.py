import socket


FILEREQPORT = 1024
MAX_FILENAME = 128
MAX_FILESIZE = 4096


def minifyString(data: bytes) -> str:
    string = data.decode("utf-8")

    for i in range(len(string)):
        if string[i] == '\0':
            return string[0:i]


if __name__ == "__main__":
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # connect to our file request server
        sock.connect(("127.0.0.1", FILEREQPORT))

        # get our filename input
        filename = input("Request file >> ")

        # pad it (if necessary)
        filename += "\0" * (MAX_FILENAME - len(filename))

        # send it to the server
        sock.sendall(bytes(filename, encoding="utf-8"))

        # wait for it to respond
        data = sock.recv(MAX_FILESIZE)
        data = minifyString(data)

        # print our received data
        print("\n", data)
