"""

This module contains all of the APIs related to the registration
of users. Most clients will start here because all of the interesting
APIs can only be used with an auth_token, and auth_tokens are only
obtained through the /auth/login/ API.

Author: Zach Anders
Date: 02/21/2015

"""
from flask import request
from models.auth_model import User
from util.rest.rest_auth import requires_user_token
from util.rest.json_helpers import JSON_SUCCESS, JSON_FAILURE
from . import rest_blueprint

@rest_blueprint.route("/auth/login", methods=['POST'])
def login():
    """ Called when an API user wishes to log in. """
    if 'email' not in request.form or 'password' not in request.form:
        return (JSON_FAILURE(), 401)

    email = request.form['email']
    submitted_password = request.form['password']
    this_user = User.log_user_in(email, submitted_password)

    if not this_user:
        return (JSON_FAILURE(), 401)

    return JSON_SUCCESS(
        auth_token=this_user.new_token()
        )

@rest_blueprint.route("/auth/logout", methods=['POST'])
@requires_user_token()
def logout():
    """ Called when an API user wishes to log out and stop using their token. """
    auth_user = User.get_user(auth_token=request.form['token'])
    auth_user.clear_token()
    return JSON_SUCCESS()

@rest_blueprint.route("/auth/register", methods=['POST'])
def register():
    """ Called when an API user wants to register a new account into the database. """
    email = request.form['email']
    password = request.form['password']

    new_user = User(email, password)
    new_user.save()

    return JSON_SUCCESS()
