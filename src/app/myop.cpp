// Standard headers
#include <vector>
#include <iostream>

// Internal headers
#include "app/Myop.hpp"
#include "app/states.hpp"

int main(int argc, char *argv[]) {
  std::string dataset = "dataset";

  if (argc >= 2) {
    dataset = argv[1];
  }

  tops::app::Myop myop(dataset);

  auto prediction = myop.predict({ 1, 1, 1, 1, 0, 1, 1, 0, 1 });

  for (auto symbol : prediction)
    std::cout << tops::app::state_names[symbol] << " ";
  std::cout << std::endl;

  return 0;
}
