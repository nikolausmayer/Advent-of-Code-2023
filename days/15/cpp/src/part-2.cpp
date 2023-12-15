
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
  const auto input = [&]{
    std::string s;
    std::getline(in, s);
    return s;
  }();

  std::array<std::list<std::pair<std::vector<char>, char>>, 256> boxes;
  auto printer = [&]{
    for (auto i{0}; i < 256; ++i) {
      const auto& lenses{boxes[i]};
      if (lenses.size() == 0)
        continue;
      std::cout << std::setw(3) << i << ": ";
      for (const auto& [label, focal_length]: lenses)
        std::cout << "[" << std::string{label.data(),2} << " " << focal_length << "] ";
      std::cout << "\n";
    }
  };

  /// Perform initialization procedure steps
  std::vector<char> lens_label;
  auto hash{0};
  for (auto c: input) {
    if (c == ',') {
      lens_label.clear();
      hash = 0;
    } else {
      if (c >= 'a' and c <= 'z') {
        /// HASH only over lens label
        hash = ((hash + c) * 17) % 256;
        lens_label.push_back(c);
        //printer();
      } else if (c == '-') {
        /// Remove lens from box
        std::cout << "remove lens >" << std::string{lens_label.data(),2} << "< from box >" << hash << "<\n";
        if (auto iter = std::ranges::find(boxes[hash], lens_label,
                                          [](const auto& a) { 
                                            return std::get<0>(a);
                                          }); iter != boxes[hash].end()) {
          boxes[hash].erase(iter);
        }
      } else if (c == '=') {
        /// NOP
      } else /* 0-9 */ {
        /// Add/replace lens with focal length "c" in box
        std::cout << "add/replace lens >" << std::string{lens_label.data(),2} << "< (" << c << ") in box >" << hash << "<\n";
        if (auto iter = std::ranges::find(boxes[hash], lens_label,
                                          [](const auto& a) { 
                                            return std::get<0>(a);
                                          }); iter != boxes[hash].end()) {
          std::get<1>(*iter) = c;
        } else {
          boxes[hash].push_back({lens_label, c});
        }
      }
    }
  }
  printer();

  /// Calculate focusing power
  auto result{0ull};
  for (auto box{0}; box < 256; ++box) {
    const auto& lenses{boxes[box]};
    if (lenses.size() == 0)
      continue;
    auto slot{1};
    for (const auto& lens: lenses) {
      result += (box + 1) * (std::get<1>(lens) - '0') * slot++;
    }
  }

  std::cout << "Lens configuration's focusing power: " << result << "\n";

  return EXIT_SUCCESS;
}

