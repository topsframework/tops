// Standard headers
#include <iostream>
#include <string>
#include <vector>

// Internal headers
#include "myop/Myop.hpp"
#include "myop/states.hpp"

int main(int argc, char *argv[]) {
  std::string dataset = "dataset";

  if (argc >= 2) {
    dataset = argv[1];
  }

  tops::myop::Myop myop(dataset);

  auto prediction = myop.predict({
    // Random sequence
    //1, 3, 1, 2, 0, 1, 3, 0, 2

    // Real gene (AT1G34280)
    3, 2, 1, 3, 3, 1, 1, 1, 0, 3, 1, 0, 0, 0, 0, 3, 0, 3, 0, 3, 3, 1,
  });

  for (auto symbol : prediction)
    std::cout << tops::myop::state_names[symbol] << " ";
  std::cout << std::endl;

  return 0;
}
