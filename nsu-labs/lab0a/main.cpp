#include <cstdlib>
#include <iostream>
#include <math.h>

#include "module1.h"
#include "module2.h"
#include "module3.h"

int main(int argc, char **argv) {

  std::cout << Module1::getMyName() << "\n";
  std::cout << Module2::getMyName() << "\n";

  using namespace Module1;
  std::cout << getMyName() << "\n";
  std::cout << Module2::getMyName() << "\n";

  // using namespace Module2;
  // std::cout << getMyName() << "\n";

  using Module2::getMyName;
  std::cout << getMyName() << "\n";

  std::cout << Module3::getMyName() << "\n";

  return EXIT_SUCCESS;
}
