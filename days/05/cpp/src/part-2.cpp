
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct Mapping
{
  std::string source_category, destination_category;
  std::vector<std::array<std::uint64_t,3>> partial_maps;

  /// Remap a value
  std::uint64_t map(std::uint64_t input)
  {
    /// Search from matching mapping
    for (const auto& [to, from, range]: partial_maps) {
      if (input >= from and input <= from + range) {
        return to + (input - from);
      }
    }
    /// No matching mapping? -> return identity
    return input;
  }

  /// Remap a value range
  std::vector<std::array<std::uint64_t,2>> split_map(std::array<std::uint64_t,2> input) {
    std::vector<std::array<std::uint64_t,2>> result;
    auto [a, b] = input;

    auto leftovers = false;
    for (const auto& [to, from, range]: partial_maps) {
      /// No intersection, but later mappings might
      ///                  [...input...]
      /// [...mapping...]
      if (from + range - 1 < a) {
        leftovers = true;
        continue;
      }
      /// No intersection, and no later mapping will either
      /// [...input...]
      ///               [...mapping...]
      else if (from > b) {
        result.push_back({a, b});
        leftovers = false;
        break;
      }
      /// Whole input range is covered
      /// Front part intersects
      ///      [...input...]
      /// [........mapping........]
      else if (from <= a and from + range - 1 >= b) {
        result.push_back({to + a - from, to + b - from});
        leftovers = false;
        break;
      }
      /// Subset on both sides
      /// [........input........]
      ///     [...mapping...]
      /// continue with later mappings
      else if (from > a and from + range - 1 < b) {
        result.push_back({a, from - 1});
        result.push_back({to, to + range - 1});
        a = from + range;
      }
      /// Partial at front
      ///     [...input...]
      /// [...mapping...]
      /// continue with later mappings
      else if (from <= a) {
        result.push_back({to + a - from, to + range - 1});
        a = from + range;
      }
      /// Partial at back
      /// [...input...]
      ///     [...mapping...]
      else if (from > a) {
        result.push_back({a, from - 1});
        result.push_back({to, to + b - from});
        leftovers = false;
        break;
      }
      /// Coder did an error :(
      else {
        std::cout << "ERR\n";
      }
    }
    if (leftovers) {
      result.push_back({a, b});
    }

    return result;
  }
};

/// Just for development
std::ostream& operator<<(std::ostream& os, const Mapping& m)
{
  os << m.source_category << " -> " << m.destination_category << "\n";
  for (const auto& [to, from, range]: m.partial_maps) 
    os << "  " << from << "-" << from + range - 1 << " -> " << to << "-" << to + range - 1 << "\n";
  return os;
}

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  std::vector<std::array<std::uint64_t,2>> seed_ranges;
  std::map<std::string, std::shared_ptr<Mapping>> map_category_to_mapping;
  std::map<std::string, std::string> map_category_to_dependency;

  /// Read and parse input file (the almanac)
  {
    std::string s;
    /// Seeds list
    in >> s;
    while (in.peek() != '\n') {
      in >> s;
      const auto from{std::stoul(s)};
      in >> s;
      const auto length{std::stoul(s)};
      seed_ranges.push_back({from, from + length - 1});
    }
    std::ranges::sort(seed_ranges, [](const auto& a, const auto& b) {
      return a[1] < b[1];
    });

    std::getline(in, s);
    std::getline(in, s);
    for (; not in.eof();) {
      /// New mapping
      auto m_ptr = std::make_shared<Mapping>();
      in >> s;
     
      /// Parse conversion categories
      /// seed-to-soil
      /// ^^^^
      m_ptr->source_category = s.substr(0, s.find("-"));
      /// seed-to-soil
      /// .... +4 ^^^^
      m_ptr->destination_category = s.substr(m_ptr->source_category.size() + 4);
      std::getline(in, s);

      /// Parse partial maps
      while (in.peek() != '\n' and not in.eof()) {
        std::uint64_t a, b, c;
        in >> a >> b >> c;
        m_ptr->partial_maps.push_back({a, b, c});
        std::getline(in, s);
      }
      /// Empty line between mappings, or nothing if EOF
      std::getline(in, s);

      /// Sort new mapping's ranges to make ::split_map()'s job easier
      std::ranges::sort(m_ptr->partial_maps, [](const auto& a, const auto& b) {
        return a[1] < b[1];
      });

      /// Build dependency graph (it's a boring graph)
      map_category_to_dependency[m_ptr->source_category] = m_ptr->destination_category;
      map_category_to_mapping[m_ptr->source_category] = m_ptr;
      std::cout << *m_ptr << "\n";
    }
  }

  for (const auto& [f,t]: map_category_to_dependency)
    std::cout << f << "->" << t << "\n";
  std::cout << "\n";

  /// Process each seed range
  {
    std::string category{"seed"}; 
    std::cout << "seed\n";
    for (;;) {
      decltype(seed_ranges) remapped;
      std::cout << "  -> " << map_category_to_dependency[category] << "\n";
      for (auto range: seed_ranges) {
        std::cout << "    [" << range[0] << " " << range[1] << "] -> ";
        for (auto remapped_range: map_category_to_mapping[category]->split_map(range)) {
          remapped.push_back(remapped_range);
          std::cout << "[" << remapped_range[0] << " " << remapped_range[1] << "] ";
        }
        std::cout << "\n";
      }
      /// Consolidate ranges
      {
        decltype(remapped) fused;
        /// Sort
        std::ranges::sort(remapped, [](const auto& a, const auto& b) {
          return a[1] < b[1];
        });
        /// Fuse adjacent
        /// (init with first range)
        auto a{remapped[0][0]}, b{remapped[0][1]};
        for (auto [from, to]: remapped) {
          if (from <= b + 1) {
            b = to;
          } else {
            fused.push_back({a, b});
            a = from;
            b = to;
          }
        }
        fused.push_back({a, b});
        remapped = fused;
      }
      seed_ranges = remapped;
      category = map_category_to_dependency[category];
      if (category == "location")
        break;
    }
    std::cout << "\n";
  }

  std::cout << "Lowest location number of any initial seed range: " << seed_ranges[0][0] << "\n";

  return EXIT_SUCCESS;
}

