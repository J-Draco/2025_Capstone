# user_to_chrome.py
from flask import Flask, request,jsonify
import global_domain
import threading
from flask_cors import CORS 
app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}},supports_credentials=True)  # 모든 origin 허용
@app.route('/receive_domain', methods=['POST'])
def receive_domain():
    data = request.get_json()
    global_domain.domain = data.get("domain")
    print(f"[도메인 수신됨] {global_domain.domain}")

    return jsonify({"status": "ok"})

def start_server():
    app.run(port=5000, debug=False)
def receive_domain_thread():
    thread = threading.Thread(target=start_server)
    thread.daemon=True
    thread.start()