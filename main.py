import requests
import json

from config import USER

SECRET = None
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
    global SECRET
    try:
        with open(".secret", "r") as file:
            SECRET = file.read().strip()
    except Exception:
        reg_user(USER)


def get_all_pairs():

    response = requests.get(f"{host}/getAllPairs")

    if response.status_code == 200:
        return json.dumps(response.json())
    else:
        print("FAILED: getAllPairs")


def create_orders(orders: list):
    """Create orders.

    Orders = [(FROM, TO, VOLUME), (BTC, USD, 100)]

    Args:
        orders (list): orders
    """
    orders = "|".join(",".join(order) for order in orders)
    response = requests.get(f"{host}/createOrders/{USER}/{SECRET}/{orders}")

    if response.status_code == 200:
        print(f"successfull trades: {orders}")
    else:
        print(f"FAILED: order ({orders})")


def balance():
    response = requests.get(f"{host}/balance/{USER}")
    if response.status_code == 200:
        return json.dumps(response.json())
    else:
        print(response.json())
        print("FAILED: balance")


if __name__ == "__main__":
    load_secret()
    # print(get_all_pairs())
    # exit()
    print(balance())
    create_orders([("USDT", "DUSK", "100")])
    print(balance())
