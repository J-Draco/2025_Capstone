from importlist import QApplication, sys,psutil
from checkauth import checkauth
from show_error import show_error
from show_ui import MainWindow


window = None 
app = QApplication(sys.argv)
mcu_running = False
for proc in psutil.process_iter(['name']):
    if 'clion' in proc.info['name'].lower():  # 예: 프로세스 이름에 'mcu' 포함 여부
        mcu_running = True
        break

if not checkauth():
    show_error(errortype=1)
else:
    if not mcu_running:
        show_error(errortype=2)
    else:
        window = MainWindow()
        window.show()

sys.exit(app.exec_())
