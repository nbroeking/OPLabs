from . import web_blueprint
from flask import render_template, request, session, redirect, url_for, flash
from models.auth_model import User
from util.web.web_auth import requires_session

@web_blueprint.route('login', methods=['GET', 'POST'])
def loginportal():
    if request.method == 'POST':
        email, pw = request.form['email'], request.form['password']
        if email and pw:
            user = User.log_user_in(email, pw)
            if user:
                session['email'] = email
                return redirect(url_for('WebInterface.dashboard'))

        flash("Incorrect Username or Password! Please check and try again.", 'error')
        return render_template('login.html')
    else:
        return render_template('login.html')

@web_blueprint.route('register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        email = request.form['email']
        pw1, pw2 = request.form['password1'], request.form['password2']

        if User.get_user(email=email):
            flash("That email is already taken!", 'error')
            return render_template('register.html')
        elif len(pw1) < 6:
            flash("Passwords must be at least six characters!", 'error')
            return render_template('register.html',
                    email=email)
        elif pw1 != pw2:
            flash("Passwords do not match!", 'error')
            return render_template('register.html',
                    email=email)

        _ = User(email, pw1)
        flash("Account successfully created.", 'success')
        return render_template('login.html')
    else:
       return render_template('register.html')

@web_blueprint.route('logout', methods=['GET'])
@requires_session
def logout():
    del session['email']
    return redirect(url_for('WebInterface.loginportal'))
