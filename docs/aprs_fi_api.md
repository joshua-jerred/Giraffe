# aprs.fi API usage

The ground station can use the aprs.fi API to get the latest position of a
station. Information about the API can be found
[here on aprs.fi](https://aprs.fi/page/api).

The API requires an API key.

Please keep in mind that this is a free API and that it should be used carefully
with respect to the server load.

The polling interval can be configured on the web interface. The minimum value
is every 3 minutes. The default value is 10 minutes. During a single run,
if there are 2 tries that fail (including not finding data), it will be
automatically disabled.

The data is stored in the `aprs_fi` table in the database.

The station name must include the SSID. For example, `N0CALL-1`.