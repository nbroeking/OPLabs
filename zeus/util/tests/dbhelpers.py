from models.test_set import TestSet
from util.tests.auth import get_test_user
import base64, os

def initialize_new_set():
    user = get_test_user()
    new_set = TestSet(user)

    router_record = new_set.new_result(device_type="router")
    router_record.test_token = base64.b64encode(os.urandom(32))
    router_record.device_ip = '127.0.0.1'
    router_record.save()
    
    new_set.save()
    return new_set
