
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  std::map<std::int32_t, std::uint64_t> map_id_to_copies;

  /// Read input file
  // Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
  for (;;) {
    std::string line;
    std::getline(in, line);
    if (in.eof())
      break;

    auto card_value{0};

    auto ID{0};
    std::set<std::int32_t> winning_numbers;

    /// Parse line
    std::istringstream iss{line};
    iss.ignore(5);  /// "Card "
    iss >> ID;
    ++map_id_to_copies[ID];
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

      if (winning_numbers.contains(n))
        ++card_value;
    }

    /// Copy following cards
    const auto multiplier{map_id_to_copies[ID]};
    std::cout << "we have " << multiplier << " of card " << ID << ", its worth is " << card_value << "\n";
    for (auto i = 1; i <= card_value; ++i) {
      map_id_to_copies[ID + i] += multiplier;
      std::cout << "  adding " << multiplier << " of " << ID+i << "\n";
    }
  }


  auto result{0ull};
  for (const auto& [_, copies]: map_id_to_copies)
    result += copies;

  std::cout << "Total count of scratchcards: " << result << "\n";

  return EXIT_SUCCESS;
}

