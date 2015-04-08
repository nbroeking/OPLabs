from util.tests.output import UNDERLINE, INFO, SUCCESS, FAILURE
from .result_tests import get_token, test_new_set, test_start_mobile
from .result_tests import test_post_results, test_get_results

def all_tests():
    print (UNDERLINE(INFO("Running Result Management tests")))

    token = get_token()

    set_id = test_new_set(token) # Creates a Test Set
    mobile_result_id = test_start_mobile(token, set_id) # Creates a Test Result

    test_post_results(token, mobile_result_id) # Updates Result with fake data
    test_get_results(token, mobile_result_id) # Verifies fake data was stored

    print (UNDERLINE(INFO("Result Management tests passed.")))

