from flask import Flask, render_template, request, session, redirect, url_for, flash, jsonify
from . import web_blueprint
from common.auth_model import User

@web_blueprint.route("/")
def home():
    user = User.from_session()
    return render_template("index.html", user=user)
