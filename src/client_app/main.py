from importlist import QApplication, sys,psutil
from auth_validator import check_auth
from error_handler import show_error
from ui_controller import MainWindow


window = None 
app = QApplication(sys.argv)
mcu_running = False
for proc in psutil.process_iter(['name']):
    if 'clion' in proc.info['name'].lower():  
        mcu_running = True
        break

if not check_auth():
    show_error(errortype=1)
else:
    if not mcu_running:
        show_error(errortype=2)
    else:
        window = MainWindow()
        window.show()

sys.exit(app.exec_())
