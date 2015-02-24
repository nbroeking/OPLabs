"""
    This model represents a single specific test run, and all of the data metrics
    that were collected over the duration of the test.

    Author: Zach Anders
    Date: 02/08/2015
"""

from flask.ext.sqlalchemy import SQLAlchemy
from app import db
import base64

NETWORK_TYPES = ('IPv4', 'IPv6')

CONNECTION_TYPES = ('wired', 'wireless')

DEVICE_TYPES = ('mobile', 'router', 'desktop')

STATES = ('wait', 'ack')

class TestResult(db.Model):
    __tablename__ = "TestResult"
    test_id = db.Column('test_id', db.Integer, primary_key=True)
    test_token = db.Column('test_token', db.String(44))
    parent_id = db.Column('parent_id', db.ForeignKey('TestSet.set_id'))
    latency_avg = db.Column('latency_avg', db.Float)
    latency_sdev = db.Column('latency_sdev', db.Float)

    jitter_avg = db.Column('jitter_avg', db.Float)
    jitter_sdev = db.Column('jitter_sdev', db.Float)

    dns_response_avg = db.Column('dns_response_avg', db.Float)
    dns_response_sdev = db.Column('dns_response_sdev', db.Float)

    throughput_avg = db.Column('throughput_avg', db.Float)
    throughput_sdev = db.Column('throughput_sdev', db.Float)

    packet_loss = db.Column('packet_loss', db.Float)

    device_type = db.Column('device_type', db.Enum(*DEVICE_TYPES))
    device_name = db.Column('device_name', db.String(64))

    state = db.Column('state', db.Enum(*STATES))

    network_type = db.Column('network_type', db.Enum(*NETWORK_TYPES))

    device_ip = db.Column('device_ip', db.String(64))
    connection_type = db.Column('connection_type', db.Enum(*CONNECTION_TYPES))

    @staticmethod
    def get_set_by_token_ip(token, ip):
        token = base64.b64encode(token)
        return db.session.query(TestResult).filter(
                TestResult.test_token == token and
                TestResult.device_ip == ip
                ).first()

    @staticmethod
    def new_anon_result():
        res = TestResult()
        db.session.add(res)
        db.session.commit()
        return res.test_id

    def __init__(self, device_type=None):
        if device_type:
            if device_type not in DEVICE_TYPES:
                raise ValueError("Invalid Device type: %s" % str(device_type))
        state = 'wait'
        db.session.add(self)
        db.session.commit()

    def save(self):
        db.session.commit()
