import uuid
from flask import Blueprint, render_template, abort, jsonify
from jinja2 import TemplateNotFound

rest_blueprint = Blueprint('RestAPI', __name__,
    template_folder='')

@rest_blueprint.route("/auth/login", methods=['POST'])
def login():
    res = {
        'auth_token' : uuid.uuid4()
    }
    return jsonify(res)
