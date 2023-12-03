
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  /// Read input file
  std::vector<std::string> map;
  for (;;) {
    std::string line;
    std::getline(in, line);
    if (in.eof())
      break;
    map.push_back(line);
  }

  const auto H = static_cast<std::int32_t>(map.size());
  const auto W = static_cast<std::int32_t>(map[0].size());

  std::vector<std::vector<std::set<int>>> gear_map(H, std::vector<std::set<int>>(W));
  std::map<int,int> map_number_idx_to_number;
  auto number_idx{0};

  for (auto y = 0; y < H; ++y) {
    auto number{0};
    bool is_part_number{false};
    for (auto x = 0; x < W; ++x) {
      const auto c = map[y][x];
      if (c >= '0' and c <= '9') {
        /// Parse number digit by digit
        number = number * 10 + (c - '0');
        /// Scan for adjacent "symbol"
        for (auto dy = -1; dy <= 1; ++dy) {
          for (auto dx = -1; dx <= 1; ++dx) {
            if (y+dy >= 0 and y+dy < H and
                x+dx >= 0 and x+dx < W) {
              const auto s = map[y+dy][x+dx];
              if ((s < '0' or s > '9') and
                  s != '\n' and s != '.') {
                is_part_number = true;
                /// Gear ratio
                if (s == '*') {
                  gear_map[y+dy][x+dx].insert(number_idx);
                }
              }
            }
          }
        }
      } else {
        if (is_part_number)
          map_number_idx_to_number[number_idx++] = number;
        number = 0;
        is_part_number = false;
      }
    }
    /// Special case: number touches right border of map
    if (is_part_number) {
      map_number_idx_to_number[number_idx++] = number;
    }
  }

  /// Find all gears (with 2 part numbers) and accumulate gear ratios
  auto gear_ratios_sum{0ul};
  for (auto y = 0; y < H; ++y) {
    for (auto x = 0; x < W; ++x) {
      const auto& set = gear_map[y][x];
      auto ratio{1ul};
      if (set.size() == 2) {
        for (auto number_idx: set) {
          ratio *= map_number_idx_to_number[number_idx];
        }
        gear_ratios_sum += ratio;
      }
    }
  }

  std::cout << "Sum of gear ratios: " << gear_ratios_sum << "\n";

  return EXIT_SUCCESS;
}

