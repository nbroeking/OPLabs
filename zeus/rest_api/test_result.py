"""

This module contains the basic CRUD functionality
for remotely managing test results.

Author: Zach Anders
Date: 04/06/15

"""

from flask import request
from models.test_result import TestResult
from util.json_helpers import JSON_SUCCESS, JSON_FAILURE
from util.rest.rest_auth import requires_user_token
from . import rest_blueprint

@rest_blueprint.route('/test_result/<result_id>/edit', methods=['POST'])
@requires_user_token()
def edit_result(result_id=None):
    invalid_res = JSON_FAILURE(reason='Invalid Result')
    if not result_id: 
        return invalid_res

    result = TestResult.get_result_by_id(result_id)
    result.device_ip = str(request.remote_addr)
    if not result:
        return invalid_res

    # Columns allowed to be updated and their types
    columns = TestResult.get_public_columns()

    for col in columns:
        if col in request.form:
            col_type = columns[col]
            datum = col_type(request.form[col])
            setattr(result, col, datum)
            
    result.save()

    return JSON_SUCCESS()

@rest_blueprint.route('/test_result/<result_id>/status', methods=['GET'])
@requires_user_token()
def result_status(result_id=None):
    invalid_res = JSON_FAILURE(reason='Invalid Result')
    if not result_id: 
        return invalid_res

    result = TestResult.get_result_by_id(result_id)
    if not result:
        return invalid_res
    return JSON_SUCCESS(
            state=result.state
            )

@rest_blueprint.route('/test_result/<result_id>', methods=['GET', 'POST'])
@requires_user_token()
def get_result(result_id=None):
    invalid_res = JSON_FAILURE(reason='Invalid Result')
    if not result_id: 
        return invalid_res

    result = TestResult.get_result_by_id(result_id)
    if not result:
        return invalid_res

    columns = TestResult.get_public_columns()

    return_vals = {}
    for col in columns:
        return_vals[col] = getattr(result, col)

    return JSON_SUCCESS(
            **return_vals
            )

