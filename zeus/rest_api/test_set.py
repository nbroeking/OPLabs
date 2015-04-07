"""

This module contains the basic CRUD functionality
for remotely managing test sets.

Author: Zach Anders
Date: 04/03/15

"""

from util.rest.rest_auth import requires_user_token
from util.json_helpers import JSON_SUCCESS, JSON_FAILURE
from models.auth_model import User
from models.test_set import TestSet
from . import rest_blueprint

@rest_blueprint.route('/test_set/create', methods=['POST'])
@requires_user_token()
def create_set():
    user = User.from_user_token()
    new_set = TestSet(user)
    new_set.save()
    return JSON_SUCCESS(
            set_id=new_set.set_id,
            )

@rest_blueprint.route('/test_set/<set_id>/delete', methods=['POST'])
@requires_user_token()
def delete_set(set_id=None):
    invalid_set = JSON_FAILURE(reason='Invalid Set')
    if not set_id:
        return invalid_set

    user_set = TestSet.get_set_by_id(set_id)
    if not user_set:
        return invalid_set

    user_set.delete()
    return JSON_SUCCESS()
