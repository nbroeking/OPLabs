"""

Contains functionality that simplifies authentication with
regards to the public RESTful API. Functionality contained is expected to
operate on either AJAX calls or some other REST client, where queries and
responses are purely JSON, not rendered HTML.

Author: Zach Anders
Date: 02/21/2015

"""

from flask import request
from models.auth_model import User
from functools import wraps
from util.json_helpers import JSON_FAILURE
from binascii import unhexlify

# This is the magic cookie the router is expecting
MAGIC_PORT = 8639
MAGIC_COOKIE_HEX = "e21a14c8a2350a92af1affd6352ba4f39779afb5c12343f0f7141762534aa97e"
MAGIC_COOKIE = unhexlify(MAGIC_COOKIE_HEX)

class base_requirement(object):
    """ This is a convenience decorator that will ensure the wrapped function will
        only be called if the currently active request contains a REST token.
        If it does not, it invokes the 'invalid_handler', which by default
        simply returns an HTTP 401 error to the client. """

    def validator(self):
        raise NotImplementedError("Validator not implemented in decorator!")

    def __init__(self, invalid_handler=None):
        if invalid_handler:
            self.invalid_handler = invalid_handler
        else:
            self.invalid_handler = lambda: (JSON_FAILURE(), 401)

    def __call__(self, route_handler):
        @wraps(route_handler)
        def on_request(*args, **kwargs):
            try:
                is_request_valid = self.validator()
            except:
                return self.invalid_handler()

            if is_request_valid:
                return route_handler(*args, **kwargs)
            return self.invalid_handler()
        return on_request

class requires_user_token(base_requirement):
    def validator(self):
        token = None
        if 'user_token' in request.form:
            token = request.form['user_token']
        elif 'user_token' in request.args:
            token = request.args.get('user_token')

        if token:
            this_user = User.get_user(user_token=token)
            if this_user and this_user.token_matches(token):
                return True
        return False

class requires_router_token(base_requirement):
    def validator(self):
        if User.from_router_token():
            return True
        return False
