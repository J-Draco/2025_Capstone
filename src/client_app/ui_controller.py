from importlist import QTimer, QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout, QLineEdit, Qt, QSpacerItem, QSizePolicy, CHECK_PATH,QSpinBox,QDialog,QIcon,QSize
from mcu_communication import communicate,sendpw,receive_result,send_req
import threading
import struct
import global_variables 
from browser_communication import receive_domain_thread,receive_final_hash_thread;
from functools import partial

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
        self.result_label.setFixedSize(300, 40)
        self.result_label.setWordWrap(True)

        self.result_label.setStyleSheet("font-size: 12px; color: black;")
        self.result_label.setAlignment(Qt.AlignCenter)

        self.countdown_label = QLabel("")
        self.countdown_label.setStyleSheet("font-size: 16px; color: blue;")
        self.countdown_label.setAlignment(Qt.AlignCenter)
        self.countdown_label.setStyleSheet("""
    font-size: 18px;
    color: #0d6efd;
    font-weight: bold;
""")

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
        self.timer_trigger = 1;
        self.default_timer = 60;
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
        self.start_button = QPushButton("시작")
        self.start_button.clicked.connect(self.start_communication)

        self.stop_button = QPushButton()
        self.stop_button.setIcon(QIcon("icon/stop.png"))
        self.stop_button.setFlat(True)
        self.stop_button.setIconSize(QSize(32,32))
        self.stop_button.setToolTip("중단")
        self.stop_button.setEnabled(False)
        self.stop_button.clicked.connect(self.stop_communication)

        self.refresh_button = QPushButton()
        self.refresh_button.setIcon(QIcon("icon/refresh.png"))
        self.refresh_button.setFlat(True)
        self.refresh_button.setIconSize((QSize(32,32)))
        self.refresh_button.setToolTip("새로고침")
        self.refresh_button.setEnabled(False)
        self.refresh_button.clicked.connect(self.refresh)

        self.set_expire_button = QPushButton()
        self.set_expire_button.setIcon(QIcon("icon/settings.png"))
        self.set_expire_button.setFlat(True)
        self.set_expire_button.setIconSize(QSize(32,32))
        self.set_expire_button.setToolTip("만료시간 설정")
        self.set_expire_button.setEnabled(False)
        self.set_expire_button.clicked.connect(self.open_timer_setting)

        button_layout = QHBoxLayout()
        button_layout.setAlignment(Qt.AlignCenter)
        #button_layout.addWidget(self.start_button)
        button_layout.addWidget(self.stop_button)
        button_layout.addWidget(self.refresh_button)
        button_layout.addWidget(self.set_expire_button)

        self.refresh_button.hide()
        self.start_button.hide()
        self.set_expire_button.hide()
        self.stop_button.hide()

        main_layout.addLayout(button_layout)

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

                self.refresh_button.show()
                #self.start_button.show()
                self.set_expire_button.show()
                self.stop_button.show()

                self.result_label.setText("인증 완료! MCU 데이터 수신 대기 중...")
                
                self.start_communication()
            else:
                self.password_input.clear()
                self.password_input.setStyleSheet(self.error_input_style())
                self.error_label.setText("비밀번호 또는 인증파일을 다시 확인 해주세요.")
            
        except Exception as e:
            self.error_label.setText(f"모듈을 실행 하여주세요.")
            #import traceback
            #traceback.print_exc()  # 터미널에 전체 예외 출력
            #self.error_label.setText(f"오류 발생: {str(e)}")
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
        def format_hex_multiline(hex_str, line_length=32):
            return '\n'.join(hex_str[i:i+line_length] for i in range(0, len(hex_str), line_length))
        self.stop_button.setEnabled(True)
        self.refresh_button.setEnabled(True)
        self.set_expire_button.setEnabled(True)
        receive_final_hash_thread();
        receive_domain_thread();
        def run_communicate_loop():
            first_received = False
            while True:
                try:
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
                    global_variables.random=random_hex;
                    global_variables.encrypted=encrypted_hex

                    #hex_str = ''.join(f'{u:08x}' for u in uints)

                    if not first_received:
                        first_received = True
                        #self.safe_update(self.result_label, f"Hashed UID:{hashed_hex}\nRandom Key:{random_hex}\nEncrypted_UID:{encrypted_hex}\n")
                        #self.safe_update(self.result_label, f"Random Key:{random_hex}\nEncrypted_Auth:{encrypted_hex}\n")
                        formatted_hex = format_hex_multiline(random_hex)
                        self.safe_update(self.result_label, formatted_hex)
                        #self.safe_update(self.result_label, f"KEY:{hex_str}")
                        self.safe_start_countdown()
                    else:
                        #self.safe_update(self.result_label, f"KEY:{hex_str}")
                        #self.safe_update(self.result_label, f"Hashed UID:{hashed_hex}\nRandom Key:{random_hex}\nEncrypted_UID:{encrypted_hex}\n")
                        #self.safe_update(self.result_label, f"{random_hex}")
                        self.safe_update(self.result_label, f"Random Key:{random_hex}\nEncrypted_Auth:{encrypted_hex}\n")
                        formatted_hex = format_hex_multiline(random_hex)
                        self.safe_update(self.result_label, formatted_hex)
                        self.safe_start_countdown()
                except Exception as e:
                    self.safe_update(self.result_label, f"MCU 통신qqq 실패: {e}")
                    break


        thread = threading.Thread(target=run_communicate_loop)
        thread.daemon = True
        thread.start()
        send_req(self.default_timer)

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
        self.remaining_seconds = self.default_timer
        minutes = self.remaining_seconds // 60
        seconds = self.remaining_seconds % 60
        self.countdown_label.setText(f"{minutes:02}:{seconds:02}")
        self.countdown_label.show()
        self.timer.start(1000)


    def update_countdown(self):
        
        if self.remaining_seconds <=6:
            self.countdown_label.setStyleSheet("""
    font-size: 18px;
    color: #ff0000;
    font-weight: bold;
""")
        elif self.remaining_seconds ==0 or self.remaining_seconds >0 :
            self.countdown_label.setStyleSheet("""
    font-size: 18px;
    color: #0d6efd;
    font-weight: bold;
""")
        self.remaining_seconds -=  1
        minutes = self.remaining_seconds // 60
        seconds = self.remaining_seconds % 60
        self.countdown_label.setText(f"{minutes:02}:{seconds:02}")
    
        if self.remaining_seconds <= 0:
            self.countdown_label.setStyleSheet("""
    font-size: 18px;
    color: #0d6efd;
    font-weight: bold;
""")
            self.remaining_seconds = self.default_timer
            print("EXP TIME 만료 재요청")
            send_req(self.default_timer)
    def stop_communication(self):
        if self.timer_trigger==1:
            self.countdown_label.hide()
            self.timer_trigger=0;
            global_variables.running_trigger=False;
            self.stop_button.setIcon(QIcon("icon/start.png"))
            self.stop_button.setToolTip("시작")
            self.result_label.setText("중단됨")
            self.timer.stop()
        
        else:
            self.countdown_label.hide()
            global_variables.running_trigger=True;
            self.stop_button.setIcon(QIcon("icon/stop.png"))
            self.stop_button.setToolTip("중단")
            self.start_communication()
            self.timer_trigger=1;
                
                
                
    def refresh(self):
        self.result_label.setText("재요청 중...")
        self.start_communication()
        

    def open_timer_setting(self):
        dialog = QDialog(self)
        dialog.setWindowTitle("만료시간 설정")
        layout = QVBoxLayout()

        layout.addWidget(QLabel("만료 시간 설정"))

        input_layout = QHBoxLayout()

        self.min_input = QLineEdit()
        self.min_input.setPlaceholderText("분")
        self.min_input.setFixedWidth(50)
        self.min_input.setAlignment(Qt.AlignCenter)

        self.sec_input = QLineEdit()
        self.sec_input.setPlaceholderText("초")
        self.sec_input.setFixedWidth(50)
        self.sec_input.setAlignment(Qt.AlignCenter)

        input_layout.addWidget(self.min_input)
        input_layout.addWidget(QLabel("분 "))
        input_layout.addWidget(self.sec_input)
        input_layout.addWidget(QLabel("초"))
        layout.addLayout(input_layout)


        self.timer_error_label = QLabel("")
        self.timer_error_label.setStyleSheet("color: red; font-size: 12px;")
        layout.addWidget(self.timer_error_label)

        ok_button = QPushButton("확인")
        ok_button.clicked.connect(lambda: self.validate_set_expire_time(dialog))
        layout.addWidget(ok_button)

        dialog.setLayout(layout)
        dialog.exec_()
    def set_expire_time(self, value, dialog):
        self.default_timer = value
        dialog.accept()
        self.start_communication()
    def validate_set_expire_time(self, dialog):
        min_text = self.min_input.text()
        sec_text = self.sec_input.text()

        if not min_text and not sec_text:
            self.timer_error_label.setText("시간을 입력하세요.")
            return

        try:
            minutes = int(min_text) if min_text else 0
            seconds = int(sec_text) if sec_text else 0
            if minutes < 0 or seconds < 0 or seconds >= 60:
                raise ValueError
            total_seconds = minutes * 60 + seconds
            if total_seconds <= 0:
                self.timer_error_label.setText("0초 이상이어야 합니다.")
                return
            self.default_timer = total_seconds
            dialog.accept()
            self.start_communication()
        except ValueError:
            self.timer_error_label.setText("유효한 숫자만 입력해주세요.")


    def get_password_from_file(self):
        try:
            with open(CHECK_PATH, 'r') as file:
                pw = file.read().strip()
            return pw
        except Exception:
            return ""
