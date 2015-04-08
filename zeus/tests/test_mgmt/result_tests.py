from models.test_result import TestResult
from util.tests.output import UNDERLINE, INFO, SUCCESS, FAILURE, truncate_repr
from util.tests.context import check_status
from util.tests.post import do_post
from util.tests.auth import login

def assert_valid_config(config):
    # Top Level
    assert 'dns_config' in config, 'Missing dns_config in configuration!'
    assert 'throughput_config' in config, 'Missing throughput_config in configuration!'

    # Inside dns_config
    dns_conf = config['dns_config']
    assert 'valid_names' in dns_conf, 'Missing valid_names in dns_config!'
    assert 'invalid_names' in dns_conf, 'Missing invalid_names in dns_config'
    assert 'dns_server' in dns_conf, 'Missing dns_server in dns_config!'
    assert 'timeout' in dns_conf, 'Missing timeout in dns_config'

    # Inside throughput_config
    thr_conf = config['throughput_config']
    assert 'server_ip' in thr_conf, 'Missing server_ip in throughput_config'


def test_start_mobile(token, set_id):
    params = {'user_token':token, 
            'set_id':set_id,
            }

    resp = do_post('/api/start_test/mobile', params)
    check_status(resp, fields=['result_id', 'config'])

    js = resp.json()
    assert_valid_config(js['config'])

    print (SUCCESS("Start mobile test",
            conf=truncate_repr(js['config']),
            result_id=js['result_id']
        )) 
    return js['result_id'] #, js['config']

def test_post_results(token, result_id):
    params = {'user_token':token,
            }
    params.update(get_example_data())

    resp = do_post('/api/test_result/%s/edit' % str(result_id), params)
    check_status(resp, fields=[])

    print (SUCCESS("Post Test Results data"))

def test_get_results(token, result_id):
    params = {'user_token':token,
            }
    params.update(get_example_data())

    resp = do_post('/api/test_result/%s' % str(result_id), params)
    check_status(resp, fields=TestResult.get_public_columns().keys())
    data = get_example_data()
    json = resp.json()
    def validate_col(column, actual, received):
        mesg = """
        Column {col} incorrect. Expecting '{exp}' Received '{recv}'
        """.strip().format(
                col=column, exp=actual, recv=received)
        assert str(actual) == str(received), FAILURE(mesg)

    for col in data:
        validate_col(col, data[col], json[col])

    print (SUCCESS("Retrieve Test Results data",
        data=truncate_repr(data),
        ))

    return resp.json()

def get_token():
    resp = login()
    check_status(resp, fields=['user_token'])
    return resp.json()['user_token']

def test_new_set(token):
    params = {'user_token':token, 
            }

    resp = do_post('/api/test_set/create', params)
    check_status(resp, fields=['set_id'])

    set_id = resp.json()['set_id']

    print (SUCCESS("Create new Test Set",
        set_id=set_id,
        ))

    return set_id

def get_example_data(overrides=None):
    default = {'latency_avg':20.0,
            'latency_sdev':1.25,
            'jitter_avg':1.11,
            'jitter_sdev':.10,
            'dns_response_avg':30.0,
            'dns_response_sdev':1.22,
            'throughput_avg':50.0,
            'throughput_sdev':15.0,
            'packet_loss':.01,
            'device_name':'Test harness',
            'network_type':'IPv4',
            'device_ip':'0.1.2.3',
            'state':'running',
            'connection_type':'wired', }
    if overrides:
        default.update(overrides)
    return default

