#include "image-tools.h"

#include <Magick++.h>

#include <iostream>

SstvImageTools::SstvImageTools(std::string source_image_path,
                               std::string destination_image_path) {
  source_path_ = source_image_path;
  if (destination_image_path == "") {
    destination_path_ = source_image_path;
  } else {
    destination_path_ = destination_image_path;
  }
  try {
    image_.read(source_image_path);
  } catch (Magick::Exception &error_) {
    throw SstvImageToolsException("Failed to read image: " + source_image_path);
  }
}

bool ConvertToRobot8(std::string image_path) {
  Magick::Image image;
  Magick::Geometry crop_size(160, 120);
  crop_size.aspect(true);
  try {
    image.read(image_path);
    image.scale(crop_size);
    image.quantizeColorSpace(Magick::GRAYColorspace);
    image.quantizeColors(8);
    image.quantize();
    image.write(image_path + ".r8.png");
  } catch (Magick::Exception &error_) {
    std::cout << "Caught exception: " << error_.what() << std::endl;
    return 1;
  }
  return 0;
}

bool ConvertToRobot36(std::string image_path) {
  Magick::Image image;
  Magick::Geometry crop_size(320, 240);
  crop_size.aspect(true);
  try {
    image.read(image_path);
    image.scale(crop_size);
    image.quantizeColorSpace(Magick::YUVColorspace);
    image.quantize();
    image.write(image_path + ".r36.png");
  } catch (Magick::Exception &error_) {
    std::cout << "Caught exception: " << error_.what() << std::endl;
    return 1;
  }
  return 0;
}

bool ConvertToCustom8(std::string image_path) {
  Magick::Image image;
  try {
    image.read(image_path);
    Magick::Geometry crop_size(400, 400);
    crop_size.aspect(true);
    image.scale(crop_size);
    image.crop(crop_size);
    image.quantizeColorSpace(Magick::GRAYColorspace);
    image.quantizeColors(200);
    image.quantize();
    image.write(image_path + ".c8.png");
  } catch (Magick::Exception &error_) {
    std::cout << "exception: " << error_.what() << std::endl;
    return 1;
  }
  return 0;
}

bool Pixels(std::string image_path) {
  Magick::Image image;
  try {
    image.read(image_path);
    Magick::Pixels view(image);
  } catch (Magick::Exception &error_) {
    std::cout << "exception: " << error_.what() << std::endl;
    return 1;
  }
  return 0;
}