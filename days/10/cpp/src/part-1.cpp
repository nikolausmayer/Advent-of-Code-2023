
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

  /// Find starting position and a starting direction
  int xs, ys;
  char dir;
  for (auto y = 0uz; y < map.size(); ++y) {
    for (auto x = 0uz; x < map[0].size(); ++x) {
      if (map[y][x] == 'S') {
        xs = x;
        ys = y;
        if (x > 0 and (map[y][x-1] == '-' or
                       map[y][x-1] == 'L' or
                       map[y][x-1] == 'F'))
          dir = 'L';
        if (x < map[0].size()-1 and (map[y][x+1] == '-' or
                                     map[y][x+1] == '7' or
                                     map[y][x+1] == 'J'))
          dir = 'R';
        if (y > 0 and (map[y-1][x] == '|' or
                       map[y-1][x] == '7' or
                       map[y-1][x] == 'F'))
          dir = 'U';
        if (y < map.size()-1 and (map[y+1][x] == '|' or
                                  map[y+1][x] == 'J' or
                                  map[y+1][x] == 'L'))
          dir = 'D';
        //std::cout << "start " << xs << "," << ys << " " << dir << "\n";
      }
    }
  }

  /// Walk whole pipe loop
  auto x{xs}, y{ys}, steps{0};
  for (;; ++steps) {
    if (steps != 0 and x == xs and y == ys)
      break;
    x += (dir == 'L' ? -1 : dir == 'R' ? 1 : 0);
    y += (dir == 'U' ? -1 : dir == 'D' ? 1 : 0);
    if (dir == 'L' and map[y][x] == 'L') dir = 'U';
    if (dir == 'L' and map[y][x] == 'F') dir = 'D';
    if (dir == 'R' and map[y][x] == 'J') dir = 'U';
    if (dir == 'R' and map[y][x] == '7') dir = 'D';
    if (dir == 'U' and map[y][x] == '7') dir = 'L';
    if (dir == 'U' and map[y][x] == 'F') dir = 'R';
    if (dir == 'D' and map[y][x] == 'J') dir = 'L';
    if (dir == 'D' and map[y][x] == 'L') dir = 'R';
  }

  /// (Round up in case of odd loop length)
  std::cout << "Farthest distance from the start: " << (steps+1)/2 << "\n";

  return EXIT_SUCCESS;
}

