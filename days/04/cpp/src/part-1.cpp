
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  auto result{0l};

  /// Read input file
  // Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
  for (;;) {
    std::string line;
    std::getline(in, line);
    if (in.eof())
      break;

    auto card_value{1};

    auto ID{0};
    std::set<std::int32_t> winning_numbers;

    /// Parse line
    std::istringstream iss{line};
    iss.ignore(5);  /// "Card "
    iss >> ID;
    iss.ignore(2);  /// ": "
    while (iss.peek() != '|') {
      std::int32_t n;
      iss >> n;
      winning_numbers.insert(n);
      iss.get();  /// whitespace
    }
    iss.get();  /// '|'
    for (;;) {
      iss.get();  /// whitespace
      if (iss.eof())
        break;
      std::int32_t n;
      iss >> n;

      /// Double card worth for every winning number (this is 1 doubling too
      /// many, we'll fix that below)
      if (winning_numbers.contains(n))
        card_value <<= 1;
    }

    /// Add card's worth to result (this is where we undo 1 of the doublings;
    /// this also makes (card_value == 0) if there is no winning number.
    result += card_value / 2;
  }

  std::cout << "Sum of card worths: " << result << "\n";

  return EXIT_SUCCESS;
}

