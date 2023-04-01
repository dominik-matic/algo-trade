import requests
import json
import time
import threading

from config import USER, FIX_DELAY, HOST, TICKRATE, NUM_HOPS
from VeryCool import find_stonks
from math import floor

SECRET = None
TICK_TIME = None
BALANCE = {}
ALL_PAIRS = {}


def update_balance(orders: list):
    """Update balance.

    Orders = [(FROM, TO, VOLUME), (BTC, USD, 100)]
    100 is NOT * 10**8 !

    Args:
        orders (list): orders
    """
    global BALANCE
    for order in orders:
        fromStock, toStock, volume = order

        if toStock not in BALANCE.keys():
            BALANCE[toStock] = int(volume * ALL_PAIRS[f"close_{fromStock},{toStock}"])
        else:
            BALANCE[toStock] += int(volume * ALL_PAIRS[f"close_{fromStock},{toStock}"])
        BALANCE[fromStock] -= int(volume * 10**8)


def sync():
    global TICK_TIME
    while time.time() > TICK_TIME:
        TICK_TIME += TICKRATE
    time.sleep(TICK_TIME - time.time())


def reg_user(username: str) -> None:
    response = requests.get(f"{HOST}/register/{username}")
    if response.status_code == 200:
        secret = response.json()["secret"]
        save_secret(secret)
    else:
        print("FAILED: register")


def save_secret(secret: str) -> None:
    with open(".secret", "w") as file:
        file.write(secret)


def load_secret() -> None:
    global SECRET
    try:
        with open(".secret", "r") as file:
            SECRET = file.read().strip()
    except Exception:
        reg_user(USER)


def get_all_pairs():
    global ALL_PAIRS
    response = requests.get(f"{HOST}/getAllPairs")

    if response.status_code == 200:
        ALL_PAIRS = response.json()
    else:
        print("FAILED: getAllPairs")


def create_orders(orders: list) -> bool:
    """Create orders.

    Orders = [(FROM, TO, VOLUME), (BTC, USD, 100)]

    Args:
        orders (list): orders
    """
    orders = list((order[0], order[1], str(int(order[2]))) for order in orders)

    orders = "|".join(",".join(order) for order in orders)
    response = requests.get(f"{HOST}/createOrders/{USER}/{SECRET}/{orders}")

    if response.status_code == 200:
        print("%%%%%%%")
        print(f"% successfull trades: {orders}")
        print("%%%%%%%")
        return True
    else:
        print(f"FAILED: order ({orders}) ({response.content})")
        return False


def balance():
    global BALANCE
    response = requests.get(f"{HOST}/balance/{USER}")
    if response.status_code == 200:
        BALANCE = response.json()
    else:
        print("FAILED: balance")


def get_current_tick() -> int:
    response = requests.get(f"{HOST}/getTime")
    if response.status_code == 200:
        return int(response.json())
    else:
        print("FAILED: getTime")
        return None


def tick_finder(time_wait=TICKRATE, num_hops=NUM_HOPS) -> None:
    global TICK_TIME
    delay = 0.25
    current_tick = get_current_tick()

    for i in range(num_hops):
        while True:
            time.sleep(delay)
            next_tick = get_current_tick()
            if current_tick != next_tick:
                break
        if i < num_hops - 1:
            time.sleep(time_wait - (delay * 1.02))
            delay /= 4

    TICK_TIME = time.time() + FIX_DELAY


def trade():
    all_tickers = list(ticker for ticker in BALANCE if BALANCE[ticker] > 0)

    closeDict, volumeDict, idx2key, key2idx, cycles, fran_cycles = find_stonks(
        ALL_PAIRS, all_tickers, n_cycles=5, max_depth=50
    )
    cycles2 = fran_cycles
    for orders in cycles2:
        start = [BALANCE[orders[0][0]] * 0.005]
        for order in orders:
            order1_recived = (
                floor((start[-1] * closeDict[order[0]][order[1]])) // 10**8
            )
            start.append(order1_recived)

        orders = list((order[0], order[1], vol) for order, vol in zip(orders, start))
        if any(order[2] < 10 for order in orders):
            continue

        if create_orders(orders):
            balance()


def update_thread():
    tick_finder()
    balance()
    get_all_pairs()

    while True:
        sync()
        get_all_pairs()
        balance()
        print("% THREAD: I did updaty")


def main():
    load_secret()

    threading.Thread(target=update_thread).start()
    time.sleep(NUM_HOPS * TICKRATE + TICKRATE / 3)

    while True:
        trade()


if __name__ == "__main__":
    main()
