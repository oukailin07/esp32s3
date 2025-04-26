import socket
import os
import zlib

# 服务器地址和端口
SERVER_IP = "192.168.0.103"
PORT = 1234

# 计算文件的 CRC32
def crc32(filepath):
    crc = 0
    with open(filepath, 'rb') as f:
        while chunk := f.read(1024):
            crc = zlib.crc32(chunk, crc)
    return crc & 0xFFFFFFFF

# 发送单个文件
def send_file(sock, base_dir, full_path):
    # 计算相对路径并添加 Emoji/ 根目录
    relative_path = os.path.relpath(full_path, base_dir).replace("\\", "/")
    relative_path = f"Emoji/{relative_path}"

    # 计算 CRC
    crc = crc32(full_path)
    print(f"Sending: {relative_path}, CRC: {crc:08X}")

    # 发送路径 + null 结尾
    sock.sendall(relative_path.encode('utf-8') + b'\x00')

    # 发送文件数据
    with open(full_path, 'rb') as f:
        while chunk := f.read(1024):
            sock.sendall(chunk)

    # 发送 CRC 校验信息
    sock.sendall(f"CRC:{crc:08X}".encode('utf-8'))

# 发送整个文件夹
def send_folder(folder_path):
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            full_path = os.path.join(root, file)
            with socket.create_connection((SERVER_IP, PORT)) as sock:
                send_file(sock, folder_path, full_path)

# 程序入口
if __name__ == '__main__':
    send_folder("Emoji")  # 传输 Emoji 文件夹下所有内容
