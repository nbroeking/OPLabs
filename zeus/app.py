"""

app.py sets up the global context and configuration for the web server.

This is where the database and other WebApp settings are configured.

Author: Zach Anders
Date: 02/03/2015

"""
from flask import Flask
from flask.ext.sqlalchemy import SQLAlchemy
import config
import os
from util.throughput_test import start_server


app = Flask(__name__, static_url_path='/noop/')
app.config['SQLALCHEMY_DATABASE_URI'] = config.DATABASE_URI
app.debug = config.DEBUG
db = SQLAlchemy(app)

THROUGHPUT_IP, THROUGHPUT_PORT = start_server()
print THROUGHPUT_PORT

if app.debug:
    # This makes debugging a lot easier. Otherwise the current session gets
    # invalidated every time flask reloads itself.
    app.secret_key = 'debug'
else:
    app.secret_key = os.urandom(128)
