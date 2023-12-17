
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
  std::vector<std::string> map;
  for (std::string s;;) {
    std::getline(in, s);
    if (in.eof())
      break;
    map.push_back(s);
  }
  const int H = map.size();
  const int W = map[0].size();

  /// I guess this is A*?

  /// {x, y, direction (ESWN->0123), duration of current straight run,
  ///  accumulated heat loss, heuristic}
  auto cmp = [](const auto& a, const auto& b) {
    return (a[4] + a[5]) > (b[4] + b[5]);
  };
  std::priority_queue<std::array<int,6>, 
                      std::vector<std::array<int,6>>,
                      decltype(cmp)> queue(cmp);
  
  /// Heuristic is Manhattan distance
  auto heuristic = [&](auto x, auto y) {
    return W - 1 - x + H - 1 - y;
  };
  
  /// Start states
  queue.push({0, 0, 0, 0, 0, heuristic(0, 0)});  /// heading east
  queue.push({0, 0, 1, 0, 0, heuristic(0, 0)});  /// heading south
  /// A*
  std::set<std::array<int,4>> visited;
  while (queue.size()) {
    auto [x, y, dir, run, heat_loss, h] = queue.top();
    queue.pop();
    if (h == 0 and run >= 3) {  /// oh the curveballs
      std::cout << "Goal reached with accumulated heat loss " << heat_loss << "\n";
      break;
    }
    if (visited.contains({x, y, dir, run})) {
      continue;
    }
    visited.insert({x, y, dir, run});
    
    /// Continue straight
    if (run < 9) {
      const auto nx = x + (dir == 0 ? 1 : dir == 2 ? -1 : 0);
      const auto ny = y + (dir == 1 ? 1 : dir == 3 ? -1 : 0);
      if (nx >= 0 and nx < W and ny >= 0 and ny < H) {
        queue.push({nx, ny, dir, run+1, heat_loss + (map[ny][nx] - '0'),
                    heuristic(nx, ny)});
      }
    }
    if (run >= 3) {
      /// Turn left
      {
        const auto ndir = (dir + 4 - 1) % 4;
        const auto nx = x + (ndir == 0 ? 1 : ndir == 2 ? -1 : 0);
        const auto ny = y + (ndir == 1 ? 1 : ndir == 3 ? -1 : 0);
        if (nx >= 0 and nx < W and ny >= 0 and ny < H) {
          queue.push({nx, ny, ndir, 0, heat_loss + (map[ny][nx] - '0'),
                      heuristic(nx, ny)});
        }
      }
      /// Turn right
      {
        const auto ndir = (dir + 1) % 4;
        const auto nx = x + (ndir == 0 ? 1 : ndir == 2 ? -1 : 0);
        const auto ny = y + (ndir == 1 ? 1 : ndir == 3 ? -1 : 0);
        if (nx >= 0 and nx < W and ny >= 0 and ny < H) {
          queue.push({nx, ny, ndir, 0, heat_loss + (map[ny][nx] - '0'),
                      heuristic(nx, ny)});
        }
      }
    }
  }

  return EXIT_SUCCESS;
}

