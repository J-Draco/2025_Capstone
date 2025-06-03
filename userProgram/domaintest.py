import global_v
from user_to_chrome import receive_domain_thread
receive_domain_thread()
while True:
    if global_v.domain:
        print(f"현재 도메인 : {global_v.domain}")
    else:
        print("[도메인 없음] 아직 도메인이 설정되지 않았습니다.")
