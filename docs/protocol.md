The protocol used for all network communication within Giraffe is fairly
simple.

# Message Format and Fields
```jsonc
{
  "src": "",
  "dst": "",
  "typ": "",
  "id": "",
  "bdy": {}
}
```
## src/dst
The source/destination is a string that identifies the source/destination of 
the message.

Valid src/dst values:
- `gfs` : Giraffe Flight System
- `ggs` : Giraffe Ground Station
- `gdl` : Giraffe Data Link (ground)
- `gwc/<id>` : Giraffe Web Client with a unique <id> (3-15 chars)

## typ
The type of the message.

Valid types are:
- `req` - A request for information
- `set` - A request to set values
- `rsp` - A response to a `req` or `set`

## id
A unique string that identifies this message. This is used to match 
requests/responses. Randomly generated 8 symbol (32 bit) hexadecimal value,
in lowercase ascii characters.

## bdy
The message body, unique to each 
### `req`
```jsonc
{
  "rsc": "string"
}
```
- `rsc` : The data resource requested to be returned

### `set`
```jsonc
{
  "rsc": "string",
  "dat": {}
}
```
- `rsc` : The data requested to be set
- `dat` : The data to set

### `rsp`
```jsonc
{
  "cde": 0,
  "dat": {}
}
```
- `cde` : Response code (see below)
- `dat` : The data requested from a `req` or `set`, given the context of the
          response code.

***

# Valid Resources
- `ping` : req
- `setting/<setting_id>` : req, set
- `data/<data_id>` : req

# Response Codes
- `er` error
  - Response dat section will be `{"error":""}` with a message
- `ok` 
  - good request, expected values should be in the message
- `un`
  - unknown

# Ports
- GFS: 7893
- GGS: 7892
- GWC: 3000

# Streams
Streams are unique to the GWC/GGS connection. They use req/rsp messages.

A client on GWC can request a stream, the GGS will then send the data associated
with that stream on a set interval.

Stream requests come from the client. 
A client has a few action options:
- `add`
  - Add a stream by name
- `remove`
  - Remove a stream by name
- `remove_all`
  - Remove all streams
- `get`
  - Get a single instance of stream data (so not really a stream, just a non-http get request)

## Stream Request Format
Stream requests are normal `req` messages, but in addition to the `rsc` (resource) field
of the body there will be a `stream` and `action` field.

- `rsc` will always be `"stream"`
- `action` will be `add`, `remove`, `remove_all`, or `get`
- `stream` will be the name of the stream for the action to be completed against.
  - If the action is `remove_all` the `stream` field is optional/will be ignored.

### Example Stream Requests
**ADD**
```jsonc
{
    "src":"gwc",
    "dst":"ggs",
    "typ":"req",
    "id":"123",
    "bdy":{
        "rsc":"stream",
        "stream":"ggs_status",
        "action":"add"
    }
}
```

**REMOVE**
```jsonc
{
    "src":"gwc",
    "dst":"ggs",
    "typ":"req",
    "id":"123",
    "bdy":{
        "rsc":"stream",
        "stream":"ggs_status",
        "action":"remove"
    }
}
```

**REMOVE_ALL**
```jsonc
{
    "src":"gwc",
    "dst":"ggs",
    "typ":"req",
    "id":"123",
    "bdy":{
        "rsc":"stream",
        "action":"remove_all"
        // stream field is optional/will be ignored
    }
}
```

**GET**
```jsonc
{
    "src":"gwc",
    "dst":"ggs",
    "typ":"req",
    "id":"123",
    "bdy":{
        "rsc":"stream",
        "stream":"ggs_status",
        "action":"get"
    }
}
```

## Stream Response Example
```jsonc
{
    "src": "ggs",
    "dst": "gwc",
    "typ": "rsp",
    "id": "eca18003",
    "bdy": {
        "cde": "ok",
        "stream": "ggs_status",
        "data": {
            "status": "online"
        }
    }
}
```