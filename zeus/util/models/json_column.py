"""

    This is module contains a simple type decorator that can be used
    to wrap columns which contains JSON. This ensures the column is
    automatically serialized/deserialized whenever the record is read/saved.

    Author: Zach Anders
    Date: 04/17/2015
"""

from app import db
import json

def make_json_list(value):
    # Try to coerce a string into a list
    if not isinstance(value, list) or not isinstance(value, dict):
        value = json.loads(value)

    # If it is still not in a list, put it in a one item list
    if not isinstance(value, list):
        value = [value]

    return value

class JSONBlob(db.TypeDecorator):
    # LargeBinary is translated to Blob/BArray on most dbs
    impl = db.LargeBinary

    def process_bind_param(self, value, _):
        return json.dumps(value)

    def process_result_value(self, value, _):
        return json.loads(value)
