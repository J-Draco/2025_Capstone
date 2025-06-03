#server_test.py
from flask import Flask,request,jsonify
import subprocess
import sqlite3
from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes

with open("server_private.pem", "rb") as key_file:
    private_key = serialization.load_pem_private_key(
        key_file.read(),
        password=None,
        backend=default_backend()
    )
def rsa_decrypt(encrypted_data: bytes) -> bytes:
    print("Received : ",encrypted_data)
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

def aes_decrypt(key: bytes, plaintext: bytes) -> bytes:
    cipher = Cipher(algorithms.AES(key), modes.ECB())
    decryptor = cipher.encryptor()
    return decryptor.update(plaintext) + decryptor.finalize()

def seperate(received):
    decrypted = rsa_decrypt(received)
    

    aes_key = decrypted[:32]             # 32 bytes
    encrypted_block = decrypted[32:80] # 80 bytes
    domain = decrypted[80:]# 나머지

    print("Decrypted : ", decrypted)
    print("aes_key : ", aes_key.hex().upper())
    print("encrypted_block : ", encrypted_block.hex().upper())
    print("domain : ", domain.decode('utf-8'))
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
with open("encrypted.bin","rb") as data:
    received = data.read()
    seperate(received)