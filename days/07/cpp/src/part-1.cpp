
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
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

  /// Parse input
  /// 32T3K 765
  /// T55J5 684
  constexpr std::string_view cards{"23456789TJQKA"};
  std::map<char,int> card_to_score;
  for (auto i = 0uz; i < cards.size(); ++i)
    card_to_score[cards[i]] = i;
  
  struct Hand 
  {
    std::string cards;
    std::uint64_t bid;
    int type;  /// 7=fiveofakind ... 1=highcard
  };
  
  std::vector<Hand> hands;
  for (;;) {
    Hand hand;
    in >> hand.cards >> hand.bid;
    if (in.eof())
      break;
    {
      std::map<char,int> cards;
      int maxcount{0};
      for (auto& c: hand.cards) {
        c = card_to_score[c] + 'a'; /// Remap cards for easy sorting below
        maxcount = std::max(maxcount, ++cards[c]);
        switch (cards.size()) {
          case 1:  hand.type = 7;                       break;
          case 2:  hand.type = (maxcount == 4 ? 6 : 5); break;
          case 3:  hand.type = (maxcount == 3 ? 4 : 3); break;
          case 4:  hand.type = 2;                       break;
          default: hand.type = 1;
        }
      }
    }
    hands.push_back(hand);
  }
  /// Sort by winning type or high-card rule; this works because we remap the
  /// (readable) card letters into alphabetically sorted letters
  std::ranges::sort(hands,
                    [](const auto& a, const auto& b) {
                      return a.type < b.type or (a.type == b.type and a.cards < b.cards);
                    });

  auto result{0ull};
  for (auto i = 0uz; i < hands.size(); ++i)
    result += (i + 1) * hands[i].bid;

  std::cout << "Total winning: " << result << "\n";

  return EXIT_SUCCESS;
}

