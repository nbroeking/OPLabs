from . import web_blueprint
from flask import render_template, request, session, redirect, url_for
from common.auth_model import User

@web_blueprint.route('login', methods=['GET', 'POST'])
def loginportal():
    if request.method == 'POST':
        email, pw = request.form["email"], request.form["password"]
        if email and pw:
            user = User.log_user_in(email, pw)
            if user:
                session['username'] = email
                return redirect(url_for('/dashboard'))
        return render_template('login.html',
                error="Incorrect Username or Password! Please check and try again.")
    else:
        return render_template('login.html')
