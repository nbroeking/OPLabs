"""

This module contains all of the APIs for managing speedtests,
including the creation and completion thereof.

Author: Zach Anders
Date: 02/21/2015

"""
from flask import request
from common.auth_model import User
from common.auth_util import requires_token
from common.json_util import JSON_SUCCESS
from . import rest_blueprint

@rest_blueprint.route("/testfunc", methods=['POST'])
@requires_token()
def test_func():
    """ Test function for logged in users. """
    auth_user = User.get_user(auth_token=request.form['token'])

    return JSON_SUCCESS(
        your_email=auth_user.email
        )
