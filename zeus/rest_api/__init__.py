"""
This package contains all of the routes and controller functions
that makes up the OPLabs REST API. 

Unless otherwise noted, it can be assumed that all REST APIs
return JSON.

Author: Zach Anders
Date: 02/21/2015

"""

from flask import Blueprint

# Defines the request blueprint used by the rest of the API
rest_blueprint = Blueprint('RestAPI',
                           __name__,
                           template_folder='')

from .auth_api import *
from .test_mgmt import *
from .test_result import *
from .test_set import *
