#server.py
from flask import Flask,request,jsonify
import subprocess
import sqlite3
from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import hashlib
import time

# RSA 키 로드
with open("server_private.pem", "rb") as key_file:
    private_key = serialization.load_pem_private_key(
        key_file.read(),
        password=None,
        backend=default_backend()
    )

def rsa_decrypt(encrypted_data: bytes) -> bytes:
    print("Received : ",encrypted_data.hex().upper)
    return private_key.decrypt(
        encrypted_data,
        padding.OAEP(
            mgf=padding.MGF1(algorithm=hashes.SHA256()),
            algorithm=hashes.SHA256(),
            label=None
        )
    )
def aes_decrypt(key: bytes, ciphertext: bytes) -> bytes:
    cipher = Cipher(algorithms.AES(key), modes.ECB())
    decryptor = cipher.decryptor()
    return decryptor.update(ciphertext) + decryptor.finalize()

def aes_encrypt(key: bytes, plaintext: bytes) -> bytes:
    cipher = Cipher(algorithms.AES(key), modes.ECB())
    decryptor = cipher.encryptor()
    return decryptor.update(plaintext) + decryptor.finalize()
    
    
def user_authorize(uid: str, auth_hash: str) -> bool:
    conn = sqlite3.connect("database.db")
    cursor = conn.cursor()
    cursor.execute("SELECT 1 FROM users WHERE uid=? AND auth_hash=?", (uid, auth_hash))
    result = cursor.fetchone()
    conn.close()
    return bool(result)

def hash_sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest().upper()

def auth_handler(received):
        decrypted = rsa_decrypt(received)

        aes_key = decrypted[:32]  # 32 bytes
        encrypted_block = decrypted[32:80]  # 다음 80 bytes
        domain = decrypted[80:]# 나머지

        print("Decrypted : ", decrypted.hex().upper())
        print("aes_key : ", aes_key.hex().upper())
        print("encrypted_block : ", encrypted_block.hex().upper())
        print("domain : ", domain.decode('utf-8'))
        # AES 복호화 (16바이트씩 3블록 = 48바이트)
        decrypted_block = b""
        for i in range(0, 48, 16):
            block = encrypted_block[i:i+16]
            decrypted_block += aes_decrypt(aes_key, block)
        print("Decrypted Auth Block : ", decrypted_block.hex().upper())

        auth=decrypted_block[:32]
        print("AUTH : ",auth.hex().upper())
        timestamp_bytes = decrypted_block[32:36]  # 마지막 블록의 앞 4바이트
        timestamp = int.from_bytes(timestamp_bytes, byteorder='big')  # big endian
        print("Timestamp:", timestamp)
        if int(time.time()) > timestamp:
            print("Expired")

        # DB에서 uid 조회
        conn = sqlite3.connect("database.db")
        cursor = conn.cursor()
        cursor.execute("SELECT uid FROM users WHERE LOWER(auth) = ?", (auth.hex().lower(),))
        row = cursor.fetchone()
        conn.close()

        if not row:
            print("No Uid Matched")
        uid = row[0]
        print("Hahed UID :", uid)
        combined = uid.encode('utf-8') + domain
        salt = hash_sha256(combined)
        print("SALT :",salt)
        salt_bytes = hashlib.sha256(combined).digest()  # 바로 bytes로 얻음

        encrypted_salt = b""
        for i in range(0, 32, 16):
            text_block = salt_bytes[i:i+16]
            encrypted_salt += aes_encrypt(aes_key, text_block )
        print("Encrypted SALT :",encrypted_salt.hex().upper())


with open("encrypted.bin","rb") as data:
    received = data.read()
    auth_handler(received)