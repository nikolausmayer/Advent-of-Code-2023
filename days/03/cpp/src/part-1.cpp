
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
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

  auto result{0ul};

  const auto H = static_cast<std::int32_t>(map.size());
  const auto W = static_cast<std::int32_t>(map[0].size());
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
                  s != '\n' and s != '.') 
                is_part_number = true;
            }
          }
        }
      } else {
        if (is_part_number)
          result += number;
        number = 0;
        is_part_number = false;
      }
    }
    /// Special case: number touches right border of map
    if (is_part_number)
      result += number;
  }

  std::cout << "Sum of part numbers: " << result << "\n";

  return EXIT_SUCCESS;
}

