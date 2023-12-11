
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
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

  /// Parse input file
  std::vector<std::string> map;
  for (;;) {
    std::string s;
    std::getline(in, s);
    if (in.eof())
      break;
    map.push_back(s);
  }

  const auto H = static_cast<int>(map.size());
  const auto W = static_cast<int>(map[0].size());
  
  /// Find galaxies and expanding (=empty) rows
  std::vector<bool> blank_rows(H), blank_columns(W);
  std::vector<std::array<int,2>> galaxies_xy;
  for (auto y = 0; y < H; ++y) {
    auto row_blank{true};
    for (auto x = 0; x < W; ++x) {
      if (map[y][x] == '#') {
        galaxies_xy.push_back({x,y});
        row_blank = false;
      }
    }
    blank_rows[y] = row_blank;
  }
  /// Find expanding columns
  for (auto x = 0; x < W; ++x) {
    auto column_blank{true};
    for (auto y = 0; y < H; ++y) {
      if (map[y][x] == '#') {
        column_blank = false;
      }
    }
    blank_columns[x] = column_blank;
  }

  /// Print
  //{
  //  std::cout << " ";
  //  for (auto x = 0; x < W; ++x)
  //    std::cout << (blank_columns[x] ? "VV" : " ");
  //  std::cout << "\n";
  //  for (auto y = 0; y < H; ++y) {
  //    for (auto i = (blank_rows[y] ? 0 : 1); i < 2; ++i) {
  //      std::cout << (blank_rows[y] ? ">" : " ");
  //      for (auto x = 0; x < W; ++x)
  //        std::cout << map[y][x] << (blank_columns[x] ? '.' : '\0');
  //      std::cout << (blank_rows[y] ? "<" : " ");
  //      std::cout << "\n";
  //    }
  //  }
  //  std::cout << " ";
  //  for (auto x = 0; x < W; ++x)
  //    std::cout << (blank_columns[x] ? "^^" : " ");
  //  std::cout << "\n";
  //}

  /// Find and sum pairwise distances
  auto result{0ull};
  for (auto i = 0uz; i < galaxies_xy.size() - 1; ++i) {
    for (auto j = i + 1; j < galaxies_xy.size(); ++j) {
      const auto [ax, ay] = galaxies_xy[i];
      auto [bx, by] = galaxies_xy[j];
      //std::cout << i << "(" << ax << "," << ay << ")->" << j << "(" << bx << "," << by << ") ";
      auto distance{0ull};
      for (auto dx = (ax - bx >= 0 ? 1 : -1); bx != ax; bx += dx) {
        distance += (blank_columns[bx] ? 1'000'000 : 1);  /// the only change
      }
      for (auto dy = (ay - by >= 0 ? 1 : -1); by != ay; by += dy) {
        distance += (blank_rows[by] ? 1'000'000 : 1); /// the only change
      }
      result += distance;
      //std::cout << distance << "\n";
    }
  }

  std::cout << "Sum of distances in expanded universe: " << result << "\n";

  return EXIT_SUCCESS;
}

