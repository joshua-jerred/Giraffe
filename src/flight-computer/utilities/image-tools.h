#ifndef IMAGE_TOOLS_H_
#define IMAGE_TOOLS_H_

#include <string>

class SstvImageTools {
 public:
  /**
   * @brief Construct a new SSTV Image Tools object
   *
   * @param source_image_path The path to the image to be converted
   * @param destination_image_path *Optional* If not specified, the source image
   * will be overwritten
   * @exception SstvImageToolsException If the image cannot be read
   */
  SstvImageTools(std::string source_image_path,
                 std::string destination_image_path = "");
  ~SstvImageTools();

  void Write();

 private:
  std::string source_path_;
  std::string destination_path_;
  Magick::Image image_;
};

class SstvImageToolsException : public std::exception {
 public:
  SstvImageToolsException(std::string message) : message_(message) {}
  ~SstvImageToolsException() throw() {}
  const char* what() const throw() { return message_.c_str(); }

 private:
  std::string message_;
};

bool ConvertToRobot8(std::string image_path);
bool ConvertToRobot36(std::string image_path);
bool ConvertToCustom8(std::string image_path);

bool Pixels(std::string image_path);

#endif