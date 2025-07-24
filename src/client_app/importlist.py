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

# 상대 경로로 변경
CHECK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "secure", "auth.txt")
