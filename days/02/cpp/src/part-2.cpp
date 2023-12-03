
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  auto result{0ul};

  for (;;) {
    std::string line;
    std::getline(in, line);
    if (in.eof())
      break;

    auto ID{0ul};

    /// Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
    auto i{5uz};  // "Game "
    for (; line[i] != ':'; ++i)
      ID = 10 * ID + (line[i] - '0');
    ++i;
    ++i;

    auto max_red{0};
    auto max_green{0};
    auto max_blue{0};

    for (; i < line.size();) {
      auto num{0};
      for (; line[i] != ' '; ++i)
        num = num * 10 + (line[i] - '0');
      ++i;
      switch (line[i]) {
        case 'r': { max_red   = std::max(max_red, num);   i += 3; break; }
        case 'g': { max_green = std::max(max_green, num); i += 5; break; }
        case 'b': { max_blue  = std::max(max_blue, num);  i += 4; break; }
      }
      ++i;
      ++i;
    }

    result += (max_red * max_green * max_blue);
  }

  std::cout << "Sum of possible games' IDs: " << result << "\n";

  return EXIT_SUCCESS;
}

