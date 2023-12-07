
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
  auto time{0ull}, distance{0ull};
  for (;;) {
    const auto c = in.get();
    if (in.eof())
      break;
    if (c == '\n')
      std::swap(time, distance);
    if (c >= '0' and c <= '9')
      time = time * 10 + (c - '0');
  }

  /// Binary searches because the winning region is going to be continuous
  std::uint64_t from, to;
  {
    /// Lower bound
    auto lower{1ull}, upper{time - 1ull};
    while (lower != upper) {
      const auto mid = (lower + upper) >> 1;
      if ((time - mid) * mid <= distance) {
        //std::cout << mid << " -> " << (time - mid) * mid << " <= " << distance << " | " << lower << " " << mid << " " << upper << "\n";
        lower = mid + 1;
      } else {
        //std::cout << mid << " -> " << (time - mid) * mid << " > " << distance << " | " << lower << " " << mid << " " << upper << "\n";
        upper = mid;
      }
      from = upper;
    }
  }
  {
    /// Upper bound
    auto lower{1ull}, upper{time - 1ull};
    while (lower != upper) {
      const auto mid = (lower + upper) >> 1;
      if ((time - mid) * mid <= distance) {
        //std::cout << mid << " -> " << (time - mid) * mid << " <= " << distance << " | " << lower << " " << mid << " " << upper << "\n";
        upper = mid;
      } else {
        //std::cout << mid << " -> " << (time - mid) * mid << " > " << distance << " | " << lower << " " << mid << " " << upper << "\n";
        lower = mid + 1;
      }
      to = mid;
    }
  }

  std::cout << "Winning ways: " << to - from + 1 << "\n";

  return EXIT_SUCCESS;
}

