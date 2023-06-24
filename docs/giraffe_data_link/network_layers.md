# Giraffe Data Link - Network Layers

## Application/Session Layer
The application/session layer will provide the common interface for the Giraffe
Data Link. It will provide the following functionality:

- Start()
  - This will start the Giraffe Data Link.

- Stop()
  - This will stop the Giraffe Data Link.

- Broadcast(message)
  - A broadcast message, will be sent regardless of the connection status.

- Exchange(message, callback, timeout)
  - Requires an active connection, but there will be a queue of messages that
    will be sent once the connection is established.

- ConnectionStatus()
  - This will provide the current connection status.

## Transport Layer

## Data Link Layer

## Physical Layer