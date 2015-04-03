"""

This module contains all of the APIs for managing speedtests,
including the creation and completion thereof.

Author: Zach Anders
Date: 02/21/2015

"""
from flask import request
from app import db
from models.auth_model import User
from models.test_result import TestResult
from util.rest.rest_auth import requires_user_token, requires_router_token
from util.json_helpers import JSON_SUCCESS, JSON_FAILURE
from . import rest_blueprint

@rest_blueprint.route("/testfunc", methods=['POST'])
@requires_user_token()
def test_func():
    """ Test function for logged in users. """
    auth_user = User.get_user(auth_token=request.form['token'])

    return JSON_SUCCESS(
        your_email=auth_user.email
        )

@rest_blueprint.route("/start_test", methods=['POST'])
@requires_router_token()
def start_test():
    ip = request.remote_addr
    token = request.form['id'].strip()

    rec = TestResult.get_result_by_token_ip(token, ip)

    if not rec:
        return JSON_FAILURE()

    rec.state = 'ack'
    db.session.commit()
    rec.save()

    # Ookla IPs / DNS IPs / Ping IPs
    config = {
            'ookla_ips' : ['0.0.0.0', '1.1.1.1'],
            'dns_ips' : ['8.8.8.8', '8.8.4.4'],
            'ping_ips' : ['1.2.3.4', '2.3.4.5']
    }

    return JSON_SUCCESS(
            config=config
        )

@rest_blueprint.route('/update_result', methods=['POST'])
def update_result():
    ip = request.remote_addr
    token = request.form['id'].strip()

    rec = TestResult.get_result_by_token_ip(token, ip)

    if not rec:
        return JSON_FAILURE()

    data = request.form['data']
    try:
        rec.updateFromDict(data)
    except AttributeError, e:
        return JSON_FAILURE(error=e.message)

    return JSON_SUCCESS()
