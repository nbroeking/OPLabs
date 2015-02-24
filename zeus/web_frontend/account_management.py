from . import web_blueprint
from flask import render_template, request, session, redirect, url_for
from common.auth_model import User

@web_blueprint.route('login', methods=['GET', 'POST'])
def loginportal():
    if request.method == 'POST':
        email, pw = request.form['email'], request.form['password']
        if email and pw:
            user = User.log_user_in(email, pw)
            if user:
                session['username'] = email
                return redirect(url_for('/dashboard'))
        return render_template('login.html',
                error="Incorrect Username or Password! Please check and try again.")
    else:
        return render_template('login.html')

@web_blueprint.route('register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        print request.form
        email = request.form['email']
        pw1, pw2 = request.form['password1'], request.form['password2']
        if pw1 != pw2:
            return render_template('register.html',
                    error="Passwords do not match!",
                    email=email)
        elif len(pw1) < 6:
            return render_template('register.html',
                    error="Passwords must be at least six characters!",
                    email=email)

        new_user = User(email, pw1)
        return render_template('login.html',
                alert='Account successfully created.')
    else:
       return render_template('register.html')
