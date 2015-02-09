"""
Authentication tests related to logging in and out.
"""

from .auth_util import login, logout
from util import SUCCESS, FAILURE, check_status, Context

def test_valid_login():
    """ Accounts with valid user/pass should be able to login and 
        get an auth_token. """
    resp = login()
    check_status(resp, fields=['auth_token'])

    json = resp.json()

    token = json['auth_token']

    assert token != None and token != '', FAILURE("Login returned empty auth_token!")

    assert len(token) > 64, FAILURE("Auth_token too small?")

    print SUCCESS("Valid login test ({user}, {passwd})".format(
        user=Context.email,
        passwd=Context.passwd))

    return token

def test_logout(token):
    """ An authenticated token should be able to log itself out, removing it
        from the database. """
    resp = logout(token)
    check_status(resp, fields=['status'])

    assert resp.json()['status'] == 'success', \
        FAILURE("Logout did not return success!")

    print SUCCESS("Logout test")

def test_invalid_login(email="fakeemail", pw="INVALIDPASSWORD"):
    """ Invalid credentials should not allow a user to log in. """
    resp = login(email=email, pw=pw)
    check_status(resp, code=401, fields=['status'])

    print SUCCESS("Invalid login test ({user}, {passwd})".format(
        user=email,
        passwd=pw))

