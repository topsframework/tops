// Standard headers
#include <vector>
#include <iostream>

// Internal headers
#include "app/Myop.hpp"

int main(int argc, char *argv[]) {
  std::string dataset = "dataset";

  if (argc >= 2) {
    dataset = argv[1];
  }

  tops::app::Myop myop(dataset);

  return 0;
}
