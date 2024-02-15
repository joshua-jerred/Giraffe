/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   curl_utility.hpp
 * @brief  Simple wrapper around libcurl for HTTP requests.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-10-26
 * @copyright  2023 (license to be defined)
 */

#ifndef CURL_UTILITY_HPP_
#define CURL_UTILITY_HPP_

#include <curl/curl.h>

#include <string>
#include <vector>

class CurlRequest {
public:
  enum class Method { GET, PUT, POST };

  CurlRequest(const std::string &url,
              CurlRequest::Method method = CurlRequest::Method::GET)
      : method_(method), url_(url) {
  }
  ~CurlRequest() = default;

  void setHeaders(const std::vector<std::string> &headers) {
    headers_ = headers;
  }
  void setBody(const std::string &body) {
    body_ = body;
  }
  void setMethod(Method method) {
    method_ = method;
  }

  bool perform() {
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());

    if (method_ == Method::PUT) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_.c_str());
    } else if (method_ == Method::POST) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_.c_str());
    } else {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    }

    struct curl_slist *headers = NULL;
    for (auto &header : headers_) {
      headers = curl_slist_append(headers, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
  }

private:
  Method method_ = Method::GET;
  std::vector<std::string> headers_{};
  std::string url_{};
  std::string body_{};
};

#endif /* CURL_UTILITY_HPP_ */