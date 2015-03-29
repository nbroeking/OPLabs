"""

This module contains all of the basic components
you would need to generate a new testing script or 
suite.

Author: Zach Anders
Date: 03/26/15

"""

import requests
from util.tests.context import Context

def do_post(url, params):
    return requests.post(Context.make_url(url), data=params, verify=False)
