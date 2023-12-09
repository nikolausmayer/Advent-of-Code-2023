
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

  /// Parse input file
  std::vector<std::vector<int>> sequences{{}};
  {
    for (;;) {
      int n;
      in >> n;
      if (in.eof())
        break;
      sequences.back().push_back(n);
      if (in.peek() == '\n')
        sequences.push_back({});
    }
  }
  sequences.pop_back();

  auto result{0ull};
  for (auto s: sequences) {
    std::vector<std::vector<int>> derivatives{s};
    std::cout << "sequence:  "; for (auto i: s) std::cout << std::setw(10) << i; std::cout << "\n";
    /// Build triangle
    for (;;) {
      auto minv = std::numeric_limits<int>::max();
      auto maxv = std::numeric_limits<int>::min();
      std::vector<int> next;
      for (auto i = 1uz; i < derivatives.back().size(); ++i) {
        auto d = derivatives.back()[i] - derivatives.back()[i-1];
        minv = std::min(minv, d);
        maxv = std::max(maxv, d);
        next.push_back(d);
      }
      std::cout << "  level " << derivatives.size() << ": "; for (auto i: next) std::cout << std::setw(10) << i; std::cout << "\n";
      if (minv == 0 and maxv == 0) {
        break;
      }
      derivatives.push_back(next);
    }

    /// Compute new prediction
    auto val{0};
    for (; derivatives.size() > 0; derivatives.pop_back()) {
      val = derivatives.back().back() + val;
      std::cout << "level " << derivatives.size()-1 << " prediction: " << val << "\n";
    }
    result += val;
  }

  std::cout << "\n" << "Sum of extrapolated values: " << result << "\n";

  return EXIT_SUCCESS;
}

