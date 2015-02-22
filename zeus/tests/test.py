from auth import auth_tests
from util import Context, FAILURE
import requests
requests.packages.urllib3.disable_warnings()

def run_tests():
    # Run all of the basic API authentication tests
    auth_tests.all_tests()

if __name__ == '__main__':
    if not Context.is_up():
        # We must be on a dev environment
        Context.port = 5000
        Context.setSSL(False)

    if Context.is_up():
        run_tests()
    else:
        print (FAILURE("Could not connect to server!"))
