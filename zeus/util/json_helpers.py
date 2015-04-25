import json
import logging
from flask import request

def __dump_state__():
    try:
        from flask import request
        logging.debug("JSON_FAILURE invoked, current state:")
        logging.debug("POST request arguments: '%s'", str(request.form))
        logging.debug("GET arguments: '%s'", str(request.args))
        logging.debug("Returned JSON: '%s'", str(kwargs))
    except RuntimeError:
        pass

def JSON(**kwargs):
    res = {}
    for kw in kwargs:
        res[kw] = kwargs[kw]
    return json.dumps(res)

def JSON_SUCCESS(**kwargs):
    return JSON(status='success', **kwargs)

def JSON_FAILURE(**kwargs):
    return JSON(status='failure', **kwargs)
