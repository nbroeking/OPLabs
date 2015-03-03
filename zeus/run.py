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

from rest_api import rest_blueprint
from web_frontend import web_blueprint

app.register_blueprint(rest_blueprint, url_prefix="/api")
app.register_blueprint(web_blueprint, url_prefix="/")

if __name__ == "__main__":
    app.run()
