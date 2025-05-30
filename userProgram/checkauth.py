from importlist import CHECK_PATH,os
def checkauth():
    return  os.path.exists(CHECK_PATH)
