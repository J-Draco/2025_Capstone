from importlist import CHECK_PATH,os
def check_auth():
    return  os.path.exists(CHECK_PATH)
