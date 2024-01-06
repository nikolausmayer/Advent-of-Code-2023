
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
  std::map<std::array<int,2>,bool> map_position_to_tile;
  int sx, sy, W, H;
  {
    auto y{0};
    for (std::string line;;) {
      std::getline(in, line);
      if (in.eof())
        break;
      for (auto x{0}; x < static_cast<std::int32_t>(line.size()); ++x) {
        const auto c{line[x]};
        if (c == 'S') {
          sx = x;
          sy = y;
        }
        map_position_to_tile[{x,y}] = (c != '#');
        W = x+1;
      }
      ++y;
    }
    H = y;
  }

  //for (auto y = 0; y < H; ++y) {
  //  for (auto x = 0; x < W; ++x) {
  //    std::cout << (map_position_to_tile[{x,y}] ? '.' : '#');
  //  }
  //  std::cout << "\n";
  //}
  
  //{
  //  std::ofstream pgm{"input.pgm"};
  //  pgm << "P2 131 131 255";
  //  for (auto y = 0; y < H; ++y) {
  //    for (auto x = 0; x < W; ++x) {
  //      pgm << " " << (map_position_to_tile[{x,y}] * 255);
  //    }
  //  }
  //}

  ///
  /// Observations:
  /// 1. after a certain step count, reachable tiles oscillate at even/odd step
  ///    counts (a/b/a/b/...)
  /// 2. input pattern has no obstacles in direct NESW lines -> pattern size
  ///    directly yields reachability of repeat patterns
  /// 3. from 2 follows that a pattern is never reached faster than from a
  ///    corner or a center-of-a-side position
  /// 4. from 3 follows that only reachabilities for those points must be
  ///    considered -- the manhattan nature of the world means that paths
  ///    reaching a pattern later at another point are ALWAYS redundant
  /// 5. the input has size 131x131
  /// 6. new patterns are reached at even/odd regimes alternatingly, in rings
  ///    moving outwards
  /// 7. from 6 follows that both even/odd regimes are active at all times
  /// 8. the task step count is 26501365 = 202300*131+65
  /// 9. from 8 follows that in a straight line the step count is exactly
  ///    enough to cross to an "S" 202300 patterns over and then travel up to
  ///    the edge of that pattern -- but NOT to fully explore that pattern
  /// 10.from 8 follows that
  ///    - most patterns are fully explored
  ///    - some patterns are a
  ///

  std::cout << sx << " " << sy << "\n";

  std::set<std::array<int,2>> from, into, all_a, all_b;
  from.insert({sx,sy});
  for (auto step_limit = 1; step_limit <= 4*131+65; ++step_limit) {
    into.clear();
    for (auto [x,y]: from) {
      for (const auto [dx,dy]: std::vector<std::array<int,2>>{{0,-1},{1,0},{0,1},{-1,0}}) {
        const auto nx{x + dx};
        const auto ny{y + dy};
        if (map_position_to_tile[{((nx%W)+W)%W,((ny%H)+H)%H}] and
            not all_b.contains({nx,ny})) {
          into.insert({nx, ny});
          all_b.insert({nx,ny});
        }
      }
    }
    //if (step_limit != 0 and (step_limit-65) % 131 == 0)
    //  std::cout << step_limit << " " << into.size() << " | " << all_b.size() << "\n";
    std::swap(from, into);
    std::swap(all_a, all_b);

    //{
    //  std::ostringstream oss;
    //  oss << "n-" << std::setw(4) << std::setfill('0') << step_limit << ".pgm";
    //  std::ofstream pgm{oss.str()};
    //  pgm << "P2 " << 2*500+1+sx << " " << 2*500+1+sy << " 255";
    //  for (auto y = -500; y <= 500+sx; ++y) {
    //    for (auto x = -500; x <= 500+sy; ++x) {
    //      pgm << " " << (all_a.contains({x,y}) * 255);
    //    }
    //  }
    //}

    //auto unreachable{0ull}, tiles{0ull};
    //for (auto y = 0; y < H; ++y) {
    //  for (auto x = 0; x < W; ++x) {
    //    if (map_position_to_tile[{y,x}]) {
    //      ++tiles;
    //      if (not goal_tiles.contains({x,y})) {
    //        ++unreachable;
    //        //std::cout << "X";
    //      } else {
    //        //std::cout << " ";
    //      }
    //    } else {
    //      //std::cout << " ";
    //    }
    //  }
    //  //std::cout << "\n";
    //}
    //std::cout << step_limit << ": unreachable " << unreachable << "/" << tiles << "\n";


    if ((step_limit - 65) % 131 == 0) {
      std::cout << (step_limit - 65) / 131 << "\n";
      std::vector<std::set<std::array<int,2>>> patterns;
      std::map<int,int> patcnt, popcnt;
      const auto rep{15};
      auto all = 0ull;
      //std::map<std::array<int,2>,int> expl;
      for (auto py = -rep; py <= rep; ++py) {
        for (auto px = -rep; px <= rep; ++px) {

          const auto tpy = py * 131 + 65;
          const auto tpx = px * 131 + 65;

          std::set<std::array<int,2>> this_pattern;
          const auto S = 65;
          for (auto dy = -S; dy <= S; ++dy) {
            for (auto dx = -S; dx <= S; ++dx) {
              const auto y = tpy + dy;
              const auto x = tpx + dx;

              //if ((step_limit - 65) / 131 == 6)
              //  ++expl[{x,y}];
              
              if (all_a.contains({x,y})) {
                this_pattern.insert({dx,dy});
                ++all;
              }
            }
          }

          if (const auto pos = std::ranges::find(patterns, this_pattern); pos != patterns.end()) {
            //std::cout << " " << std::distance(patterns.begin(), pos);
          } else {
            patterns.push_back(this_pattern);
            //std::cout << "." << patterns.size() - 1;
          }
          ++patcnt[std::distance(patterns.begin(), std::ranges::find(patterns, this_pattern))];

          popcnt[std::distance(patterns.begin(), std::ranges::find(patterns, this_pattern))] = this_pattern.size();
        }
        //std::cout << "\n";
      }
      std::cout << "ALL " << all << "\n";

      //if ((step_limit - 65) / 131 == 6) {
      //  std::ostringstream oss;
      //  oss << "GNI.pgm";
      //  std::ofstream pgm{oss.str()};
      //  pgm << "P2 " << 2*500+1+sx << " " << 2*500+1+sy << " 255";
      //  for (auto y = -500; y <= 500+sx; ++y) {
      //    for (auto x = -500; x <= 500+sy; ++x) {
      //      pgm << " " << (expl[{x,y}]* 100);
      //    }
      //  }
      //}

      for (const auto [p,c]: patcnt)
        std::cout << p << ": " << c << " | " << popcnt[p] << "\n";

      const auto N = static_cast<std::int64_t>((step_limit - 65) / 131);
      //std::cout << (N+1)*(N+1)         * 3776ll +
      //             (((N+1)*(N+1)-1)/4) * 3659ll +
      //             (((N+1)*(N+1)-1)/4) * 3664ll +
      //              N*N                * 3642ll +
      //             (N/2)*(N/2)         * 3795ll +
      //             (((N+1)*(N+1)-1)/4) * 3664ll +
      //             (N/2)*(N/2)         * 3790ll +
      //             (N/2)*(N/2)         * 3790ll +
      //             (N/2)*(N/2)         * 3795ll +
      //             (((N+1)*(N+1)-1)/4) * 3659ll
      //          << std::endl
      //          << all_a.size()
      //          << std::endl;
      std::cout << N           * 954  +
                   1           * 5594 +
                   N           * 947  +
                   (N-1)       * 6501 +
                   N*N         * 7450 +
                   (N-1)       * 6514 +
                   (N-1)*(N-1) * 7421 +
                   1           * 5592 +
                   1           * 5605 +
                   N           * 948  +
                   (N-1)       * 6512 +
                   (N-1)       * 6512 +
                   N           * 959  +
                   1           * 5603
                << std::endl
                << all_a.size()
                << std::endl;

    }

  }

  const auto N = 202300ull;
  std::cout << N           * 954  +
               1           * 5594 +
               N           * 947  +
               (N-1)       * 6501 +
               N*N         * 7450 +
               (N-1)       * 6514 +
               (N-1)*(N-1) * 7421 +
               1           * 5592 +
               1           * 5605 +
               N           * 948  +
               (N-1)       * 6512 +
               (N-1)       * 6512 +
               N           * 959  +
               1           * 5603
            << std::endl;



  return EXIT_SUCCESS;
}

