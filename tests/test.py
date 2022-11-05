from datetime import timedelta
import threading
import multiprocessing
from time import perf_counter

import sleep_thread as st

def sleep(duration: timedelta, event):
    total_seconds = duration.total_seconds()
    current_time = perf_counter()
    while perf_counter() < current_time + total_seconds and not event.is_set():
        pass

def sub_thread(duration: timedelta, event):

    print()

    # 1. Threadのビジーウェイト
    print("--- Thread ---")
    th_event = threading.Event()
    th = threading.Thread(target=sleep, args=(duration, th_event))
    
    current_time = perf_counter()
    th.start()
    th.join()
    print(perf_counter() - current_time)

    # 2. Processのビジーウェイト
    print("--- Process ---")
    proc_event = multiprocessing.Event()
    proc = multiprocessing.Process(target=sleep, args=(duration, proc_event))
    
    current_time = perf_counter()
    proc.start()
    proc.join()
    print(perf_counter() - current_time)
    
    # 3. m.start() ~ m.join()のビジーウェイト
    print("--- C++ ---")
    m_event = st.Event()
    
    current_time = perf_counter()
    st.start(duration, m_event)
    st.join()
    print(perf_counter() - current_time)
    
    print()
    event.set()

def fibonacci(n, event):
    if event.is_set():
        raise Exception()

    if n == 1:
        return 0
    elif n == 2:
        return 1
    else:
        return fibonacci(n - 1, event) + fibonacci(n - 2, event)

def main():

    duration = timedelta(seconds=10)
    event = threading.Event()
    
    sub_th = threading.Thread(target=sub_thread, args=(duration, event))
    sub_th.start()

    # Main thread is supercalifragilisticexpialidocious-ly busy
    try:
        while not event.is_set():
            fibonacci(100, event)
    except:
        pass
    
    sub_th.join()

if __name__ == "__main__":
    main()
