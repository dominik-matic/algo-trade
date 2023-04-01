import requests
import json
import time

from config import USER

SECRET = None
HOST = "http://192.168.1.101:3000"


def reg_user(username):
    response = requests.get(f"{HOST}/register/{username}")
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

    response = requests.get(f"{HOST}/getAllPairs")

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
    response = requests.get(f"{HOST}/createOrders/{USER}/{SECRET}/{orders}")

    if response.status_code == 200:
        print(f"successfull trades: {orders}")
    else:
        print(f"FAILED: order ({orders})")


def balance():
    response = requests.get(f"{HOST}/balance/{USER}")
    if response.status_code == 200:
        return json.dumps(response.json())
    else:
        print(response.json())
        print("FAILED: balance")


def get_time():
    response = requests.get(f"{HOST}/getTime")
    if response.status_code == 200:
        return int(response.json())
    else:
        print("FAILED: getTime")
        return None


def tick_finder(time_wait=30, num_hops=3):
    delay = 0.25
    current_tick = get_time()

    for i in range(num_hops):
        while True:
            time.sleep(delay)
            next_tick = get_time()
            if current_tick != next_tick:
                break
        if i < num_hops - 1:
            time.sleep(time_wait - (delay * 1.02))
            delay /= 10

    return time.time()


if __name__ == "__main__":
    load_secret()
    get_time()
    tick_finder()
    while True:
        print(get_time())
        time.sleep(30)
