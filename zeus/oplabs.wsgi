# Virtualenv is optional, you can comment this if you install packages systemwide
venv_activation = '/var/<pathtoenv>/activate_this.py'

import sys, os

execfile(venv_activation, dict(__file__=venv_activation))

sys.path.insert(0, '/var/<pathtocode>/zeus/')

from run import app as application
