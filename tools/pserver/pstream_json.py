#!/usr/bin/env python3
import json
import time
from pstream_base import PStreamBase

class PStreamJSON(PStreamBase):
    """JSON stream implementation that generates sensor data."""

    def __init__(self):
        super().__init__()
        self.counter = 0
        self.sample_data = [
            {
                "sensor": "temperature",
                "value": 25.5,
                "unit": "celsius"
            },
            {
                "sensor": "humidity",
                "value": 60.2,
                "unit": "percent"
            },
            {
                "sensor": "pressure",
                "value": 1013.25,
                "unit": "hPa"
            }
        ]

    def get_next_data(self) -> bytes:
        """Generate the next JSON data packet."""
        # Cycle through sample data
        data_index = self.counter % len(self.sample_data) # The % does the cycling
        json_obj = self.sample_data[data_index].copy()

        # Add timestamp and sequence number
        json_obj["timestamp"] = time.time()
        json_obj["sequence"] = self.counter

        # Add variation to the values
        if json_obj["sensor"] == "temperature":
            json_obj["value"] = 25.5 + (self.counter % 10) * 0.5
        elif json_obj["sensor"] == "humidity":
            json_obj["value"] = 60.2 + (self.counter % 10) * 0.3
        elif json_obj["sensor"] == "pressure":
            json_obj["value"] = 1013.25 + (self.counter % 10) * 0.1

        self.counter += 1

        # Convert to JSON string with newline delimiter ('\n')
        json_str = json.dumps(json_obj)
        message = json_str + '\n'
       

        return message.encode('utf-8')