#include <ctime>
#include <curl_utility.hpp>
#include <iostream>
#include <sstream>

int main() {
  CurlRequest request{"http://localhost:8086/api/v2/"
                      "write?org=testOrg&bucket=testBucket&precision=s"};

  std::vector<std::string> headers{"Authorization: Token "
                                   "HJz7efsR1_"
                                   "kEENqTCUpTcrYC8VMm8tBLH9gUf3NVGg6FUS32xBgfd"
                                   "ZoyMF2iIdvxaiv1sQMpCKwsp1saEuAX"
                                   "Fg==",
                                   "Content-Type: text/plain; charset=utf-8",
                                   "Accept: application/json"};

  request.setMethod(CurlRequest::Method::POST);
  request.setHeaders(headers);
  request.setBody("airSensors,sensor_id=TLM0201 "
                  "temperature=73.97038159354763,humidity=35.23103248356096,co="
                  "0.48445310567793615");

  request.perform();
  return 0;
}