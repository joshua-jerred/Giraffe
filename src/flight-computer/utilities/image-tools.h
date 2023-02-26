#ifndef IMAGE_TOOLS_H_
#define IMAGE_TOOLS_H_

#include <string>

bool ConvertToRobot8(std::string image_path);
bool ConvertToRobot36(std::string image_path);
bool ConvertToCustom8(std::string image_path);

bool Pixels(std::string image_path);

#endif