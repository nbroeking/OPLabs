"""

This module contains all

"""

from flask import request
from app import db
from models.test_result import TestResult
from util.json_helpers import JSON_SUCCESS, JSON_FAILURE
from util.rest.rest_auth import requires_router_token
from . import rest_blueprint
from models.config import TestConfiguration
import json

@rest_blueprint.route("/router/get_config", methods=['POST'])
@requires_router_token()
def get_config():
    ip = request.remote_addr
    token = request.form['router_token'].strip()

    rec = TestResult.get_result_by_token_ip(token, ip)

    if 'data' in request.form:
        data = json.loads(request.form['data'])
        rec.interface_stats = data 

    if not rec:
        return JSON_FAILURE()

    rec.state = 'running'
    db.session.commit()
    rec.save()

    config = TestConfiguration()

    return JSON_SUCCESS(
            config=config.get_config()
        )

@rest_blueprint.route("/router/edit", methods=['POST'])
@requires_router_token()
def edit():
    ip = request.remote_addr
    token = request.form['router_token'].strip()

    rec = TestResult.get_result_by_token_ip(token, ip)

    if not rec:
        return JSON_FAILURE(
                reason="Invalid token or IP."
                )
    if 'data' not in request.form:
        return JSON_FAILURE(
                reason="Missing 'data' parameter"
                )
    # Columns allowed to be updated and their types
    columns = TestResult.get_public_columns()

    data = json.loads(request.form['data'])

    for col in columns:
        if col in data:
            col_type = columns[col]
            datum = col_type(data[col])
            setattr(rec, col, datum)

    ploss_cnt, ploss_len = 0, 0

    # Calculate packet loss from the down/up latencies. (-1 = packet lost)
    if rec.download_latencies:
        ploss_cnt += rec.download_latencies.count(-1)
        ploss_len += len(rec.download_latencies)

    if rec.upload_latencies:
        ploss_cnt += rec.upload_latencies.count(-1)
        ploss_len += len(rec.upload_latencies)

    if ploss_len > 0:
        rec.packet_loss_under_load = float(ploss_cnt) / float(ploss_len)

    rec.save()

    return JSON_SUCCESS()
