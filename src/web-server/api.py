import flask
from web_server import GiraffeWebServer
from flask import after_this_request

def add_pages(app):
    @app.route("/config")
    def config():
        config = flask.render_template('config.html')
        return config

    @app.route("/log-files")
    def logs_files():
        logs_files = flask.render_template('log-files.html')
        return logs_files

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

        connected = gfs.getConnectionStatus()
        connection_uptime = gfs.getConnectionUptime()
        return flask.jsonify(connected=connected, connection_uptime=connection_uptime)

    @app.route('/api/get-connection-requested', methods=['GET'])
    def get_connection_requested():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response

        connection_requested = gfs.getConnectionRequested()
        return flask.jsonify(connection_requested=connection_requested)

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
        return flask.jsonify(gfs.get('config'))

    @app.route('/api/get-status-data', methods=['GET'])
    def get_status():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.get('status'))

    @app.route('/api/get-gfs-data', methods=['GET'])
    def get_gfs():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.get('gfs-data'))

    @app.route('/api/get-extension-statuses', methods=['GET'])
    def get_extension_statuses():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.get('extension-statuses'))

    @app.route('/api/get-telemetry-data', methods=['GET'])
    def get_telemetry():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.get('telemetry-data'))

    @app.route('/api/get-data-frame', methods=['GET'])
    def get_data_frame():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.get('data-frame'))

    @app.route('/api/get-error-frame', methods=['GET'])
    def get_error_frame():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.get('error-frame'))

    @app.route('/api/command', methods=['POST'])
    def command():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        command = flask.request.json['command']
        return flask.jsonify(gfs.command(command))
    
    @app.route('/api/log-files', methods=['GET'])
    def data_logs():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        return flask.jsonify(gfs.get('log-files'))
    
    @app.route('/api/delete-data-log-file', methods=['POST'])
    def delete_log_file():
        @after_this_request
        def add_header(response):
            response.headers['Access-Control-Allow-Origin'] = '*'
            return response
        file_name = flask.request.json['file_name']
        return flask.jsonify(gfs.delete_log_file(file_name))