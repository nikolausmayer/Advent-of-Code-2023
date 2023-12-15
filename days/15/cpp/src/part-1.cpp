
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  /// Read input file
  const auto input = [&]{
    std::string s;
    std::getline(in, s);
    return s;
  }();

  /// "HASH" algorithm
  auto result{0ull};
  auto current_value{0};
  for (auto c: input) {
    if (c == ',') {
      result += current_value;
      current_value = 0;
    } else {
      current_value = ((current_value + c) * 17) % 256;
    }
  }
  result += current_value;

  std::cout << "Sum of HASHes: " << result << "\n";

  return EXIT_SUCCESS;
}

