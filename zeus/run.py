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

from app import app
import config

from rest_api.controller import rest_blueprint
app.register_blueprint(rest_blueprint, url_prefix="/api")

if __name__ == "__main__":
    app.run()