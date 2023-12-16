
#include <cassert>
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
  std::vector<std::string> map;
  for (std::string s;;) {
    std::getline(in, s);
    if (in.eof())
      break;
    map.push_back(s);
  }
  const int H = map.size();
  const int W = map[0].size();

  auto trace = [&](int tx, int ty, int tdir) {
    /// Directions: 0:E 1:S 2:W 3:N
    std::list<std::array<int,3>> beams{{tx, ty, tdir}};

    std::vector<std::vector<int>> tile_states_yx(H, std::vector<int>(W, 0));

    std::set<std::array<int,3>> beams_memory;
    while (beams.size()) {
      decltype(beams) new_beams;

      for (auto& [x,y,dir]: beams) {
        if (beams_memory.contains({x,y,dir}))
          continue;
        beams_memory.insert({x,y,dir});

        x += (dir == 0 ? 1 : dir == 2 ? -1 : 0);
        y += (dir == 1 ? 1 : dir == 3 ? -1 : 0);
        if (x < 0 or x >= W or y < 0 or y >= H)
          continue;

        tile_states_yx[y][x] = 1;
        
        const auto tile{map[y][x]};
        /// > \.
        ///   v
        if      (dir == 0 and tile == '\\') { dir = 1; }
        ///   ^
        /// > /
        else if (dir == 0 and tile == '/' ) { dir = 3; }
        ///   ^
        /// > |
        ///   v
        else if (dir == 0 and tile == '|' ) { dir = 3;
          new_beams.push_back({x, y, 1});  /// v split
        }
        /// ^
        /// \ <
        else if (dir == 2 and tile == '\\') { dir = 3; }
        /// / <
        /// v
        else if (dir == 2 and tile == '/' ) { dir = 1; }
        /// ^
        /// | <
        /// v
        else if (dir == 2 and tile == '|' ) { dir = 3;
          new_beams.push_back({x, y, 1});  /// v split
        }
        ///   v
        /// < /
        else if (dir == 1 and tile == '/' ) { dir = 2; }
        /// v
        /// \ >
        else if (dir == 1 and tile == '\\') { dir = 0; }
        ///   v
        /// < - >
        else if (dir == 1 and tile == '-' ) { dir = 2;
          new_beams.push_back({x, y, 0});  /// > split
        }
        /// < \.
        ///   ^
        else if (dir == 3 and tile == '\\') { dir = 2; }
        /// / >
        /// ^
        else if (dir == 3 and tile == '/' ) { dir = 0; }
        /// < - >
        ///   ^
        else if (dir == 3 and tile == '-' ) { dir = 2; 
          new_beams.push_back({x, y, 0});  /// > split
        }

        new_beams.push_back({x, y, dir});
      }

      beams = new_beams;

      //for (auto& [x,y,dir]: beams)
      //  std::cout << "Beam: " << x << "," << y << "  " << dir << "\n";
      //std::cout << "State: \n";
      //for (const auto& row: tile_states_yx) {
      //  for (auto tile: row)
      //    std::cout << (tile ? '#' : '.');
      //  std::cout << "\n";
      //}
      //std::cout << "\n";
    }

    /// Count energized tiles
    auto result{0};
    for (const auto& row: tile_states_yx)
      for (auto tile: row)
        result += tile;

    return result;
  };

  /// Brute-force all beam entries
  auto result{0};
  for (auto x = 0; x < W; ++x) result = std::max(result, trace( x, -1, 1));
  for (auto x = 0; x < W; ++x) result = std::max(result, trace( x,  H, 3));
  for (auto y = 0; y < H; ++y) result = std::max(result, trace(-1,  y, 0));
  for (auto y = 0; y < H; ++y) result = std::max(result, trace( W,  y, 2));

  std::cout << "Energized tiles in most-energized-tiles configuration: " << result << "\n";

  return EXIT_SUCCESS;
}

