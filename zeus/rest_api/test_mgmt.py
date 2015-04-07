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
from models.test_set import TestSet
from models.config import TestConfiguration
from util.rest.rest_auth import requires_user_token, requires_router_token
from util.json_helpers import JSON_SUCCESS, JSON_FAILURE
from util.router import Router
from . import rest_blueprint
import base64

@rest_blueprint.route("/testfunc", methods=['POST'])
@requires_user_token()
def test_func():
    """ Test function for logged in users. """
    auth_user = User.get_user(auth_token=request.form['token'])

    return JSON_SUCCESS(
            your_email=auth_user.email
            )


@rest_blueprint.route("/start_test/<test_type>", methods=['POST'])
@requires_user_token()
def start_test(test_type=None):
    if 'set_id' not in request.form:
        return JSON_FAILURE(reason='Missing set_id')

    set_id = request.form['set_id'].strip()
    test_set = TestSet.get_set_by_id(set_id)

    if not test_set:
        return JSON_FAILURE(reason='Invalid set_id')
    if not test_type:
        return JSON_FAILURE(reason='Invalid test type')

    config = TestConfiguration()
    conf_json = config.get_config()

    if test_type == 'mobile':
        # Create a new mobile TestResult
        result = test_set.new_result(device_type="mobile")

        # Commit to database
        test_set.save()
        result.save()
        
        # Return config + new result_id
        return JSON_SUCCESS(
                config=conf_json,
                result_id=result.test_id,
                )
    elif test_type == 'router':
        # Create a new Router TestResult
        result = test_set.new_result(device_type="router")

        # Get IP of connected client. This should be the address for the router
        ip = request.remote_addr
        router = Router(ip)
        result.test_token = base64.b64encode(router.req_id)
        result.device_ip = ip

        result.save()
        test_set.save()

        # Send a magic packet to the router
        router.wakeup()

        # Return config + new result_id
        return JSON_SUCCESS(
                config=conf_json,
                result_id=result.test_id,
                )
    return JSON_FAILURE(
            reason="Invalid test type!"
            )

@rest_blueprint.route("/router/get_config", methods=['POST'])
@requires_router_token()
def get_config():
    ip = request.remote_addr
    token = request.form['id'].strip()

    rec = TestResult.get_result_by_token_ip(token, ip)

    if not rec:
        return JSON_FAILURE()

    rec.state = 'ack'
    db.session.commit()
    rec.save()

    config = TestConfiguration()

    return JSON_SUCCESS(
            config=config.get_config()
        )
