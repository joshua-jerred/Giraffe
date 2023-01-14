import flask
from gfs import GFS
from flask import after_this_request

def add_pages(app):
    @app.route("/config")
    def config():
        config = flask.render_template('config.html')
        return config

def add_handlers(app, gfs):
    @app.route('/api/connect', methods=['GET'])
    def connect():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response

        connected = gfs.connect()
        return flask.jsonify(connected=connected)

    @app.route('/api/disconnect', methods=['GET'])
    def disconnect():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response

        connected = gfs.disconnect()
        return flask.jsonify(connected=connected)

    @app.route('/api/get-connection-status', methods=['GET'])
    def get_connection_status():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response

        connected = gfs.getConnection()
        connection_uptime = gfs.getConnectionUptime()
        return flask.jsonify(connected=connected, connection_uptime=connection_uptime)

    @app.route('/api/get-connection-uptime', methods=['GET'])
    def get_uptime():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response

        uptime = gfs.getConnectionUptime()
        return flask.jsonify(uptime=uptime)

    @app.route('/api/get-config', methods=['GET'])
    def get_general():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.cfg_getConfig())