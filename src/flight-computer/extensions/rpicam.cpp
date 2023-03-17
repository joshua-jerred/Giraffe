/**
 * @file rpicam.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Temporary Pi Camera Extension
 * @date 2023-02-25
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

/*
 This is very much a temporary implementation, it desperately needs to be
 updated using libcamera.
*/

#include <ctime>

#include "configurables.h"
#include "extensions.h"

extension::RaspPiCamera::RaspPiCamera(DataStream *p_data_stream,
                                      ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {}

extension::RaspPiCamera::~RaspPiCamera() {}

std::string exec(std::string command) {
  std::string response;
  std::array<char, 1028> out_buff;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    return "";
  }
  while (fgets(out_buff.data(), out_buff.size(), pipe.get()) != nullptr) {
    response += out_buff.data();
  }
  return response;
}

int extension::RaspPiCamera::runner() {
  setStatus(ExtensionStatus::RUNNING);
  while (!stop_flag_) {
    
    std::string image_path = CaptureImage();
    if (image_path != "") {
      sendData("NEW_IMAGE", image_path);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(getUpdateInterval()));
  }
  setStatus(ExtensionStatus::STOPPED);
  return 0;
}

std::string extension::RaspPiCamera::CaptureImage() {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%d-%m-%Y--%H-%M-%S",timeinfo);
  std::string time_str(buffer);
  
  std::string image_name = time_str + ".png";
  std::string image_path = "images/" + image_name;
  std::string command = 
      "libcamera-still -e png -o " + image_path + " --immediate";
  
  std::string response = exec(command);

  return image_name;
}