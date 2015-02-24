import json

def JSON(**kwargs):
    res = {}
    for kw in kwargs:
        res[kw] = kwargs[kw]
    return json.dumps(res)

def JSON_SUCCESS(**kwargs):
    return JSON(status='success', **kwargs)

def JSON_FAILURE(**kwargs):
    return JSON(status='failure', **kwargs)
