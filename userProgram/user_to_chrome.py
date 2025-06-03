# user_to_chrome.py
from flask import Flask, request,jsonify
import global_v
import threading
from flask_cors import CORS 
from rsa_crypt import  encrypt_with_rsa_public_key;
import hashlib
import requests
import socket
import time
app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}},supports_credentials=True)  # 모든 origin 허용
@app.route('/receive_domain', methods=['POST'])
def receive_domain():
    data = request.get_json()
    domain = data.get("domain")
    password = data.get("password")
    global_v.final_hash = ''

    domain_hash = hashlib.sha256(domain.encode('utf-8')).hexdigest().upper()
    password_hash = hashlib.sha256(password.encode('utf-8')).digest()

    global_v.domain = domain_hash

    print(f"[도메인 수신됨] {domain}")
    print(f"[도메인 해시됨] {global_v.domain}")
    print(f"[비밀번호 수신됨] {password}")
    print(f"[비밀번호 해시됨] {password_hash}")

    random_bytes = bytes.fromhex(global_v.random);
    encrypted_bytes = bytes.fromhex(global_v.encrypted)
    domain_bytes = global_v.domain.encode('utf-8')
    data_to_encrypt = random_bytes+encrypted_bytes+domain_bytes
    encrypted_result = encrypt_with_rsa_public_key(data_to_encrypt);
    print(f"현재 도메인 : {global_v.domain.encode('utf-8')}")
    with open("encrypted.bin","wb") as f:
        f.write(encrypt_with_rsa_public_key(data_to_encrypt))#출력테스트
        print(f"Encrypted :{encrypt_with_rsa_public_key(data_to_encrypt)}");
    if (global_v.running_trigger):
        try:
            response=requests.post("http://127.0.0.1:10001/auth",data=encrypted_result)
            result_dict = response.json()
            cipher_hex = result_dict["result"]
            cipher_bytes = bytes.fromhex(cipher_hex)

            print(f"중앙 서버 응답: {response.status_code} {response.text}")
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect(("127.0.0.1", 4444))
                sock.sendall(cipher_bytes+password_hash)
            timeout = 5.0  # 최대 3초 대기
            interval = 0.05  # 50ms 간격
            waited = 0
            while waited < timeout:
                if global_v.final_hash != '':
                    break
                time.sleep(interval)
                waited += interval

            if global_v.final_hash == "":
                print("[Flask] 최종 해시 수신 실패 (timeout)")
                return jsonify({"status": "timeout", "final_hash": None})
            else:
                print("[Flask] 최종 해시:", global_v.final_hash)
                return jsonify({"status": "ok", "final_hash": global_v.final_hash})
        except Exception as e:
            print(f"서버 전송 실패:{e}")
    else:
        print("Program is not running")

    return jsonify({"status": "ok", "final_hash": global_v.final_hash})

def start_server():
    app.run(port=5555, debug=False)
def receive_domain_thread():
    if global_v.domain_thread_started:
        return
    global_v.domain_thread_started = True

    thread = threading.Thread(target=start_server)
    thread.daemon=True
    thread.start()
def receive_final_hash_thread():
    if global_v.final_hash_thread_started:
        return
    global_v.final_hash_thread_started = True

    def update_final_hash():
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
            server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server.bind(('127.0.0.1', 5556))
            print("[수신 스레드] 5556 포트 대기 중")

            server.listen(1)
            while True:
                conn, _ = server.accept()
                print("[수신 스레드] 연결 수락됨")

                with conn:
                    final_hash = conn.recv(32)
                    global_v.final_hash = final_hash.hex().upper()
                    print("[사용자 프로그램] 최종 해시 수신:", global_v.final_hash)

    thread = threading.Thread(target=update_final_hash, daemon=True)
    thread.start()
