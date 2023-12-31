
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <queue>
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
  std::vector<std::string> plan;
  for (std::string s;;) {
    std::getline(in, s);
    if (in.eof())
      break;
    plan.push_back(s);
  }

  /// Parse instructions and split into horizontal segments
  std::vector<std::array<std::int64_t,2>> pts;
  std::vector<std::array<std::int64_t,3>> horizontal_segments;
  {
    auto x{0ll}, y{0ll};
    for (const auto& instruction: plan) {
      //const auto ndir{instruction[0]};
      //const auto steps{std::stoi(instruction.substr(2, instruction.size() - 12))};
      //const auto nx = x + (ndir == 'L' ? -1 : ndir == 'R' ? 1 : 0) * steps;
      //const auto ny = y + (ndir == 'U' ? -1 : ndir == 'D' ? 1 : 0) * steps;
      //if (ndir == 'R') {
      //  /// L->R segment
      //  horizontal_segments.push_back({x,nx,y});
      //} else if (ndir == 'L') {
      //  /// R->L segment
      //  horizontal_segments.push_back({nx,x,y});
      //}

      const auto ndir{instruction[instruction.size() - 2]};
      auto steps{std::stoi(instruction.substr(instruction.size() - 7, 5), nullptr, 16)};
      const auto nx = x + (ndir == '2' ? -1 : ndir == '0' ? 1 : 0) * steps;
      const auto ny = y + (ndir == '3' ? -1 : ndir == '1' ? 1 : 0) * steps;
      if (ndir == '0') {
        /// L->R segment
        horizontal_segments.push_back({x,nx,y});
      } else if (ndir == '2') {
        /// R->L segment
        horizontal_segments.push_back({nx,x,y});
      }

      x = nx;
      y = ny;
      pts.push_back({x,y});
      //std::cout << "-> " << nx << "," << ny << "\n";
    }

    /// Area of closed polygon, via https://stackoverflow.com/a/451482
    auto result{0ull};
    auto len{0ull};
    for (auto i{0uz}; i < pts.size() - 1; ++i) {
      const auto a = pts[i][0] * pts[i+1][1] - pts[i+1][0] * pts[i][1];
      len += std::abs(pts[i][0] - pts[i+1][0]) + 
             std::abs(pts[i][1] - pts[i+1][1]);
      result += a;
    }
    result -= pts[pts.size()-1][0] * pts[0][1] - pts[0][0] * pts[pts.size()-1][1];
    len += std::abs(pts[pts.size()-1][0] - pts[0][0]) +
           std::abs(pts[pts.size()-1][1] - pts[0][1]);
    result /= 2;
    result += len/2;
    /// TODO: understand why +1
    std::cout << "Tiles enclosed by loop: " << result+1 << "\n";
  }

  return EXIT_SUCCESS;
}

