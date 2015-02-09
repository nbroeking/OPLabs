from flask import Blueprint, request
from common.auth_model import User
from common.auth_util import requires_token
from common.json_util import JSON_SUCCESS, JSON_FAILURE

rest_blueprint = Blueprint('RestAPI', __name__,
                           template_folder='')

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
@requires_token()
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

    User.create_user(email, password)

    return JSON_SUCCESS()

@rest_blueprint.route("/testfunc", methods=['POST'])
@requires_token()
def test_func():
    """ Test function for logged in users. """
    auth_user = User.get_user(auth_token=request.form['token'])

    return JSON_SUCCESS(
        your_email=auth_user.email
        )
