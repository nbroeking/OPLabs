from flask import jsonify, request
from common.auth_model import User
from functools import wraps

INVALID_AUTH = ('{"status":"invalid"}', 401)

class requires_token():
    """ This is a convenience decorator that will ensure the rapped function will
        only be called if the currently active request contains a valid REST token.
        If it does not, it invokes the 'invalid_handler', which by default
        simply returns an HTTP 401 error to the client. """
    def __init__(self, invalid_handler=None):
        if invalid_handler:
            self.invalid_handler = invalid_handler
        else:
            self.invalid_handler = lambda: INVALID_AUTH

    def __call__(self, route_handler):
        @wraps(route_handler)
        def on_request(*args, **kwargs):
            if 'token' in request.form:
                token = request.form['token']
                this_user = User.get_user(auth_token=token)
                if this_user and this_user.token_matches(token):
                    return route_handler(*args, **kwargs)
            return self.invalid_handler()
        return on_request
