
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
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct Part
{
  int x,m,a,s;
};
struct Rule
{
  std::function<bool(Part)> predicate;
  std::string destination;
};
struct Workflow
{
  std::vector<Rule> rules;
};

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  /// Read input file
  std::vector<std::string> workflow_strings, part_ratings;
  {
    auto* target = &workflow_strings;
    for (std::string s;;) {
      std::getline(in, s);
      if (in.eof()) {
        break;
      }
      if (s == "") {
        target = &part_ratings;
        continue;
      }
      target->push_back(s);
    }
  }

  /// Parse workflows
  std::map<std::string, Workflow> map_name_to_workflow;
  for (auto s: workflow_strings) {
    /// Extract workflow name and collection-of-rules
    const auto name = s.substr(0, s.find('{'));
    //std::cout << s << " --> " << name;
    s = s.substr(name.size() + 1, s.size() - name.size() - 2);
    
    Workflow w;

    /// Parse workflow rules
    Rule r;
    std::string buffer, tested, rule;
    for (auto c: s) {
      if (c == ',') {
        /// Rule complete
        r.destination = buffer;
        buffer = "";
        w.rules.push_back(r);
        r = Rule{};
      } else if (c == '<' or c == '>') {
        tested = buffer;
        rule = c;
        buffer = "";
      } else if (c == ':') {
        /// Condition complete
        const auto value = std::stoi(buffer);
        r.predicate = [=](const Part& part) {
          const auto& ref = (tested == "x" ? part.x :
                             tested == "m" ? part.m :
                             tested == "a" ? part.a :
                                             part.s);
          if (rule == ">") {
            return ref > value;
          } else {
            return ref < value;
          }
        };
        buffer = "";
      } else {
        buffer += c;
      }
    }
    /// Store default rule
    //std::cout << "  DEF<" << buffer << "> ";
    r.predicate = [](const auto&) { return true; };
    r.destination = buffer;
    w.rules.push_back(r);

    map_name_to_workflow[name] = w;
    //std::cout << "\n";
  }

  /// Parse parts
  std::vector<Part> parts;
  for (auto s: part_ratings) {
    std::vector<int> elements;
    std::optional<int> v;
    for (auto c: s) {
      if (c >= '0' and c <= '9') {
        if (not v)
          v = 0;
        v = v.value() * 10 + (c - '0');
      } else {
        if (v)
          elements.push_back(v.value());
        v.reset();
      }
    }
    //for (auto x: elements)
    //  std::cout << x << " ";
    //std::cout << "\n";
    parts.push_back({elements[0], elements[1], elements[2], elements[3]});
  }

  auto result{0ull};

  /// Process part
  for (const auto& part: parts) {
    std::string workflow_name{"in"};
    for (;;) {
      if (workflow_name == "A") {
        result += part.x + part.m + part.a + part.s;
        break;
      } else if (workflow_name == "R") {
        break;
      }
      auto const* const workflow_ptr = &map_name_to_workflow[workflow_name];
      for (const auto& rule: workflow_ptr->rules) {
        if (rule.predicate(part)) {
          workflow_name = rule.destination;
          workflow_name = rule.destination;
          break;
        }
      }
    }
  }

  std::cout << "Sum of rating numbers for all accepted parts: " << result << "\n";


  return EXIT_SUCCESS;
}

