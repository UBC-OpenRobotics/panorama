#!/usr/bin/env python3
import threading
import time
from typing import Optional
from putils import *
from pstream_base import PStreamBase

class PStreamer:
    """
    Builder pattern class that manages a stream in a separate thread.
    It continuously generates data using the provided stream implementation
    and makes it available via a queue.
    """

    def __init__(self):
        self.stream: Optional[PStreamBase] = None
        self.thread: Optional[threading.Thread] = None
        self._stop_event = threading.Event()
        self.stream_interval = 1.0  # *** By Default: 1 second between data generation! ***

    def build_stream(self, stream: PStreamBase) -> 'PStreamer':
        """
        Builder method to set the stream implementation.

        Args:
            stream: An instance of a PStreamBase subclass

        Returns:
            self for method chaining
        """
        self.stream = stream
        return self

    def set_interval(self, interval: float) -> 'PStreamer':
        """
        Builder method to set the streaming interval.

        Args:
            interval: Time in seconds between data generation

        Returns:
            self for method chaining
        """
        self.stream_interval = interval
        return self

    def _stream_worker(self):
        """Internal worker method that runs in a separate thread."""
        if self.stream is None:
            raise ValueError("Stream not built. Call build_stream() first.")

        self.stream.start()
        print(f"[PStreamer] Stream thread started with {self.stream.__class__.__name__}")

        try:
            while not self._stop_event.is_set():
                # Generate next data
                data = self.stream.get_next_data()

                # Put data into the queue
                self.stream.put_data(data)

                # Wait for the specified interval or until stop is requested
                self._stop_event.wait(timeout=self.stream_interval)

                #print(data)

        except Exception as e:
            print(f"[PStreamer] Error in stream worker: {e}")
        finally:
            self.stream.stop()
            print("[PStreamer] Stream thread stopped")

    def start(self):
        """Start the streaming thread."""
        if self.stream is None:
            raise ValueError("Stream not built. Call build_stream() first.")

        if self.thread is not None and self.thread.is_alive():
            print("[PStreamer] Stream already running")
            return

        self._stop_event.clear()
        self.thread = threading.Thread(target=self._stream_worker, daemon=True)
        self.thread.start()

    def stop(self):
        """Stop the streaming thread."""
        if self.thread is None or not self.thread.is_alive():
            print("[PStreamer] Stream not running")
            return

        print("[PStreamer] Stopping stream thread...")
        self._stop_event.set()
        self.thread.join(timeout=5.0)

        if self.thread.is_alive():
            print("[PStreamer] Warning: Thread did not stop cleanly")

    def get_data(self, timeout: Optional[float] = None) -> Optional[bytes]:
        """
        Get data from the stream queue.

        Args:
            timeout: Maximum time to wait for data

        Returns:
            Data bytes or None if no data available
        """
        if self.stream is None:
            return None

        return self.stream.get_data(timeout=timeout)

    def is_running(self) -> bool:
        """Check if the streaming thread is running."""
        return self.thread is not None and self.thread.is_alive()


if __name__ == "__main__":
    from pstream_json import PStreamJSON

    streamer = PStreamer() # Stream builder class
    # Building JSON stream using the PStreamJSON class
    streamer.build_stream(PStreamJSON()).set_interval(1.0)

    streamer.start()

    try:
        # Read and print data for testing purposes
        for i in range(10):
            data = streamer.get_data(timeout=2.0)
            if data:
                print(f"Received: {data.decode('utf-8').strip()}")
            else:
                print("No data received")
    except KeyboardInterrupt:
        print("\nStopping...")
    finally:
        streamer.stop()
