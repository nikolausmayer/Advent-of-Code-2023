
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  std::uint64_t result{0};

  for (;;) {
    std::string line;
    std::getline(in, line);
    if (in.eof())
      break;

    std::optional<int> first_digit{}, last_digit{};
    for (auto c: line) {
      if (c >= '0' and c <= '9') {
        if (not first_digit) {
          first_digit = (c - '0');
        } else {
          last_digit = (c - '0');
        }
      }
    }
    result += (first_digit.value() * 10 + last_digit.value_or(first_digit.value()));
  }

  std::cout << "Sum: " << result << "\n";

  return EXIT_SUCCESS;
}

