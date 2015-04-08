from flask import render_template
from . import web_blueprint
from models.auth_model import User

@web_blueprint.route("/")
def home():
    user = User.from_session()
    return render_template("index.html", user=user)
