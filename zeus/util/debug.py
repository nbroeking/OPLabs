from flask import request
import logging

def before_request():
    logging.debug("POST PARAMS: " + str(request.form))
    logging.debug("GET PARAMS: " + str(request.args))
