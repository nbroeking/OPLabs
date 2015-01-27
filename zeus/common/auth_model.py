from flask.ext.sqlalchemy import SQLAlchemy

class User(db.Model):
    __tablename__ = "auth_user"
    id = db.Column('user_id', db.Integer, primary_key=True)
    username = db.Column('username', db.String(20), unique=True, index=True)
    password = db.Column('password', db.String(10))
    email = db.Column('email', db.String(50), unique=True, index=True)
 
    def __init__(self, username, password, email):
        self.username = username
        self.password = password
        self.email = email
