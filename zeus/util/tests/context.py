"""
    This file contains some useful utilities for creating tests.

    Author: Zach Anders
    Date: February 8th
"""

import socket
from util.tests.output import FAILURE

def check_status(response, code=200, need_json=True, fields=()):
    def is_json(inp):
        try:
             inp.json()
             return True
        except:
             return False

    assert response.status_code == code, \
        FAILURE("Bad status code. Expected {code} got {res}".format(
            code=code,
            res=response.status_code)
               )

    # Optional: Check if response needs json
    if need_json:
        assert is_json(response), FAILURE("Response did not return JSON!")

        # Optional: Check if certain fields are present in JSON
        for field in fields:
            assert field in response.json(), FAILURE("JSON missing field: %s" % str(field))

class __TEST_CONTEXT__():
    def __init__(self):
        self.host = 'localhost'
        self.scheme = 'https'
        self.email = 'testuser@example.com'
        self.passwd = 'UpStb15dIvBpTmRJSxB0Ma2ZhGa6lh50'
        self.port = 443

    def make_url(self, route='/'):
        if route.startswith("/"):
            route = route[1:]
        return "{scheme}://{host}:{port}/{route}".format(
            scheme=self.scheme,
            host=self.host,
            port=str(self.port),
            route=route
            )

    def setSSL(self, isSSL):
        if isSSL:
            self.scheme = "https"
        else:
            self.scheme = "http"

    def is_up(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        try:
            s.connect((self.host, self.port))
        except socket.error:
            return False

        s.close()
        return True

# Create a global instance of context
Context = __TEST_CONTEXT__()
