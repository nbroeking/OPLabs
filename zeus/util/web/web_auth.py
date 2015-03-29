"""

Contains the functional to simplify authentication between a web browser
and the server. Functionality in this module takes advantage of features only
available when request responses cause whole templates to get rendered directly
to the browser.

Author: Zach Anders
Date: 02/21/2015

"""

from flask import request, render_template, flash, session, abort, flash
from models.auth_model import User
from functools import wraps
import base64
import time
import os

def requires_session(some_route):
    # Flask requires that decorators are implemented utilizing functools
    @wraps(some_route)
    def protected(*args, **kwargs):
        if User.from_session():
            return some_route(*args, **kwargs)
        else:
            flash("Please login before continuing.", 'error')
            return render_template("login.html")
    return protected

def csrf_protect(some_route):
    @wraps(some_route)
    def protected(*args, **kwargs):
        # We don't check the timestamp
        good_token = session.pop('_csrf_token', None)
        good_tstamp = session.pop('_csrf_timestamp', None)
        user_token = request.form.get('_csrf_token')

        if good_token:
            if user_token == good_token and time.time() - int(good_tstamp) < (60*5):
                return some_route(*args, **kwargs)

        flash("Session expired, please try again")
        # Otherwise we abort
        abort(403)
    return protected

def generate_csrf_token():
    if '_csrf_token' not in session:
        session['_csrf_token'] = base64.b64encode(os.urandom(64))
    session['_csrf_timestamp'] = str(int(time.time()))

    return session['_csrf_token']

from app import app
app.jinja_env.globals['csrf_token'] = generate_csrf_token
