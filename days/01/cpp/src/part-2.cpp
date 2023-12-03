
#include <cassert>
#include <cstdlib>
#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  std::uint64_t result{0};

  constexpr std::array<std::string_view,10> dict{
    "zero", "one", "two", "three", "four", 
    "five", "six", "seven", "eight", "nine",
  };

  for (;;) {
    std::string line;
    std::getline(in, line);
    if (in.eof())
      break;

    std::optional<int> first_digit{}, last_digit{};
    for (auto i = 0uz, block = 0uz; i < line.size(); ++i) {
      const auto c = line[i];
      std::optional<int> digit;
      if (c >= '0' and c <= '9') {
        digit = (c - '0');
      } else {
        for (auto j = 0uz; j < dict.size(); ++j) {
          const auto& word = dict[j];
          if ((i - block) + 1 >= word.size()) {
            if (line.substr(i + 1 - word.size(), word.size()) == word) {
              digit = j;
              block = i;
            }
          }
        }
      }

      if (digit) {
        if (not first_digit) {
          first_digit = digit;
        } else {
          last_digit = digit;
        }
      }
    }
    result += (first_digit.value() * 10 + last_digit.value_or(first_digit.value()));
  }

  std::cout << "Sum: " << result << "\n";

  return EXIT_SUCCESS;
}

