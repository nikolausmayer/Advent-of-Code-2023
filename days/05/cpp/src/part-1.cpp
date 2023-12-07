
#include <cassert>
#include <cstdlib>
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

  std::vector<std::uint64_t> seeds;
  std::map<std::string, std::shared_ptr<Mapping>> map_category_to_mapping;
  std::map<std::string, std::string> map_category_to_dependency;

  /// Read and parse input file (the almanac)
  {
    std::string s;
    /// Seeds list
    in >> s;
    while (in.peek() != '\n') {
      in >> s;
      seeds.push_back(std::stoul(s));
    }
    std::getline(in, s);
    std::getline(in, s);
    for (; not in.eof();) {
      /// New mapping
      auto m_ptr = std::make_shared<Mapping>();
      in >> s;
     
      /// Parse conversion categories
      /// seed-to-soil
      /// ^^^^
      m_ptr->source_category      = s.substr(0, s.find("-"));
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

      /// Build dependency graph (it's a boring graph)
      map_category_to_dependency[m_ptr->source_category] = m_ptr->destination_category;
      map_category_to_mapping[m_ptr->source_category] = m_ptr;
      //std::cout << *m_ptr << "\n";
    }
  }

  //for (const auto& [f,t]: map_category_to_dependency)
  //  std::cout << f << "->" << t << "\n";
  //std::cout << "\n";

  auto result{std::numeric_limits<std::uint64_t>::max()};

  /// Process each seed
  for (auto val: seeds) {
    std::cout << "seed " << val << "\n";
    std::string category{"seed"}; 
    for (;;) {
      val = map_category_to_mapping[category]->map(val);
      category = map_category_to_dependency[category];
      std::cout << "  -> " << category << " " << val << "\n";
      if (category == "location")
        break;
    }
    std::cout << "\n";

    result = std::min(result, val);
  }

  std::cout << "Lowest location number of any initial seed: " << result << "\n";

  return EXIT_SUCCESS;
}

