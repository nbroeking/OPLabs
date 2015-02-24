from . import web_blueprint
from flask import render_template, request, flash, redirect, url_for
from common.auth_model import User
from common.auth_util import requires_session
from common.test_set import TestSet
from common.router import Router
import base64

@web_blueprint.route('dashboard', methods=['GET'])
@requires_session
def dashboard():
    user = User.from_session()
    user_sets = TestSet.get_all_user_sets(user)

    return render_template('dashboard.html',
            user=user,
            user_sets=user_sets)

@web_blueprint.route('tests', methods=['GET'])
@requires_session
def showtests():
    user = User.from_session()

    return render_template('showtests.html', user=user)

@web_blueprint.route('start_router_test', methods=['POST'])
@requires_session
def start_router_test():
    user = User.from_session()
    ip = request.form['ipaddr']

    new_set = TestSet(user)

    # This manages the actual work
    router = Router(ip)

    router_record = new_set.new_result(device_type="router")
    router_record.test_token = base64.b64encode(router.req_id)
    router_record.device_ip = ip
    router_record.save()
    
    new_set.save()

    try:
        router.wakeup()
        flash("Your test has been initiated")
    except:
        flash("Test failed to be initiated!", 'error')


    #return render_template('showtests.html', user=user, error=error)
    return redirect(url_for("WebInterface.showtests"))
