from importlist import QMessageBox,sys

def show_error(errortype):
    msg = QMessageBox()
    msg.setIcon(QMessageBox.NoIcon)
    msg.setWindowTitle("ERROR")
    if errortype == 1:
        msg.setText("인증파일을 확인할 수 없습니다.")
    if errortype == 2:
        msg.setText("모듈을 장착하여 주세요.")
    msg.setStandardButtons(QMessageBox.Ok)
    msg.exec_()
    sys.exit();
