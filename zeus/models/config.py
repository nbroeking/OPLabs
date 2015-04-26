from app import THROUGHPUT_PORT, APP_ROOT
from os.path import join
import random

with open(join(APP_ROOT, "data/opendns-random-domains.txt"), 'r') as rdoms:
    random_domains = [line.strip() for line in rdoms.readlines()]

with open(join(APP_ROOT, "data/opendns-top-domains.txt"), 'r') as tdoms:
    top_domains = [line.strip() for line in tdoms.readlines()]

class TestConfiguration(object):
    def __init__(self):
        self.default_config = {
                "dns_config": {
                    "valid_names": random.sample(top_domains, 10),
                    "invalid_names": random.sample(random_domains, 10),
                    "dns_server": "8.8.8.8",
                    "timeout": 2000
                    },
                "throughput_config": {
                    "server_ip": "10.201.21.216:" + THROUGHPUT_PORT,
                    },
                }
    def get_config(self):
        return dict(self.default_config)
