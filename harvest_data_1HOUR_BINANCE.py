import pandas as pd
import talib
import datetime as dt
import numpy as np
from binance.client import Client
from binance.enums import *
import time
import config
from myTools import *
import pickle

HOUR = 1

def harvest(client, PAIR, datetime_begin, datetime_end):
    frames = []

    begin_dates = []
    end_dates = []

    kk = 0
    while True:
        begin_dates.append(datetime_begin + dt.timedelta(hours=900*HOUR*kk))
        end_dates.append(datetime_begin + dt.timedelta(hours=900*HOUR*(kk+1)))
        kk = kk+1
        if end_dates[-1] >= datetime_end:
            break

    if end_dates[-1] > datetime_end:
        end_dates[-1] = datetime_end

    nb = len(begin_dates)

    dfall = pd.DataFrame()
    df = pd.DataFrame()

    all_candlesticks = np.array([], dtype=float).reshape(0, 6)

    for ii in range(nb):
        print(ii, nb)

        t1_str = (begin_dates[ii]-dt.timedelta(hours=HOUR*10)
                  ).strftime('%a, %d %b %Y %H:%M:%S %Z')
        t2_str = (end_dates[ii]+dt.timedelta(hours=HOUR*10)
                  ).strftime('%a, %d %b %Y %H:%M:%S %Z')

        if HOUR == 1:
            candlesticks = client.get_historical_klines(PAIR, Client.KLINE_INTERVAL_1HOUR,
                                                        start_str=t1_str, end_str=t2_str, limit=1000)
        elif HOUR == 2:
            candlesticks = client.get_historical_klines(PAIR, Client.KLINE_INTERVAL_2HOUR,
                                                        start_str=t1_str, end_str=t2_str, limit=1000)
        elif HOUR == 4:
            candlesticks = client.get_historical_klines(PAIR, Client.KLINE_INTERVAL_4HOUR,
                                                        start_str=t1_str, end_str=t2_str, limit=1000)
        else:
            print('error')
            sys.exit()

        candlesticks = np.array(candlesticks)

        candlesticks = np.delete(candlesticks, [6, 7, 8, 9, 10, 11], 1)

        candlesticks = candlesticks.astype(float)

        all_candlesticks = np.concatenate(
            (all_candlesticks, candlesticks), axis=0)

    df = pd.DataFrame(
        all_candlesticks[:-1], columns=['timestamp', 'open', 'high', 'low', 'close', 'volume'])
    df['timestamp'] = df['timestamp']/1000.0
    df.round({'timestamp': 0})

    df = df.astype({"timestamp": int})

    df = df.drop_duplicates(subset=['timestamp'])
    df = df.reset_index(drop=True)
    df = df.sort_values(by='timestamp', ascending=True)
    df = df.reset_index(drop=True)

    df['time_str'] = [str(dt.datetime.fromtimestamp(np.array(val)))
                      for val in df['timestamp']]

    print(df.tail(5))

    return df

#########################################################################################################


def process(client, PAIR, datetime_begin, datetime_end):
    print('Harvesting {}'.format(PAIR))

    data = harvest(client, PAIR, datetime_begin, datetime_end)

    dt_array = np.array(data['timestamp'])

    pickle.dump([data, dt_array], open(
        "./data/{}_{}HOUR.p".format(PAIR, HOUR), "wb"))

    with open(f"./data/{PAIR}_{HOUR}HOUR.txt", "w") as f:
        for ii in range(len(data)):
            f.write(
                f"{data['timestamp'][ii]} {data['open'][ii]:.2f} {data['high'][ii]:.2f} {data['low'][ii]:.2f} {data['close'][ii]:.2f} \n")


#########################################################################################################

if __name__ == "__main__":

    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    datetime_begin = dt.datetime(2017, 8, 10, tzinfo=dt.timezone.utc)
    datetime_end = dt.datetime.utcnow().replace(tzinfo=dt.timezone.utc)-dt.timedelta(hours=2)

    client = Client()

    PAIRS = ['BTCUSDT', 'ETHUSDT']

    for PAIR in PAIRS:
        while True:
            try:
                process(client, PAIR, datetime_begin, datetime_end)
                break
            except Exception as error:
                error_str = repr(error)
                print(error_str)
                print('some error happened, trying again')
