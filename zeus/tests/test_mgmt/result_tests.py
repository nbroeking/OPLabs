from models.test_result import TestResult
from util.tests.output import SUCCESS, FAILURE, truncate_repr
from util.tests.context import check_status
from util.tests.post import do_post
from util.tests.auth import login
import json

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
    print token
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

def validate_col(column, actual, received):
    mesg = """
    Column {col} incorrect. Expecting '{exp}' Received '{recv}'
    """.strip().format(
            col=column, exp=actual, recv=received)
    assert actual == received, FAILURE(mesg)

def test_post_results(token, result_id, **overrides):
    params = {'user_token': token}
    params.update(get_example_data(overrides))
    for key, val in params.items():
        if isinstance(val, list):
            params[key] = json.dumps(params[key])

    resp = do_post('/api/test_result/%s/edit' % str(result_id), params)
    check_status(resp, fields=[])

    print (SUCCESS("Post Test Results data"))

def test_get_results(token, result_id, **overrides):
    params = {'user_token':token,
            }
    extra_columns = [
            'download_throughputs_avg',
            'download_latencies_avg',
            'upload_throughputs_avg',
            'upload_latencies_avg',
            ]

    resp = do_post('/api/test_result/%s' % str(result_id), params)
    check_status(resp,
            fields=TestResult.get_public_columns().keys() + extra_columns)
    data = get_example_data(overrides)
    json_resp = resp.json()

    for col in data:
        validate_col(col, data[col], json_resp[col])

    print (SUCCESS("Retrieve Test Results data",
        data=truncate_repr(data, priority=overrides),
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
    def mkjs(arg):
        return json.dumps(arg)
    default = {'latency_avg':20.0,
            'latency_sdev':1.25,
            'dns_response_avg':30.0,
            'dns_response_sdev':1.22,
            'packet_loss':.01,
            'download_throughputs':[0, 1, 2, 3, 4],
            'download_latencies':[10, 15, 20, 25],
            'upload_throughputs':[0, 1, 2, 3, 4],
            'upload_latencies':[10, 15, 20, 25],
            'packet_loss_under_load':.05,
            'device_name':'Test harness',
            'network_type':'IPv4',
            'device_ip':'0.1.2.3',
            'state':'running',
            'message':'This is my example message.',
            'connection_type':'wired',
            'interface_stats':[
                {
                    'intf':'eth0',
                    'rx_bytes': 200,
                    'tx_bytes': 100,
                    'timestamp': 0,
                },
                {
                    'intf':'eth1',
                    'rx_bytes': 2**30,
                    'tx_bytes': 2**24,
                    'timestamp': 1,
                },
                {
                    'intf':'eth1',
                    'rx_bytes': 200,
                    'tx_bytes': 100,
                    'timestamp': 2,
                },
            ],
            }
    if overrides:
        default.update(overrides)
    return default

