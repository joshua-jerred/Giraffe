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