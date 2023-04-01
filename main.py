import requests
import json

from config import USER

secret = None
host = "http://192.168.1.101:3000"


def reg_user(username):
    response = requests.get(f"{host}/register/{username}")
    if response.status_code == 200:
        secret = response.json()["secret"]
        save_secret(secret)
    else:
        print(response.json())
        print("FAILED: register")


def save_secret(secret):
    with open(".secret", "w") as file:
        file.write(secret)


def load_secret():
    try:
        with open(".secret", "r") as file:
            secret = file.read().strip()
    except Exception:
        reg_user(USER)


def balance():
    response = requests.get(f"{host}/balance/{USER}")
    if response.status_code == 200:
        return json.dumps(response.json())
    else:
        print(response.json())
        print("FAILED: balance")


if __name__ == "__main__":
    secret = load_secret()
    bal = balance()
    print(bal)
