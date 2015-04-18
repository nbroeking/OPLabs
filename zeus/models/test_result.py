"""
    This model represents a single specific test run, and all of the data metrics
    that were collected over the duration of the test.

    Author: Zach Anders
    Date: 02/08/2015
"""

from flask.ext.sqlalchemy import SQLAlchemy
from app import db
from util.models.json_column import JSONBlob, make_json_list
import json
import base64

NETWORK_TYPES = ('IPv4', 'IPv6')

CONNECTION_TYPES = ('wired', 'wireless')

DEVICE_TYPES = ('mobile', 'router', 'desktop')

STATES = ('wait', 'running', 'failed', 'finished')

class TestResult(db.Model):
    __tablename__ = "TestResult"
    test_id = db.Column('test_id', db.Integer, primary_key=True)
    test_token = db.Column('test_token', db.String(44))
    parent_id = db.Column('parent_id', db.ForeignKey('TestSet.set_id'))

    latency_avg = db.Column('latency_avg', db.Float)
    latency_sdev = db.Column('latency_sdev', db.Float)

    dns_response_avg = db.Column('dns_response_avg', db.Float)
    dns_response_sdev = db.Column('dns_response_sdev', db.Float)

    download_throughputs = db.Column('download_throughputs', JSONBlob)
    download_latencies = db.Column('download_latencies', JSONBlob)
    upload_throughputs = db.Column('upload_throughputs', JSONBlob)
    upload_latencies = db.Column('upload_latencies', JSONBlob)
    packet_loss_under_load = db.Column('packet_loss_under_load', db.Float)

    packet_loss = db.Column('packet_loss', db.Float)

    device_type = db.Column('device_type', db.Enum(*DEVICE_TYPES))
    device_name = db.Column('device_name', db.String(64))

    state = db.Column('state', db.Enum(*STATES))
    message = db.Column('message', db.String(64))

    network_type = db.Column('network_type', db.Enum(*NETWORK_TYPES))

    device_ip = db.Column('device_ip', db.String(64))
    interface_stats = db.Column('interface_stats', JSONBlob)
    connection_type = db.Column('connection_type', db.Enum(*CONNECTION_TYPES))

    @staticmethod
    def get_public_columns():
        return {'latency_avg':float,
            'latency_sdev':float,
            'dns_response_avg':float,
            'dns_response_sdev':float,
            'download_throughputs':make_json_list,
            'download_latencies':make_json_list,
            'upload_throughputs':make_json_list,
            'upload_latencies':make_json_list,
            'packet_loss_under_load':float,
            'packet_loss':float,
            'device_name':str,
            'network_type':str,
            'device_ip':str,
            'state':str,
            'message':str,
            'interface_stats':make_json_list,
            'connection_type':str, }

    @staticmethod
    def get_result_by_token_ip(token, ip):
        return db.session.query(TestResult).filter(
                TestResult.test_token == token and
                TestResult.device_ip == ip
                ).first()

    @staticmethod
    def get_result_by_id(result_id):
        return db.session.query(TestResult).filter(
                TestResult.test_id == result_id
                ).first()

    def __init__(self, device_type=None):
        if device_type:
            if device_type not in DEVICE_TYPES:
                raise ValueError("Invalid Device type: %s" % str(device_type))
            self.device_type = device_type

        self.state = 'wait'
        db.session.add(self)

    def delete(self):
        parent = self.parent_set
        db.session.delete(self)
        db.session.commit()

        if len(parent.tests) == 0:
            db.session.delete(parent)
            db.session.commit()

    def exportDict(self):
        return_vals = {}
        for col in self.get_public_columns():
            col_val = getattr(self, col)
            return_vals[col] = col_val
            if isinstance(col_val, list):
                try:
                    return_vals[col+"_avg"] = sum(col_val) / len(col_val)
                except TypeError:
                    # Ignore columns that failed to summarize
                    pass

        return return_vals

    def save(self):
        db.session.commit()

    def update(self, key, data, typeCast=None):
        if typeCast:
            data = typeCast(data)
        setattr(self, key, data)
