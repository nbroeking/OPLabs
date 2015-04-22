from . import mobile_blueprint
from flask import render_template, request, flash, redirect, url_for
from app import db
from models.auth_model import User
from models.test_set import TestSet
from models.test_result import TestResult
from util.web.web_auth import requires_session, csrf_protect
from util.router import Router
from util.json_helpers import JSON_SUCCESS, JSON_FAILURE
from util.rest.rest_auth import requires_user_token
import base64
import logging
import math
from util.jinja.units import KILOBYTES, MEGABYTES, GIGABYTES, BITS

@mobile_blueprint.route('test_set/<set_id>', methods=['GET'])
@requires_user_token()
def set_summary(set_id):
    test_set = TestSet.get_set_by_id(set_id)
    for aset in test_set.tests:
        logging.info("Set: %s, %s,", aset.device_name, aset.download_latencies)

    router = [aset for aset in test_set.tests if aset.device_type == 'router'][0]
    mobile = [aset for aset in test_set.tests if aset.device_type == 'mobile'][0]

    tputs = []
    if router.download_throughputs:
       tputs += router.download_throughputs
    if router.upload_throughputs:
       tputs += router.upload_throughputs
    if mobile.download_throughputs:
       tputs += mobile.download_throughputs
    if mobile.upload_throughputs:
       tputs += mobile.upload_throughputs

    tput_max = max(tputs)
    if tput_max > 2**30:
        tput_units = lambda x: GIGABYTES(BITS(x))
        tput_units_name = "Gigabits"
    elif tput_max > 2**20:
        tput_units = lambda x: MEGABYTES(BITS(x))
        tput_units_name = "Megabits"
    else:
        tput_units = lambda x: KILOBYTES(BITS(x))
        tput_units_name = "Kilobits"

    return render_template('set_summary.html',
            router=router,
            mobile=mobile,
            tput_units=tput_units,
            tput_units_name=tput_units_name,
            )
