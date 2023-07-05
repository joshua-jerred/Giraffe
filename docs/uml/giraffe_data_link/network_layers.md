# Giraffe Data Link - Network Layers

## Application/Session Layer
The application/session layer will provide the common interface for the Giraffe
Data Link. It will provide the following functionality:

Public:

- Start()
  - This will start the Giraffe Data Link.

- Stop()
  - This will stop the Giraffe Data Link.

- Broadcast(message)
  - A broadcast message, will be sent regardless of the connection status.

- Exchange(message, callback)
  - Requires an active connection, but there will be a queue of messages that
    will be sent once the connection is established.

- ConnectionStatus()
  - This will provide the current connection status.

Transport Layer Interface:
- Broadcast(message)
 - This will send a broadcast message

## Transport Layer
- This layer will provide the following functionality:
  - Reliable message delivery option (RTP)
  - Message Segmentation
  - Message Reassembly
  - Message Acknowledgement

## Data Link Layer
- AX.25

## Physical Layer
- VHF/UHF

***

# Configuration
- Timeout