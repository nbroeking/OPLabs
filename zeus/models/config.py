class TestConfiguration(object):
    def __init__(self):
        self.default_config = {
                "dns_config": {
                    "valid_names": [
                        "google.com",
                        "facebook.com",
                        "microsoft.com",
                        "apple.com",
                        ],
                    "invalid_names": [
                        "abc.def.ghi.jkl.com",
                        "invalid.domain.com",
                        "a.b.c.d.e.f.net",
                        "potato.xyz"
                        ],
                    "dns_server": "8.8.8.8",
                    "timeout": 2000
                    },
                "throughput_config": {
                    "server_ip": "128.138.202.143:" + THROUGHPUT_PORT,
                    },
                }
    def get_config(self):
        return dict(self.default_config)
