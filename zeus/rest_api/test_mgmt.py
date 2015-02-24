"""

This module contains all of the APIs for managing speedtests,
including the creation and completion thereof.

Author: Zach Anders
Date: 02/21/2015

"""
from flask import request
from app import db
from common.auth_model import User
from common.test_result import TestResult
from common.auth_util import requires_token
from common.json_util import JSON_SUCCESS, JSON_FAILURE
from . import rest_blueprint

@rest_blueprint.route("/testfunc", methods=['POST'])
@requires_token()
def test_func():
    """ Test function for logged in users. """
    auth_user = User.get_user(auth_token=request.form['token'])

    return JSON_SUCCESS(
        your_email=auth_user.email
        )

@rest_blueprint.route("/start_test", methods=['POST'])
def start_test():
    """ Test function for logged in users. """

    ip = request.remote_addr
    token = request.form['id'].strip()
    print ip, token

    rec = TestResult.get_set_by_token_ip(token, ip)

    rec.state = 'ack'
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

@rest_blueprint.route("/delete_result", methods=['POST'])
def delete_result():
    if User.from_session():
        rec = TestResult.get_set_by_id(request.form['test_id'])
        db.session.delete(rec)
        db.session.commit()
        return JSON_SUCCESS()
    else:
        return JSON_FAILURE()
