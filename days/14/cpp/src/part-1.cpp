
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
  std::vector<std::string> map;
  for (;;) {
    std::string s;
    std::getline(in, s);
    if (in.eof())
      break;
    map.push_back(s);
  }
  const int H = map.size();
  const int W = map[0].size();

  //for (const auto& row: map)
  //  std::cout << row << "\n";
  //std::cout << "\n";

  /// Walk over map and move each "O" round rock north
  for (auto y = 0; y < H; ++y) {
    for (auto x = 0; x < W; ++x) {
      if (map[y][x] == 'O') {
        auto ty{y};
        /// Roll north while tiles are empty
        while (ty > 0 and map[ty-1][x] == '.') {
          map[ty][x] = '.';
          map[ty-1][x] = 'O';
          --ty;
        }
      }
    }
  }
  //for (const auto& row: map)
  //  std::cout << row << "\n";
  //std::cout << "\n";

  /// Sum up result
  auto result{0ull};
  for (auto y = 0; y < H; ++y) {
    for (auto x = 0; x < W; ++x) {
      if (map[y][x] == 'O') {
        result += H - y;
      }
    }
  }
  
  std::cout << "Total round-rocks load on north support beams: " << result << "\n";

  return EXIT_SUCCESS;
}

