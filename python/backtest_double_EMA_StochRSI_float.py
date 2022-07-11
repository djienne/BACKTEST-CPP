import pandas as pd
import numpy as np
import pandas_ta as ta
import psutil
import time
import os

abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

class RUN_RESULT():
    pass

DATAFILE = "../data/Binance/4h/BTC-USDT.csv"
STRAT_NAME = "2-EMA crossover with Stoch RSI"

start_year = 2017  # forced year to start (applies if data below is available)
FEE = 0.07        # FEES in %
USDT_amount_initial = 1000.0
MIN_NUMBER_OF_TRADES_PER_YEAR = 14 # minimum number of trades required (to avoid some noise / lucky circunstances)
MIN_NUMBER_OF_TRADES = -1
MIN_ALLOWED_MAX_DRAWBACK = -50.0  # %
MAX_ALLOWED_DAYS_BETWEEN_PORTFOLIO_ATH = 365
STOCH_RSI_UPPER = 0.800
STOCH_RSI_LOWER = 0.200
minimum_yearly_gain_pc = -100.0  # pc
i_start_year = 0

# RANGE OF EMA PERIDOS TO TEST ####################################
period_max_EMA = 350
range_step = 1
range1 = np.arange(2, period_max_EMA, range_step)
range2 = np.arange(2, period_max_EMA, range_step)
#range1 = [116]
#range2 = [3]
###################################################################

i_print = 0
ii_begin = 0
nb_tested = 0

###################################################################

def custom_stochRSI_TravingView_Style(close, length=14, rsi_length=14, k=3, d=3):
    """Indicator: Stochastic RSI Oscillator (STOCHRSI)
    Should be similar to TradingView's calculation"""
    if k<0:
        raise Exception("k cannot be negative")
    if d<0:
        raise Exception("d cannot be negative")
    # Calculate Result
    rsi_ = ta.rsi(close, length=rsi_length,talib=False)
    lowest_rsi = rsi_.rolling(length).min()
    highest_rsi = rsi_.rolling(length).max()
    stochrsi = 100.0 * (rsi_ - lowest_rsi) / ta.non_zero_range(highest_rsi, lowest_rsi)
    if k>0:
        stochrsi_k = ta.ma('sma', stochrsi, length=k ,talib=False)
        stochrsi_d = ta.ma('sma', stochrsi_k, length=d ,talib=False)
    else:
        stochrsi_k = None
        stochrsi_d = None
    return (stochrsi/100.0).round(4), (stochrsi_k/100.0).round(4), (stochrsi_d/100.0).round(4)

###################################################################

def print_res(best):
    print("-------------------------------------")
    print("EMA: ", best.ema1, " ", best.ema2)
    print("Gain: ", best.gain_pc, "%")
    print("Win rate: ", best.win_rate, "%")
    print("max DD: ", best.max_DD, "%")
    print("Gain over DDC: ", best.gain_over_DDC)
    print("Score: ", best.score)
    print("Number of trades: ", best.nb_posi_entered)
###################################################################

def print_best_res(best):
    print("\n-------------------------------------")
    print("BEST PARAMETER SET FOUND: ")
    print("-------------------------------------")
    print(f"Strategy : {STRAT_NAME}")
    print(f"EMAs     : ", best.ema1, " ", best.ema2)
    print(f"Best Gain: {best.gain_pc:.2f}%")
    print(f"Porfolio : {best.WALLET_VAL_USDT:.2f}$ (started with 1000$)")
    print(f"Win rate : {best.win_rate:.2f}%")
    print(f"max DD   : {best.max_DD:.2f}%")
    print(f"Gain/DDC : {best.gain_over_DDC:.2f}")
    print(f"Score    : {best.score:.2f}")
    print("Number of trades: ", best.nb_posi_entered)
    print("Yearly gains: ")
    for i in range(len(best.years_yearly_gains)):
        print(best.years_yearly_gains[i], " :: ", best.yearly_gains[i], "%")

    print("Total fees paid: ", round(best.total_fees_paid*100.0)/100.0, "$ (started with 1000$)")

    print("-------------------------------------")

###################################################################

def INITIALIZE_DATA(kline):
    global MIN_NUMBER_OF_TRADES
    global ii_begin
    list_ema = []

    maxval = np.max([np.max(range1), np.max(range2)]) + 5
    for i in range(maxval):
        list_ema.append(i)

    for i in list_ema:
        kline["EMA" + str(i)] = ta.ema(kline["close"], length=int(i), talib=False)

    print("Calculated EMAs.")
    kline['StochRSI'], _, _ = custom_stochRSI_TravingView_Style(kline["close"], length=14, rsi_length=14, k=3, d=3)
    print("Calculated STOCHRSI.")

    kline['date'] = pd.to_datetime(kline['time'], unit='ms')

    kline['year']  = kline['date'].dt.year
    kline['hour']  = kline['date'].dt.hour
    kline['month'] = kline['date'].dt.month
    kline['day']   = kline['date'].dt.day
    kline['shifted_year']  = kline['year'].shift(-1)

    MIN_NUMBER_OF_TRADES = MIN_NUMBER_OF_TRADES_PER_YEAR * int(np.max(kline['year']) - np.min(kline['year']) + 1)

    ii_begin = period_max_EMA + 2

    print("Initialized calculations.")

###################################################################

def PROCESS(kline, ema1_v, ema2_v):
    global nb_tested
    global range1
    global range2
    global i_print
    global ii_begin
    global FEE

    nb_tested += 1

    result = RUN_RESULT()

    result.years_yearly_gains = []
    result.yearly_gains = []

    nb_max = len(kline)

    LAST_ITERATION = False
    OPEN_LONG_CONDI = False
    CLOSE_LONG_CONDI = False
    nb_profit = 0
    nb_loss = 0
    NB_POSI_ENTERED = 0
    pc_change_with_max = 0.0
    max_drawdown = 0.0
    price_position_open = 0.0

    USDT_amount = USDT_amount_initial
    WALLET_VAL_begin_year = USDT_amount_initial
    MAX_WALLET_VAL_USDT = USDT_amount_initial
    COIN_AMOUNT = 0.0
    total_fees_paid_USDT = 0.0
    WALLET_VAL_USDT = USDT_amount_initial

    emastr1 = "EMA" + str(ema1_v)
    emastr2 = "EMA" + str(ema2_v)

    for ii, row in kline.iterrows():

        if (ii<ii_begin):
            continue

        if (ii == nb_max - 1):
            LAST_ITERATION = True

        # condition for open / close position
        OPEN_LONG_CONDI  = row[emastr2] >= row[emastr1] and row['StochRSI'] > STOCH_RSI_UPPER
        CLOSE_LONG_CONDI = row[emastr2] <= row[emastr1] and row['StochRSI'] < STOCH_RSI_LOWER

        # IT IS IMPORTANT TO CHECK FIRST FOR CLOSING POSITION AND THEN FOR OPENING POSITION

        # CLOSE LONG
        if (COIN_AMOUNT > 0.0 and (CLOSE_LONG_CONDI or LAST_ITERATION)):

            USDT_amount = COIN_AMOUNT * row['close']
            COIN_AMOUNT = 0.0

            # apply FEEs
            fe = USDT_amount * FEE / 100.0
            USDT_amount -= fe
            total_fees_paid_USDT += fe
            #
            if (row['close'] >= price_position_open):
                nb_profit += 1
            else:
                nb_loss += 1

        # OPEN LONG
        if (COIN_AMOUNT == 0.0 and OPEN_LONG_CONDI and LAST_ITERATION == False):

            price_position_open = row['close']
            COIN_AMOUNT = USDT_amount / row['close']
            USDT_amount = 0.0

            # apply FEEs
            fe = COIN_AMOUNT * FEE / 100.0
            COIN_AMOUNT -= fe
            total_fees_paid_USDT += fe * row['close']
            #

            NB_POSI_ENTERED += 1

        # check yealy gains

        if (row['shifted_year'] != row['year'] or LAST_ITERATION):
            result.years_yearly_gains.append(row['year'])
            WALLET_VAL_USDT = USDT_amount + COIN_AMOUNT * row['close']
            yg = (WALLET_VAL_USDT - WALLET_VAL_begin_year) / WALLET_VAL_begin_year * 100.0
            result.yearly_gains.append(np.round(yg * 100.0) / 100.0)
            WALLET_VAL_begin_year = WALLET_VAL_USDT

        # check wallet status
        if (CLOSE_LONG_CONDI or LAST_ITERATION):
            WALLET_VAL_USDT = USDT_amount + COIN_AMOUNT * row['close']
            if (WALLET_VAL_USDT > MAX_WALLET_VAL_USDT):
                MAX_WALLET_VAL_USDT = WALLET_VAL_USDT

            pc_change_with_max = (WALLET_VAL_USDT - MAX_WALLET_VAL_USDT) / MAX_WALLET_VAL_USDT * 100.0

            if (pc_change_with_max < max_drawdown):
                max_drawdown = pc_change_with_max

    WALLET_VAL_USDT = USDT_amount + COIN_AMOUNT * row['close']

    gain = (WALLET_VAL_USDT - USDT_amount_initial) / USDT_amount_initial * 100.0
    WR = float(nb_profit) / float(NB_POSI_ENTERED) * 100.0
    DDC = (1.0 / (1.0 + max_drawdown / 100.0) - 1.0) * 100.0
    score = gain / DDC * WR

    i_print += 1

    if (i_print == 1000):
        i_print = 0
        print("DONE: EMA: ", ema1_v, " and EMA: ", ema2_v)

    result.WALLET_VAL_USDT = USDT_amount
    result.gain_over_DDC = gain / DDC
    result.gain_pc = gain
    result.max_DD = max_drawdown
    result.nb_posi_entered = NB_POSI_ENTERED
    result.win_rate = WR
    result.score = score
    result.ema1 = ema1_v
    result.ema2 = ema2_v
    result.total_fees_paid = total_fees_paid_USDT

    return result

###################################################################

def read_input_data(input_file_path):
    kline = pd.read_csv(input_file_path, delimiter=';',header=None,names=['time','open','high','low','close','volume'])
    print(kline.tail(4))
    print("Loaded data file.")
    return kline

###################################################################

if __name__ == "__main__":

    t_begin = time.time()

    print("\n-------------------------------------")
    print("Strategy to test: ", STRAT_NAME)
    print("DATA FILE TO PROCESS: ", DATAFILE)

    kline = read_input_data(DATAFILE)

    INITIALIZE_DATA(kline)

    best = RUN_RESULT()

    best.gain_over_DDC = -100.0

    # Display info
    print("Begin day      : ", kline['year'][0], "/", kline['month'][0], "/", kline['day'][0])
    print("End day        : ", kline['year'].iloc[-1], "/", kline['month'].iloc[-1], "/", kline['day'].iloc[-1])
    print("OPEN/CLOSE FEE : ", FEE, " %")
    print("Minimum number of trades required    : ", MIN_NUMBER_OF_TRADES)
    print("Maximum drawback (=drawdown) allowed : ", MIN_ALLOWED_MAX_DRAWBACK, " %")
    print("StochRSI Upper Band   : ", STOCH_RSI_UPPER)
    print("StochRSI Lower Band   : ", STOCH_RSI_LOWER)
    print("EMA period max tested : ", period_max_EMA)
    print("EMA range step        : ", range_step)
    print("-------------------------------------")

    # MAIN LOOP

    for ema1 in range1:
        for ema2 in range2:

            if (np.abs(ema1-ema2) < 3):
                continue

            res = PROCESS(kline, ema1, ema2)

            if (res.gain_over_DDC > best.gain_over_DDC
                    and res.gain_pc < 100000.0
                    and res.nb_posi_entered >= MIN_NUMBER_OF_TRADES
                    and res.max_DD > MIN_ALLOWED_MAX_DRAWBACK
                    and np.min(res.yearly_gains) > minimum_yearly_gain_pc):

                best = res

    print_best_res(best)
    t_end = time.time()

    print("Number of backtests performed : ", nb_tested)
    print("Time taken                    : ", int(t_end - t_begin), " seconds ")
    ram_usage = psutil.virtual_memory().available * 100.0 / psutil.virtual_memory().total
    print("RAM usage                     : ", np.round(ram_usage * 10.0) / 10.0 , " MB" )
    print("-------------------------------------")
