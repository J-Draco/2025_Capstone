from flask import Flask,request,jsonify
import subprocess
import sqlite3
from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.backends import default_backend
app = Flask(__name__)
with open("keys/server_private.pem", "rb") as key_file:
    private_key = serialization.load_pem_private_key(
        key_file.read(),
        password=None,
        backend=default_backend()
    )
def rsa_decrypt(encrypted_data: bytes) -> bytes:
    return private_key.decrypt(
        encrypted_data,
        padding.OAEP(
            mgf=padding.MGF1(algorithm=hashes.SHA256()),
            algorithm=hashes.SHA256(),
            label=None
        )
    )
def aes_decrypt(input_hex: str) -> str:
    result = subprocess.run(
        ["./crypto/aes_decrypt_exec", input_hex],
        capture_output=True,
        text=True
    )
    return result.stdout.strip()
def user_authorize(uid:str,auth:str) ->bool:
    conn = sqlite3.connect("database.db")
    cursor = conn.cursor()
    cursor.execute("SELECT 1 FROM users WHERE uid=? AND auth_hash=?", (uid, auth_hash))
    result = cursor.fetchone()
    conn.close()
    if result:
        return result
    else:
        return None
@app.route("/auth", methods=["POST"])
