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

    @app.route('/api/get-general', methods=['GET'])
    def get_general():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.cfg_getGeneral())

    @app.route('/api/get-debug', methods=['GET'])
    def get_debug():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.cfg_getDebug())

    @app.route('/api/get-flight-procedures', methods=['GET'])
    def get_flight_procedures():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.cfg_getFlightProcedures())

    @app.route('/api/get-extensions', methods=['GET'])
    def get_extension():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.cfg_getExtensions())

    @app.route('/api/get-telemetry', methods=['GET'])
    def get_telemetry():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.cfg_getTelemetry())

    @app.route('/api/get-data-packet-contents', methods=['GET'])
    def get_data_packet_contents():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.cfg_getDataPacketContents())

    @app.route('/api/get-data-log-contents', methods=['GET'])
    def get_data_log_contents():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.cfg_getDataLogContents())