from flask import Flask, render_template, request, session, redirect, url_for, flash, jsonify
from . import web_blueprint

@web_blueprint.route("/")
def home():
    print session
    return render_template("index.html")
