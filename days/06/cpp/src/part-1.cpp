
#include <cassert>
#include <cstdlib>
#include <array>
#include <fstream>
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

  /// Parse input file
  std::vector<std::uint64_t> times, distances;
  {
    std::string s;
    in >> s;
    while (in.peek() != '\n') {
      std::uint64_t i; in >> i; times.push_back(i);
    }
    in >> s;
    while (not in.eof() and in.peek() != '\n') {
      std::uint64_t i; in >> i; distances.push_back(i);
    }
  }

  auto result{1ull};
  for (auto race = 0uz; race < times.size(); ++race) {
    const auto time{times[race]};
    const auto distance{distances[race]};
    auto winning_ways{0};

    auto speed = 0ull;
    for (auto i = 0ull; i < time; ++i) {
      winning_ways += (++speed * (time - i - 1) > distance);
    }
    result *= winning_ways;
  }

  std::cout << "Result: " << result << "\n";

  return EXIT_SUCCESS;
}

