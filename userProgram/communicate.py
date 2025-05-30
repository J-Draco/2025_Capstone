import socket
import time
def communicate():
    HOST = '127.0.0.1'
    PORT = 9999

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    server_socket.bind((HOST, PORT))
    server_socket.listen(1)

    print(f"MCU 데이터 대기 중... (포트 {PORT})")

    client_socket, addr = server_socket.accept()
    print(f"MCU 연결됨: {addr}")
    
    data = b''
    while len(data) < 80:
        packet = client_socket.recv(80 - len(data))
        if not packet:
            break  # 연결 끊김
        data += packet
    print(f"[DEBUG] 수신 전체 길이: {len(data)}")
    print(f"[DEBUG] 내용 (hex): {data.hex()}")

    client_socket.close()
    server_socket.close()

    return data  

def sendpw(password,auth):
    message = password.encode('utf-8')+bytes.fromhex(auth)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        
        sock.connect(("127.0.0.1", 8888))  
        sock.sendall(message)

def receive_result():
    HOST = '127.0.0.1'
    PORT = 7777

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((HOST, PORT))
        server.listen(1)
        conn, addr = server.accept()
        with conn:
            result = conn.recv(4)
            return result.decode('utf-8')
def send_req():
    HOST = '127.0.0.1'
    PORT = 6666  
    timestamp=int(time.time())
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((HOST, PORT))
        sock.sendall(b'R'+timestamp.to_bytes(4,'big')) 
        print("요청완료 6666");
    print("요청 소켓 종료됨")
