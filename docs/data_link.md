# Giraffe Data Link (GDL)

## GDL Layers
- GiraffeDataLink (GDL)
- Transport Layer
  - Handles transport of broadcast and exchange messages asynchronously.
- Network Layer
  - Controls the physical layer.
  - Implements SignalEasel
- Physical Layer

## Data Link Server
The Giraffe Data Link Server is a TCP socket server that allows for the sending and receiving of data over the GDL.

The server needs to be actively reached out to because it only stores a limited number of messages. If the server crashes or is restarted, all messages will be lost.

### Configuration
The configuration for the GDL server is stored in `.giraffe/gdl_server.json`. It should not be edited while running except for updating via the API.

### Logging
There is a logging queue. The log can be retrieved with the API. The log will be cleared when the log is retrieved. The log has a maximum size so it will delete the oldest log items when the log is full.

#### API
The API uses the [giraffe protocol](protocol.md).

##### REQ Requests
- `status`: Get status of GDL
- `config`: Get configuration of GDL (JSON)
- `received_messages`: Get number of received messages. This will clear the received message queue. This will include location messages, broadcast messages, and exchange messages.
- `sent_messages`: Get the messages that have been queued to be sent. This will clear the server sent message queue (not the gdl sent message queue of messages that need to be sent).
- `log`: Returns all log items. This will clear the log.

##### SET Requests
- `config`: Set configuration of GDL (JSON). See the format in `.giraffe/gdl_server.json`. This will overwrite the current configuration. The server will **not** be restarted although a restart may be required for some changes to take effect.
- `disable_receiver`: Disable the GDL receiver
- `enable_receiver`: Enable the GDL receiver
- `reset_config`: Reset the GDL configuration to default
- `restart`: Restarts the GDL server. There will be no response to this request.
- `new_broadcast`: This will auto generate a new ID number. Add a new broadcast message to the GDL. The body (dat field) must contain:
      - `data`: The data (string) to send
- `new_exchange`: Same as new_broadcast but for exchanges
