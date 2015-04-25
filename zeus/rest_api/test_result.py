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
import logging
from . import rest_blueprint

@rest_blueprint.route('/test_result/<result_id>/edit', methods=['POST'])
@requires_user_token()
def edit_result(result_id=None):
    logging.info("Received API edit request for result_id: {%s}", result_id)
    logging.debug("POST params: %s", request.form)
    if not result_id: 
        logging.warning("Receieved invalid result_id {%s}", result_id)
        return JSON_FAILURE(reason='Invalid Result')

    result = TestResult.get_result_by_id(result_id)
    if not result:
        logging.warning("Could not find result in db (result_id={%s}", result_id)
        return JSON_FAILURE(reason='Invalid Result')

    logging.debug("Setting device IP to {%s}", str(request.remote_addr))
    result.device_ip = str(request.remote_addr)

    # Columns allowed to be updated and their types
    columns = TestResult.get_public_columns()

    for col in columns:
        if col in request.form:
            col_type = columns[col]
            logging.debug("Parsing column in request, {'%s' = '%s', type=%s}",
                    str(col), str(request.form[col]), str(col_type))
            datum = col_type(request.form[col])
            setattr(result, col, datum)

    # Request from Nic
    if "throughput_latency" in request.form:
        result.download_latencies = [float(request.form["throughput_latency"])]
        result.upload_latencies = [float(request.form["throughput_latency"])]
            
    result.save()

    return JSON_SUCCESS()

@rest_blueprint.route('/test_result/<result_id>/status', methods=['GET'])
@requires_user_token()
def result_status(result_id=None):
    if not result_id: 
        return JSON_FAILURE(reason='Invalid Result')

    result = TestResult.get_result_by_id(result_id)
    if not result:
        return JSON_FAILURE(reason='Invalid Result')

    return JSON_SUCCESS(
            state=result.state
            )

@rest_blueprint.route('/test_result/<result_id>', methods=['GET', 'POST'])
@requires_user_token()
def get_result(result_id=None):
    logging.info("Retrieving result for result_id %s", result_id)
    if not result_id: 
        return JSON_FAILURE(reason='Invalid Result')

    result = TestResult.get_result_by_id(result_id)
    if not result:
        return JSON_FAILURE(reason='Invalid Result')

    return JSON_SUCCESS(
            **result.exportDict()
            )

