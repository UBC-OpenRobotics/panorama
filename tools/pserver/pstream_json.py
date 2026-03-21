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

        """
        Fundamental Change:
        Instead of using self.counter, we will introduce a seeded 
        RNG to maintain deterministic behaviour.
        """

        # Add variation to the values
        random.seed(self.counter) # Seed the RNG with the counter for deterministic behavior
        if json_obj["sensor"] == "temperature":
            json_obj["value"] = 25.5 + random.uniform(0, 4.5)
        elif json_obj["sensor"] == "humidity":
            json_obj["value"] = 60.2 + random.uniform(0, 2.7)
        elif json_obj["sensor"] == "pressure":
            json_obj["value"] = 1013.25 + random.uniform(0, 0.9)
        

        # Simulating bad data
        if random.random() < 0.4: # 40% chance of seeing bad data
            data_type: str = random.choice(["nonphysical", "noise", "missing"])

            if data_type == "nonphysical":
                json_obj["value"] = random.uniform(-9999, 9999)
            elif data_type == "noise":
                json_obj["value"] *= random.uniform(1.5, 3.0)
            elif data_type == "missing":
                del json_obj["value"]

        self.counter += 1

        # Simulating data corruption and hardware failures
        if random.random() < 0.4: #40% chance of seeing corrupt data or hardware failure
            corruption_type: str = random.choice(["unusable", "hardware_failure"])

            if corruption_type == "unusable":
                random_ascii = random.randint(32, 126)
                random_char = chr(random_ascii)
                json_obj["value"] = random_char
                
            elif corruption_type == "hardware_failure":
                # Simulate hardware failure by setting value to None and adding error status
                json_obj["value"] = None
                json_obj["status"] = "hardware_failure"
                json_obj["error_message"] = "Sensor hardware malfunction detected"
            

        """
        Simulating delay in data stream.
        Note: If there is a delay,  the server will state: No data received
        """
        if random.random() < 0.5: # 50% chance of delay
            pause_time = random.uniform(1, 5) 
            time.sleep(pause_time)

        # Convert to JSON string with newline delimiter ('\n')
        return (json.dumps(json_obj) + '\n').encode('utf-8') 