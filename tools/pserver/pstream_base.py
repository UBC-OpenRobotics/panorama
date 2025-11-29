#!/usr/bin/env python3
from abc import ABC, abstractmethod
from typing import Optional
import queue

class PStreamBase(ABC):
    """
    Base class for all stream types. 
    Subclasses must implement get_next_data().
    """

    def __init__(self):
        self.data_queue = queue.Queue(maxsize=100)
        self._running = False

    @abstractmethod
    def get_next_data(self) -> bytes:
        """
        Generate and return the next piece of data to stream.
        This should be implemented by subclasses to define their data format.

        Returns:
            bytes: The data to be sent to the client
        """
        pass

    def start(self):
        """Mark the stream as running."""
        self._running = True

    def stop(self):
        """Mark the stream as stopped."""
        self._running = False

    def is_running(self) -> bool:
        """Check if the stream is running."""
        return self._running

    def put_data(self, data: bytes):
        """Put data into the queue for sending."""
        try:
            self.data_queue.put(data, block=False)
        except queue.Full:
            # Drop oldest data if queue is full
            try:
                self.data_queue.get_nowait()
                self.data_queue.put(data, block=False)
            except queue.Empty:
                pass

    def get_data(self, timeout: Optional[float] = None) -> Optional[bytes]:
        """Get data from the queue."""
        try:
            return self.data_queue.get(timeout=timeout)
        except queue.Empty:
            return None