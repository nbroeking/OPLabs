from util.tests.context import Context
from common.auth_model import User
from util.tests.post import do_post
import os, base64

def get_test_user():
    user = User.get_user(email='test')
    if not user:
        user = User('test', base64.b64encode(os.urandom(16)))
    return user

def login(email=None, pw=None):
    """ Attempt to login. Defaults to the special testing username and password specified
        in util/tests/context.py. """
    if email == None:
        email = Context.email
    if pw == None:
        pw = Context.passwd
    return do_post('/api/auth/login', {'email':email, 'password':pw})

def create_user(email=None, pw=None):
    """ Create a user with the given email and password.
        Defaults to the testing account. """
    email = email or Context.email
    pw = pw or Context.passwd
    return do_post('/api/auth/register', {'email':email, 'password':pw})

def logout(token):
    """ Attempt to logout the user who is identified by the given token. """
    return do_post('/api/auth/logout', {"token":token})

def testfunc(token):
    """ Make a call to the api testfunc. Require a valid authentication token."""
    return do_post('/api/testfunc', {"token":token})
