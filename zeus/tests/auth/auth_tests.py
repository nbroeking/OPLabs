"""
Contains tests related to the authentication API.

Currently contains:
    test_login - Tests /api/auth/login
    test_logout - Tests /api/auth/logout
    test_authenticated - Tests an authenticated method

"""
from .auth_util import testfunc
from util import UNDERLINE, INFO, SUCCESS, FAILURE, check_status, Context
from .login_tests import test_valid_login, test_logout, test_invalid_login

def test_authenticated(token):
    """ An authenticated token should be able to call methods that require
        authentication. """
    resp = testfunc(token)
    check_status(resp, fields=['status', 'your_email'])

    json = resp.json()
    assert json['status'] == 'success', \
        FAILURE("testfunc did not return status:success!", token=token)

    assert json['your_email'] == Context.email, \
        FAILURE("testfunc returned wrong email!", token=token)

    print (SUCCESS("Authenticated method", token=token))

def test_unauthenticated(token="asdfdsasdf"):
    """ An invalid token should not be able to call authenticated methods. """
    resp = testfunc(token)
    check_status(resp, code=401, fields=['status'])

    print (SUCCESS("Unauthenticated method", token=token))

def all_tests():
    print (UNDERLINE(INFO("Running Basic Authentication tests")))
    # Log user in
    user_token = test_valid_login()

    # Test user is properly authenticated in database
    test_authenticated(user_token)

    # Make sure invalid token does not work
    test_unauthenticated(token='asdfdsadf')

    # Log user out, deleting their token
    test_logout(user_token)

    # Make sure the expired token no longer works
    test_unauthenticated(user_token)
    test_unauthenticated("")

    # Make sure invalid users do not get auth_tokens
    test_invalid_login()
    test_invalid_login(email='', pw='')
    test_invalid_login(email=Context.email, pw='wrongpassword')
    test_invalid_login(email="wrongemail", pw=Context.passwd)

    print (UNDERLINE(INFO("Basic Authentication tests passed.")))

if __name__ == '__main__':
    all_tests()
