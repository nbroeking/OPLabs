#!/usr/bin/env python
"""

run.py makes it easy to start Flask's built-in development webserver.

To start a development server, ensure all the necessary packages are
installed: (I suggest setting up a Virtualenv for this)

$ pip install -r requirements.txt
< ... >
$ python run.py
 * Running on http://127.0.0.1:5000/
 * Restarting with reloader

Author: Zach Anders
Date: 01/26/2015

"""

from flask import Flask
from flask.ext.sqlalchemy import SQLAlchemy
import config
import os


app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = config.DATABASE_URI
db = SQLAlchemy(app)

from rest_api.controller import rest_blueprint
app.register_blueprint(rest_blueprint)

if __name__ == "__main__":
    app.debug = config.DEBUG
    if app.debug:
        # This makes debugging a lot easier. Otherwise the current session gets
        # invalidated every time flask reloads itself.
        app.secret_key = 'debug'
    else:
        app.secret_key = os.urandom(128)

    app.run()
