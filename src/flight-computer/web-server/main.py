#!/usr/bin/python3

import flask
import api
from gfs import GFS

gfs = GFS()

app = flask.Flask(__name__)

api.add_handlers(app, gfs)
api.add_pages(app)

base = flask.Blueprint('base', __name__, template_folder='templates')
app.register_blueprint(base)

@app.route("/")
def index():
    return flask.render_template('data.html')

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8672)