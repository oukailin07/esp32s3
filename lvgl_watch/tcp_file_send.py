#这份代码是通过tcp将文件发送给服务端
import socket
import zlib
import os

def crc32(filepath):
    crc = 0
    with open(filepath, 'rb') as f:
        while chunk := f.read(1024):
            crc = zlib.crc32(chunk, crc)
    return crc & 0xFFFFFFFF

def send_file(ip, port, filepath):
    filename = os.path.basename(filepath)
    crc = crc32(filepath)
    print(f"CRC: {crc:08X}")

    with socket.create_connection((ip, port)) as sock:
        sock.sendall(filename.encode() + b'\x00')  # Null-terminated
        with open(filepath, 'rb') as f:
            while chunk := f.read(1024):
                sock.sendall(chunk)
        sock.sendall(f"CRC:{crc:08X}".encode())

if __name__ == '__main__':
    send_file("192.168.0.103", 1234, "disdain2.gif")
