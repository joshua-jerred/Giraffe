#include "image-tools.h"

int main() {
  ConvertToRobot8("test-pattern.png");
  ConvertToRobot36("test-pattern.png");
  ConvertToCustom8("test-pattern.png");

  ConvertToRobot8("test-image.png");
  ConvertToRobot36("test-image.png");
  ConvertToCustom8("test-image.png");

  Pixels("test-image.png.r8.png");
  return 0;
}