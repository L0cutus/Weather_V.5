#!/usr/bin/bash
export FLASK_APP=/home/pi/srv/web/app.py

flask run --host=0.0.0.0 --debugger
