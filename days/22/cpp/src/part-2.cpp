
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
#include <optional>
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

  /// Read input
  std::vector<std::array<std::array<int,3>,2>> bricks;
  for (std::string line;;) {
    std::getline(in, line);
    if (in.eof())
      break;
    std::array<std::array<int,3>,2> brick;
    auto it = 0ul;
    for (auto i = 0; i < 2; ++i) {
      for (auto j = 0; j < 2; ++j) {
        const auto tmp = line.find(',', it);
        brick[i][j] = std::stoi(line.substr(it, tmp - it));
        it = tmp+1;
      }
      {
        const auto tmp = line.find('~', it);
        brick[i][2] = std::stoi(line.substr(it, tmp - it));
        it = tmp+1;
      }
    }
    if (brick[0][0] > brick[1][0] or
        brick[0][1] > brick[1][1] or
        brick[0][2] > brick[1][2])
      std::cout << "BAD BRICK\n";
    bricks.push_back(brick);
  }

  /// Sort bricks my time-to-ground
  std::ranges::sort(bricks, [](const auto& a, const auto& b) {
                      return std::min(a[0][2], a[1][2]) <
                             std::min(b[0][2], b[1][2]);
                    });

  /// Store shifting contour as heightmap
  std::map<std::array<int,2>,int> grid_height_map;
  /// Start sh^H^Hdropping bricks
  for (auto& brick: bricks) {
    /// Scan footprint
    auto drop_height = brick[1][2];
    const auto z = brick[0][2];
    for (auto x = brick[0][0]; x <= brick[1][0]; ++x) {
      for (auto y = brick[0][1]; y <= brick[1][1]; ++y) {
        drop_height = std::min(drop_height, z - grid_height_map[{x,y}]);
      }
    }
    //std::cout << brick[0][2] << " -> " << brick[0][2]-drop_height << "\n";

    brick[0][2] -= drop_height - 1;
    brick[1][2] -= drop_height - 1;

    for (auto x = brick[0][0]; x <= brick[1][0]; ++x) {
      for (auto y = brick[0][1]; y <= brick[1][1]; ++y) {
        grid_height_map[{x,y}] = brick[1][2];
      }
    }
  }
  //for (const auto& b: bricks)
  //  std::cout << "[" << b[0][0] << " "
  //                   << b[0][1] << " "
  //                   << b[0][2] << "] [" 
  //                   << b[1][0] << " "
  //                   << b[1][1] << " "
  //                   << b[1][2] << "]\n";

  /// Analyse brick support network
  std::map<std::size_t, std::set<std::size_t>> supports, supported_by;
  for (auto i = 0uz; i < bricks.size() - 1; ++i) {
    const auto& brick = bricks[i];
    const auto z = brick[1][2];
    for (auto j = i + 1; j < bricks.size(); ++j) {
      const auto& higher_brick = bricks[j];
      auto does_support = false;
      for (auto x = brick[0][0]; x <= brick[1][0]; ++x) {
        for (auto y = brick[0][1]; y <= brick[1][1]; ++y) {
          if (x >= higher_brick[0][0] and x <= higher_brick[1][0] and
              y >= higher_brick[0][1] and y <= higher_brick[1][1] and
              z == higher_brick[0][2] - 1) {
            does_support = true;
          }
        }
      }
      if (does_support) {
        supports[i].insert(j);
        supported_by[j].insert(i);
      }
    }
  }

  auto result = 0;

  /// Test all bricks
  for (auto i = 0uz; i < bricks.size() - 1; ++i) {
    std::queue<std::size_t> falling_bricks;
    falling_bricks.push(i);
    std::set<std::size_t> fallen_bricks;

    while (falling_bricks.size()) {
      const auto brick = falling_bricks.front();
      falling_bricks.pop();
      if (fallen_bricks.contains(brick))
        continue;
      fallen_bricks.insert(brick);

      /// Propagate bricks that would now fall
      for (auto higher_brick: supports[brick]) {
        auto falls_now = true;
        for (auto supporting_brick: supported_by[higher_brick])
          if (not fallen_bricks.contains(supporting_brick))
            falls_now = false;
        if (falls_now)
          falling_bricks.push(higher_brick);
      }
    }

    /// Only count *other* bricks!
    result += fallen_bricks.size() - 1;
  }

  std::cout << "In total, " << result << " bricks would fall.\n";

  return EXIT_SUCCESS;
}

