
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
  std::vector<std::vector<std::string>> inputs;
  for (std::string s; not in.eof(); std::getline(in, s)) {
    if (s == "")
      inputs.push_back({});
    else
      inputs.back().push_back(s);
  }
  //for (auto& m: inputs) {
  //  for (auto& l: m) {
  //    std::cout << "<< " << l << " >>\n";
  //  }
  //  std::cout << "---\n";
  //}

  auto result{0ull};
  for (const auto& pattern: inputs) {
    /// Vertical line reflection
    const int H = pattern.size();
    const int W = pattern[0].size();
    for (auto x{0}; x < W-1; ++x) {  /// ! First/last columns trivial symmetry
      auto symmetric{true};
      for (auto dx{0};; ++dx) {
        const auto x0{x - dx};      /// Left
        const auto x1{x + dx + 1};  /// Right
        if (x0 < 0 or x1 >= W)
          break;
        for (auto y = 0; y < H; ++y)
          symmetric &= (pattern[y][x0] == pattern[y][x1]);
      }
      if (symmetric) {
        //std::cout << "V symmetry " << x+1 << "\n";
        result += (x + 1);
        break;
      }
    }
    /// Horizontal line reflection
    for (auto y{0}; y < H-1; ++y) {  /// ! First/last rows trivial symmetry
      auto symmetric{true};
      for (auto dy{0};; ++dy) {
        const auto y0{y - dy};      /// Left
        const auto y1{y + dy + 1};  /// Right
        if (y0 < 0 or y1 >= H)
          break;
        for (auto x = 0; x < W; ++x)
          symmetric &= (pattern[y0][x] == pattern[y1][x]);
      }
      if (symmetric) {
        //std::cout << "H symmetry " << y+1 << "\n";
        result += (y + 1) * 100;
        break;
      }
    }
  }

  std::cout << "Summarizing all symmetry notes: " << result << "\n";

  return EXIT_SUCCESS;
}

