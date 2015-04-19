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

def average(*iterables):
    running_sum, running_len = 0, 0
    for lst in iterables:
        if lst:
            running_sum += sum(lst)
            running_len += len(lst)
    if running_len == 0:
        return "None"
    return running_sum / running_len

app.jinja_env.globals['average'] = average
