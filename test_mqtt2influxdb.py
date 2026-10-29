import unittest
from mqtt2influxdb import parse_message, SensorData

class TestMqtt2Influxdb(unittest.TestCase):
    def test_parse_message_invalid_payload(self):
        """Test that a non-numeric payload raises a ValueError."""
        topic = 'ab123456/sensor1/temp'
        payload = 'not-a-number'
        with self.assertRaises(ValueError):
            parse_message(topic, payload)

if __name__ == '__main__':
    unittest.main()