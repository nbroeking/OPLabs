import uuid
from flask import Blueprint, render_template, abort, jsonify, request
from jinja2 import TemplateNotFound
from common.auth_model import User
from common.auth_util import requires_token

rest_blueprint = Blueprint('RestAPI', __name__,
    template_folder='')

@rest_blueprint.route("/auth/login", methods=['POST'])
def login():
    """ Called when an API user wishes to log in. """
    invalid_user_code = (jsonify({"status":"invalid"}), 401)
    if 'email' not in request.form or 'password' not in request.form:
        return invalid_user_code

    email = request.form['email']
    submitted_password = request.form['password']
    this_user = User.log_user_in(email, submitted_password)

    if not this_user:
        return invalid_user_code

    res = {
        'auth_token' : this_user.new_token()
    }
    return jsonify(res)

@rest_blueprint.route("/auth/logout", methods=['POST'])
@requires_token()
def logout():
    """ Called when an API user wishes to log out and stop using their token. """
    auth_user = User.get_user(auth_token=request.form['token'])
    auth_user.clear_token()
    return jsonify({"result":"success"})

@rest_blueprint.route("/auth/register", methods=['POST'])
def register():
    """ Called when an API user wants to register a new account into the database. """
    email = request.form['email']
    password = request.form['password']

    User.create_user(email, password)

    return jsonify({"result":"success"})

@rest_blueprint.route("/api/testfunc", methods=['POST'])
@requires_token()
def test_func():
    """ Test function for logged in users. """
    auth_user = User.get_user(auth_token=request.form['token'])

    return jsonify({"result":"Success", 'your_email':auth_user.email})

