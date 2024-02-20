/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   request_handler_image.cpp
 * @brief  The image request handler implementation.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#include "giraffe_file_paths.hpp"
#include "request_handler.hpp"

void RequestRouter::handleImageRequest(sock::TcpSocketServer &client,
                                       const std::string &msg_rsc) {
  constexpr size_t K_MAX_IMAGE_SIZE = 1024 * 1024 * 5; // 5MB

  std::string image_path{};
  std::ifstream image_file;
  try {
    image_path =
        giraffe::file_paths::getGfsImageDirPath() + "/" + msg_rsc.substr(4);
    std::filesystem::path path(image_path);
    if (!std::filesystem::exists(path)) {
      client.send("image not found");
      return;
    }
    size_t image_size = std::filesystem::file_size(path);
    if (image_size > K_MAX_IMAGE_SIZE) {
      client.send("image too large");
      return;
    }

    image_file.open(image_path, std::ios::binary);
  } catch (const std::exception &e) {
    client.send("invalid image request/path");
    return;
  }

  if (!image_file.is_open()) {
    client.send("open failure");
    return;
  }

  std::vector<uint8_t> image_data((std::istreambuf_iterator<char>(image_file)),
                                  std::istreambuf_iterator<char>());
  image_file.close();

  uint32_t image_size = image_data.size();
  client.sendRawData(reinterpret_cast<uint8_t *>(&image_size),
                     sizeof(image_size));

  // send the image in chunks
  constexpr size_t K_BUFFER_SIZE = 512;
  size_t bytes_sent = 0;
  while (bytes_sent < image_data.size()) {
    size_t bytes_to_send =
        std::min(K_BUFFER_SIZE, image_data.size() - bytes_sent);
    client.sendRawData(image_data.data() + bytes_sent, bytes_to_send);
    bytes_sent += bytes_to_send;
  }

  // send terminate signal (image size again)
  client.sendRawData(reinterpret_cast<uint8_t *>(&image_size),
                     sizeof(image_size));
}