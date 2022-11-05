# sleep_thread

Provides sleep function not affected by the GIL.

```
pip install git+https://github.com/mukai1011/sleep_thread.git
```

```python
import timedelta from datetime
import sleep_thread as st

# example
duration = timedelta(seconds=10)
event = st.Event()

st.start(duration, event)
st.join()

# you **cannot** call start multiple times without a join
try:
    st.start(duration, event)
    st.start(duration, event)
except RuntimeError:
    pass
finally:
    # Even without join, thread should be terminated when the caller is destroyed, 
    # but of course that's a bad practice.
    st.join()

# attempting to join before calling start is also prohibited
try:
    st.join()
except RuntimeError:
    pass

# to cancel blocking, call like that from another thread
event.set()

# to check if join is due to cancellation
if event.is_set():
    pass
```
