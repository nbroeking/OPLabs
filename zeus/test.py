from tests.auth import auth_tests
from tests.test_mgmt import result_tests
from util.tests.context import Context
from util.tests.output import FAILURE

import requests
requests.packages.urllib3.disable_warnings()

def run_tests():
    # Run all of the basic API authentication tests
    #auth_tests.all_tests()
    result_tests.all_tests()

if __name__ == '__main__':
    if not Context.is_up():
        # We must be on a dev environment
        Context.port = 5000
        Context.setSSL(False)

    if Context.is_up():
        run_tests()
    else:
        print (FAILURE("Could not connect to server!"))
