#!/usr/bin/env python

"""

Simple utility script to import all of our database models and then get
SQLAlchemy to actually define our schema in the current database.

Author: Zach Anders
Date: 02/03/2015
"""

# Import SQLAlchemy
from app import db

# Import all of the models we want to define
from models.auth_model import User
from models.test_set import TestSet
from models.test_result import TestResult

# Get SQLALchemy to create them
db.create_all()
