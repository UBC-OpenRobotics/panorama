#!/usr/bin/env python3
import json
import time
from pstream_base import PStreamBase
import random
import time


class PStreamJSON(PStreamBase):
    """JSON stream implementation that generates sensor data."""

    def __init__(self):
        super().__init__() #gives other methods access to the base class
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

        # Simulating corrupt data
        if random.random() < 0.5: # 50% chance of seeing corruption in data
            corruption_type = random.choice(["nonphysical", "noise", "missing", "no_change"])

            if corruption_type == "nonphysical":
                json_obj["value"] == random.uniform(-9999, 9999)
            elif corruption_type == "noise":
                json_obj["value"] *= random.uniform(1.5, 3.0)
            elif corruption_type == "missing":
                del json_obj["value"]
            elif corruption_type == "no_change":
                pass

        self.counter += 1

        """
        Simulating delay in data stream.
        Note: If there is a delay,  the server will state: No data received
        """
        if random.random() < 0.5: # 50% chance of delay
            pause_time = random.uniform(1, 5) 
            time.sleep(pause_time)

        # Convert to JSON string with newline delimiter ('\n')
        return (json.dumps(json_obj) + '\n').encode('utf-8') 