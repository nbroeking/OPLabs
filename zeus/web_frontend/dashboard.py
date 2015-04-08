from . import web_blueprint
from flask import render_template, request, flash, redirect, url_for
from app import db
from models.auth_model import User
from models.test_set import TestSet
from models.test_result import TestResult
from util.web.web_auth import requires_session, csrf_protect
from util.router import Router
from util.json_helpers import JSON_SUCCESS, JSON_FAILURE
import base64
import math

@web_blueprint.route('dashboard', methods=['GET'])
@requires_session
def dashboard():
    current_page, page_size = 0, 10
    if 'p' in request.args:
        current_page = int(request.args['p'])-1
        
    user = User.from_session()
    user_sets = TestSet.get_all_user_sets(user)
    num_pages = math.ceil(len(user_sets)/float(page_size))
    pages = range(0, int(num_pages))

    start = current_page*page_size
    end = (current_page+1)*page_size
    user_sets = user_sets[start:end]

    prev_page = max(0, current_page - 1) + 1
    next_page = min(num_pages-1, current_page + 1) + 1
    print current_page, ":", prev_page, next_page

    return render_template('dashboard.html',
            user=user,
            user_sets=user_sets,
            pages=pages,
            current_page=current_page,
            prev_page=prev_page, next_page=next_page,
            )

@web_blueprint.route("dashboard/delete_result", methods=['POST'])
@requires_session
@csrf_protect
def delete_result():
    if User.from_session():
        rec = TestResult.get_result_by_id(request.form['test_id'])
        rec.delete()

        return JSON_SUCCESS()
    else:
        return JSON_FAILURE()

@web_blueprint.route('useredit', methods=['GET'])
@requires_session
def useredit():
    user = User.from_session()
    user_sets = TestSet.get_all_user_sets(user)
    print user_sets

    return render_template('useredit.html',
            user=user,
            user_sets=user_sets)

@web_blueprint.route('admin', methods=['GET'])
@requires_session
def admin():
    user = User.from_session()
    user_sets = TestSet.get_all_user_sets(user)
    print user_sets

    return render_template('admin.html',
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
