import sqlite3
import os

# secure 디렉토리 생성 (없는 경우)
secure_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "secure")
if not os.path.exists(secure_dir):
    os.makedirs(secure_dir)

# 데이터베이스 파일 경로 설정
db_path = os.path.join(secure_dir, "database.db")

# 데이터베이스 파일이 이미 존재하는지 확인
db_exists = os.path.exists(db_path)

# 데이터베이스 연결
conn = sqlite3.connect(db_path)
cursor = conn.cursor()

# 테이블 생성 (이미 존재하지 않는 경우)
cursor.execute('''
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    auth TEXT NOT NULL,
    auth_hash TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)
''')

# 샘플 사용자 추가 (데이터베이스가 새로 생성된 경우에만)
if not db_exists:
    print("새 데이터베이스 생성 중... 샘플 사용자 추가")
    
    # 샘플 사용자 데이터 (실제 사용 시 변경 필요)
    sample_uid = "A12345"
    sample_auth = "1234567890ABCDEF1234567890ABCDEF"  # 예시 인증 코드
    sample_auth_hash = "E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855"  # 빈 문자열의 SHA-256 해시 (예시)
    
    cursor.execute('''
    INSERT INTO users (uid, auth, auth_hash)
    VALUES (?, ?, ?)
    ''', (sample_uid, sample_auth, sample_auth_hash))
    
    print("샘플 사용자가 추가되었습니다.")
    print(f"UID: {sample_uid}")
    print(f"AUTH: {sample_auth}")
    print(f"AUTH_HASH: {sample_auth_hash}")
    print("\n주의: 이 샘플 데이터는 테스트 목적으로만 사용하세요. 실제 사용 전에 변경하세요.")

# 변경사항 저장 및 연결 종료
conn.commit()
conn.close()

print(f"데이터베이스 설정이 완료되었습니다. 경로: {db_path}") 