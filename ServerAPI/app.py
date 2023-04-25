#!/usr/bin/env python
# encoding: utf-8
import json
import api
from flask import Flask

estado = api.estado
print("state: ", estado)

app = Flask(__name__)
@app.route('/')
def index():
    if estado == 1:
        return json.dumps({'state': '1'})
    else:
        return json.dumps({'state': '0'})
#app.run()
