
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

struct Pulse
{
  std::string from, to;
  bool is_high;
};
std::ostream& operator<<(std::ostream& os, const Pulse& p)
{
  os << p.from << " -" << (p.is_high ? "high" : "low") << "-> " << p.to;
  return os;
}

struct Module
{
  enum Type {
    FLIPFLOP,
    CONJUNCTION,
    BROADCASTER,
  } type{BROADCASTER};

  std::string name;
  std::vector<std::string> inputs, outputs;
  std::map<std::string, bool> conjunction_input_states;
  bool flipflop_state_is_on{false};

  std::vector<Pulse> process(Pulse p)
  {
    //std::cout << name << " got " << (p.is_high ? "high" : "low") << " from " << p.from << "\n";

    std::vector<Pulse> send;
    switch (type) {
      case FLIPFLOP: {
        if (not p.is_high) {
          flipflop_state_is_on = !flipflop_state_is_on;
          for (auto out: outputs)
            send.push_back({name, out, flipflop_state_is_on});
        }
        break;
      }
      case CONJUNCTION: {
        conjunction_input_states[p.from] = p.is_high;
        auto all_high{true};
        for (const auto& in: inputs)
          all_high &= conjunction_input_states[in];
        for (auto out: outputs)
          send.push_back({name, out, not all_high});
        break;
      }
      default: {
        /// Broadcaster sends LOW to all
        for (auto out: outputs)
          send.push_back({name, out, false});
      }
    }
    return send;
  };
};
std::ostream& operator<<(std::ostream& os, const Module& m)
{
  os << "(" << (m.type == Module::Type::FLIPFLOP
                ? (m.flipflop_state_is_on ? "%1" : "%0") :
                m.type == Module::Type::CONJUNCTION ? "&" : "")
      << ")" << m.name << ": ( ";
  for (auto in: m.inputs)
    os << in << (m.type == Module::Type::CONJUNCTION
                 ? (m.conjunction_input_states.at(in) ? "1" : "0")
                 : "") << " ";
  os << ") -> ( ";
  for (auto in: m.outputs)
    os << in << " ";
  os << ")";
  return os;
}

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  /// Parse input file and connect modules outputs
  std::map<std::string, Module> map_name_to_module;
  {
    Module m{};
    std::string buffer;
    for (char c = in.get(); not in.eof(); c = in.get()) {
      switch (c) {
        case '%': { 
          m.type = Module::Type::FLIPFLOP;
          break;
        }
        case '&': {
          m.type = Module::Type::CONJUNCTION;
          break;
        }
        case ',': {
          m.outputs.push_back(buffer);
          buffer = "";
          break;
        }
        case '\n': {
          m.outputs.push_back(buffer);
          map_name_to_module[m.name] = m;
          m = Module{};
          buffer = "";
          break;
        }
        case ' ': { 
          if (m.name == "")
            m.name = buffer;
          buffer = "";
          break;
        }
        default: {
          buffer += c;
        }
      }
    }
  }

  /// Connect module inputs
  for (const auto& [n,m]: map_name_to_module) {
    for (auto out: m.outputs) {
      if (auto& other = map_name_to_module[out];
          std::ranges::find(other.inputs, n) == other.inputs.end()) {
        other.inputs.push_back(n);
        if (other.type == Module::Type::CONJUNCTION)
          other.conjunction_input_states[n] = false;
      }
    }
  }

  //for (const auto& [n,m]: map_name_to_module)
  //  std::cout << "[" << n << "] " << m << "\n";
  //std::cout << "\n";

  auto low_pulses{0ull}, high_pulses{0ull};
  std::queue<Pulse> q;
  for (auto cycle = 0; cycle < 1000; ++cycle) {
    //std::cout << "\n";
    /// Push the button
    q.push({"(BUTTON)", "broadcaster", false});
    while (q.size()) {
      const auto p = q.front();
      q.pop();
      //std::cout << p << "\n";

      low_pulses += (not p.is_high);
      high_pulses += p.is_high;

      for (auto new_pulse: map_name_to_module[p.to].process(p))
        q.push(new_pulse);
      //std::cout << map_name_to_module[p.to] << "\n";
    }
  }

  std::cout << "Low pulses * high pulses: " << low_pulses * high_pulses << "\n";

  return EXIT_SUCCESS;
}

