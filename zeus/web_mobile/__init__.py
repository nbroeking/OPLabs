"""
This package contains all of the routes and controller functions
that are used in the OPLabs mobile interface.

Author: Zach Anders
Date: 04/19/2015

"""

from flask import Blueprint

# Defines the request blueprint used by the rest of the API
mobile_blueprint = Blueprint('WebMobile',
                          __name__,
                          template_folder='templates',
                          static_folder='static',
                          static_url_path='static')

from .set_summary import set_summary
