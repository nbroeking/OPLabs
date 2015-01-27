#!/usr/bin/env python

"""
This module contains all of the models related to user
authentication and administration.

Author: Zach Anders
Date: 01/27/2015
"""

from flask.ext.sqlalchemy import SQLAlchemy
from app import db

class User(db.Model):
    """ 
    This model represents a user in the database.
    """
    __tablename__ = "auth_user"
    id = db.Column('user_id', db.Integer, primary_key=True)
    username = db.Column('username', db.String(20), unique=True, index=True)
    email = db.Column('email', db.String(50), unique=True, index=True)
    password = db.Column('password', db.String(10))
    permissions = db.Column('permissions', db.String(50))
 
    def __init__(self, username, password, email):
        """
        Creates a new User. 
        """
        self.username = username
        self.password = password
        self.email = email
