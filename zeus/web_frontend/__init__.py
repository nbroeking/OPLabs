"""
This package contains all of the routes and controller functions
that are used in the OPLabs Web Interface.

Author: Zach Anders
Date: 02/21/2015

"""

from flask import Blueprint

# Defines the request blueprint used by the rest of the API
web_blueprint = Blueprint('WebInterface',
                          __name__,
                          template_folder='templates',
                          static_folder='static',
                          static_url_path='static')

from .public import home
from .account_management import loginportal, register
from .dashboard import dashboard

from app import app
from util.jinja.units import KILOBYTES, MEGABYTES, GIGABYTES, BITS, PERCENT
from util.jinja.aggregate import average, round_float

app.jinja_env.globals['average'] = average
app.jinja_env.globals['round'] = round_float
app.jinja_env.globals['KILOBYTES'] = KILOBYTES
app.jinja_env.globals['MEGABYTES'] = MEGABYTES
app.jinja_env.globals['GIGABYTES'] = GIGABYTES
app.jinja_env.globals['BITS'] = BITS
app.jinja_env.globals['PERCENT'] = PERCENT
