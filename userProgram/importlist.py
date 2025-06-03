from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QMessageBox, QLineEdit,QHBoxLayout,QSpacerItem,QSizePolicy,QDialog,QSpinBox
)
from PyQt5.QtGui import QIcon
import psutil
from PyQt5.QtCore import Qt,QSize
from PyQt5.QtCore import QTimer as QTimer
import sys
import os
import subprocess
domain = None;

# 체크할 파일 경로 (여기 본인 경로로 수정)
CHECK_PATH = "/Users/moonseoungmin/GIT/2025_Capstone/userProgram/auth.txt"
