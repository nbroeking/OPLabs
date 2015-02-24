"""
    This model represents a single specific test run, and all of the data metrics
    that were collected over the duration of the test.

    Author: Zach Anders
    Date: 02/08/2015
"""

from flask.ext.sqlalchemy import SQLAlchemy
from app import db

NETWORK_TYPES = ('IPv4', 'IPv6')

CONNECTION_TYPES = ('wired', 'wireless')

DEVICE_TYPES = ('mobile', 'router', 'desktop')

class TestResult(db.Model):
    __tablename__ = "TestResult"
    test_id = db.Column('test_id', db.Integer, primary_key=True)
    test_token = db.Column('test_token', db.String(32))
    parent_id = db.Column('parent_id', db.ForeignKey('test_set.set_id'))
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

    network_type = db.Column('network_type', db.Enum(*NETWORK_TYPES))

    device_ip = db.Column('device_ip', db.String(64))
    connection_type = db.Column('connection_type', db.Enum(*CONNECTION_TYPES))

    @staticmethod
    def get_set_by_id(req_id):
        return db.session.query(TestResult).filter(TestResult.test_token == req_id).all()

    @staticmethod
    def new_anon_result():
        res = TestResult()
        db.session.add(res)
        db.session.commit()
        return res.test_id

