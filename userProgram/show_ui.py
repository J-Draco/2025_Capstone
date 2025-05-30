from importlist import QTimer, QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout, QLineEdit, Qt, QSpacerItem, QSizePolicy, CHECK_PATH
from communicate import communicate,sendpw,receive_result,send_req
import threading
import struct
import global_domain 
from rsa_crypt import  encrypt_with_rsa_public_key;
from user_to_chrome import receive_domain_thread;
class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Lockify")

        window_width, window_height = 400, 300
        self.resize(window_width, window_height)
        self.center()

        self.setWindowFlag(Qt.WindowStaysOnTopHint)
        self.raise_()
        self.activateWindow()

        main_layout = QVBoxLayout()
        main_layout.setAlignment(Qt.AlignCenter)

        input_layout = QHBoxLayout()
        input_layout.setAlignment(Qt.AlignCenter)

        self.password_input = QLineEdit()
        self.password_input.setEchoMode(QLineEdit.Password)
        self.password_input.setPlaceholderText("비밀번호 입력")
        self.password_input.setFixedWidth(250)
        self.password_input.setStyleSheet(self.default_input_style())
        self.password_input.returnPressed.connect(self.check_password)
        input_layout.addWidget(self.password_input)

        self.confirm_button = QPushButton("확인")
        self.confirm_button.setFixedHeight(self.password_input.sizeHint().height())
        self.confirm_button.setStyleSheet("""
            QPushButton {
                background-color: #0d6efd;
                color: white;
                border-radius: 8px;
                padding: 8px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #0b5ed7;
            }
        """)
        self.confirm_button.clicked.connect(self.check_password)
        input_layout.addWidget(self.confirm_button)

        self.error_label = QLabel("")
        self.error_label.setStyleSheet("color: red; font-size: 12px;")
        self.error_label.setAlignment(Qt.AlignLeft)
        self.error_label.setFixedHeight(20)

        self.result_label = QLabel("")
        self.result_label.setStyleSheet("font-size: 12px; color: black;")
        self.result_label.setAlignment(Qt.AlignCenter)

        self.countdown_label = QLabel("")
        self.countdown_label.setStyleSheet("font-size: 16px; color: blue;")
        self.countdown_label.setAlignment(Qt.AlignCenter)
        self.countdown_label.hide()

        center_layout = QVBoxLayout()
        center_layout.setAlignment(Qt.AlignCenter)
        center_layout.addLayout(input_layout)
        center_layout.addWidget(self.error_label)
        center_layout.addWidget(self.result_label)
        center_layout.addWidget(self.countdown_label)

        main_layout.addSpacerItem(QSpacerItem(0, 0, QSizePolicy.Minimum, QSizePolicy.Expanding))
        main_layout.addLayout(center_layout)
        main_layout.addSpacerItem(QSpacerItem(0, 0, QSizePolicy.Minimum, QSizePolicy.Expanding))

        self.setLayout(main_layout)

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_countdown)
        self.remaining_seconds = 3
        self.communication_started = False
        self.start_button = QPushButton("확인")
        self.start_button.setFixedHeight(self.password_input.sizeHint().height())
        self.start_button.setStyleSheet("""
            QPushButton {
                background-color: #0d6efd;
                color: white;
                border-radius: 8px;
                padding: 8px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #0b5ed7;
            }
        """)
        self.start_button.clicked.connect(self.start_communication)
        
    def default_input_style(self):
        return """
            QLineEdit {
                border: 2px solid #0d6efd;
                border-radius: 8px;
                padding: 6px;
                font-size: 14px;
            }
        """

    def error_input_style(self):
        return """
            QLineEdit {
                border: 2px solid red;
                border-radius: 8px;
                padding: 6px;
                font-size: 14px;
            }
        """

    def center(self):
        screen = self.screen().geometry()
        size = self.geometry()
        self.move(
            int((screen.width() - size.width()) / 2),
            int((screen.height() - size.height()) / 2)
        )

    def check_password(self):
        entered_pw = self.password_input.text()
        correct_pw = self.get_password_from_file()
        try:
            sendpw(entered_pw,correct_pw)  
            #self.result_label.setText("비밀번호 전송 완료! MCU 응답 대기 중...")
            
            result = receive_result()

            if result == "OK":
                self.password_input.hide()
                self.confirm_button.hide()
                self.error_label.hide()
                self.result_label.setText("인증 완료! MCU 데이터 수신 대기 중...")
                
                self.start_communication()
            else:
                self.password_input.clear()
                self.password_input.setStyleSheet(self.error_input_style())
                self.error_label.setText("비밀번호 또는 인증파일을 다시 확인 해주세요.")
            
        except Exception as e:
            self.error_label.setText(f"모듈을 실행 하여주세요.")

        '''if entered_pw == correct_pw:
            self.password_input.hide()
            self.confirm_button.hide()
            self.error_label.hide()
            self.result_label.setText("인증 완료! MCU 데이터 수신 대기 중...")
            self.start_communication()
        else:
            self.password_input.clear()
            self.password_input.setStyleSheet(self.error_input_style())
            self.error_label.setText("비밀번호를 다시 입력하세요")'''

    def start_communication(self):
        self.start_button.setEnabled(False)
        receive_domain_thread();
        def run_communicate_loop():
            first_received = False
            while True:
                
                try:
                    if global_domain.domain:
                        print(f"현재 도메인 : {global_domain.domain}")
                    data = communicate()
                    if len(data) != 80:
                        self.safe_update(self.result_label, "수신 데이터 길이 오류")
                        continue

                    #hashed_part = data[:32]
                    random_part = data[:32]
                    encrypted_part = data[32:]
                    #print(f"len(hashed_part) = {len(hashed_part)}")
                    print(f"len(random_part) = {len(random_part)}")
                    print(f"len(encrypted_part) = {len(encrypted_part)}")

                    #uints = struct.unpack('<8I', data)
                    #hashed_unit =  struct.unpack('>8I',hashed_part);
                    random_unit =  struct.unpack('>8I',random_part);
                    encrypted_unit =  struct.unpack('>12I',encrypted_part);

                    #hashed_hex = ''.join(f'{u:08x}' for u in hashed_unit)
                    random_hex = ''.join(f'{u:08X}' for u in random_unit)
                    encrypted_hex = ''.join(f'{u:08X}' for u in encrypted_unit)
                    #hex_str = ''.join(f'{u:08x}' for u in uints)

                    if not first_received:
                        first_received = True
                        #self.safe_update(self.result_label, f"Hashed UID:{hashed_hex}\nRandom Key:{random_hex}\nEncrypted_UID:{encrypted_hex}\n")
                        self.safe_update(self.result_label, f"Random Key:{random_hex}\nEncrypted_Auth:{encrypted_hex}\n")
                        data_to_encrypt = (random_hex + encrypted_hex).encode('utf-8')
                        with open("encrypted.bin","wb") as f:
                            f.write(encrypt_with_rsa_public_key(data_to_encrypt))#출력테스트
                        print(f"Encrypted :{encrypt_with_rsa_public_key(data_to_encrypt)}");

                        #self.safe_update(self.result_label, f"KEY:{hex_str}")
                        self.safe_start_countdown()
                    else:
                        #self.safe_update(self.result_label, f"KEY:{hex_str}")
                        #self.safe_update(self.result_label, f"Hashed UID:{hashed_hex}\nRandom Key:{random_hex}\nEncrypted_UID:{encrypted_hex}\n")
                        self.safe_update(self.result_label, f"Random Key:{random_hex}\nEncrypted_Auth:{encrypted_hex}\n")


                except Exception as e:
                    self.safe_update(self.result_label, f"MCU 통신qqq 실패: {e}")
                    break

        thread = threading.Thread(target=run_communicate_loop)
        thread.daemon = True
        thread.start()
        send_req()

    def safe_update(self, label, text):
        self.result_label.setText(text)


    def safe_hide_inputs(self):
        QTimer.singleShot(0, self.start_countdown)

    def hide_inputs(self):
        self.password_input.hide()
        self.confirm_button.hide()
        self.error_label.hide()

    def safe_start_countdown(self):
        QTimer.singleShot(0, self.start_countdown)

    def start_countdown(self):
        self.remaining_seconds = 61
        minutes = self.remaining_seconds // 60
        seconds = self.remaining_seconds % 60
        self.countdown_label.setText(f"남은 시간: {minutes:02}:{seconds:02}")
        self.countdown_label.show()
        self.timer.start(1000)


    def update_countdown(self):
        self.remaining_seconds -= 1
        minutes = self.remaining_seconds // 60
        seconds = self.remaining_seconds % 60
        self.countdown_label.setText(f"남은 시간: {minutes:02}:{seconds:02}")
        if self.remaining_seconds <= 0:
            self.remaining_seconds = 61
            print("EXP TIME 만료 재요청")
            send_req()

    def get_password_from_file(self):
        try:
            with open(CHECK_PATH, 'r') as file:
                pw = file.read().strip()
            return pw
        except Exception:
            return ""
