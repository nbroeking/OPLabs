import requests

HOSTNAME = 'http://localhost:5000'

def do_post(url, params):
    return requests.post(HOSTNAME + url, data=params)

def login(email, pw):
    return do_post('/auth/login', {'email':email, 'password':pw})

def create_user(email, pw):
    return do_post('/auth/register', {'email':email, 'password':pw})

def logout(token):
    return do_post('/auth/logout', {"token":token})

def testfunc(token):
    return do_post('/api/testfunc', {"token":token})

if __name__ == '__main__':
    creds = ("testuser@somewhere.com", '1234')
    resp = login(*creds)
    if resp.status_code == 200:
        print "Logged in!"
    else:
        print "Failed to log in, attempting to create account."
        create_user(*creds)
        resp = login(*creds)
    token = resp.json()['auth_token']
    print "Got token:", token

    print "POSTing testfunc"
    print testfunc(token).json()

    print "Logging out..."
    logout(token)


