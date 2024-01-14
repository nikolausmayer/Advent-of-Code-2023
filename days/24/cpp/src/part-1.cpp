
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


int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());


  /// Read and parse input
  std::vector<std::array<std::array<double,3>,2>> hailstones;
  for (std::string line;;) {
    std::getline(in, line);
    if (in.eof())
      break;
    std::array<std::array<double,3>,2> s;
    auto it = 0ul;
    for (auto i = 0; i < 2; ++i) {
      for (auto j = 0; j < 2; ++j) {
        const auto tmp = line.find(',', it);
        s[i][j] = std::stod(line.substr(it, tmp - it));
        it = tmp+2;
      }
      {
        const auto tmp = line.find('@', it);
        s[i][2] = std::stod(line.substr(it, tmp - it));
        it = tmp+2;
      }
    }
    hailstones.push_back(s);
    std::cout << s[0][0] << " " << s[0][1] << " " << s[0][2] << " <@> "
              << s[1][0] << " " << s[1][1] << " " << s[1][2] << "\n";
  }

  auto mindim = 200000000000000.;
  auto maxdim = 400000000000000.;

  auto result = 0;
  for (auto i = 0uz; i < hailstones.size() - 1; ++i) {
    std::cout << i << "\n";
    const auto& a = hailstones[i];
    const auto slope_i = a[1][1] / a[1][0];
    for (auto j = i + 1; j < hailstones.size(); ++j) {
      std::cout << "  " << j << "   ";
      const auto& b = hailstones[j];
      const auto slope_j = b[1][1] / b[1][0];

      ///
      /// ! we only check for COURSE collision, not HAILSTONE collision
      /// ! disregard Z axis
      ///

      if (std::abs(slope_i - slope_j) < 1e-3) {
        /// Parallel or identical
        /// Try to solve b's course for a's starting position
        if (b[1][0] != 0) {
          std::cout << "a ";
          const auto t = (a[0][0] - b[0][0]) / b[1][0];
          const auto tby = b[0][1] + t * b[1][1];
          if (std::abs(a[0][1] - tby) < 1e-3) {
            ++result;
            std::cout << "identical 1";
          } else {
            std::cout << "parallel 1";
          }
          std::cout << "\n";
        } else {
          std::cout << "b ";
          const auto t = (a[0][1] - b[0][1]) / b[1][1];
          const auto tbx = b[0][0] + t * b[1][0];
          if (std::abs(a[0][0] - tbx) < 1e-3) {
            ++result;
            std::cout << "identical 2";
          } else {
            std::cout << "parallel 2";
          }
          std::cout << "\n";
        }
      } else {
        std::cout << "c ";
        /// Skewed
        /// Intersect courses
        if (b[1][0] != b[1][1]) {
          const auto ta = 
            (b[0][0] - a[0][0] + b[1][0] * ((a[0][0] - b[0][0] - a[0][1] + b[0][1]) / (b[1][0]-b[1][1])))
            /
            (a[1][0] - b[1][0] * ((a[1][0]-a[1][1]) / (b[1][0]-b[1][1])));

          const auto x = a[0][0] + ta * a[1][0];
          const auto y = a[0][1] + ta * a[1][1];

          const auto tb = (b[1][0] == 0
              ? (x - b[0][0]) / b[1][0]
              : (y - b[0][1]) / b[1][1]);

          if (ta < 0 or tb < 0) {
            if (ta < 0)
              std::cout << "(PAST A) ";
            if (tb < 0)
              std::cout << "(PAST B) ";
            std::cout << "\n";
          } else {
            const auto x = a[0][0] + ta * a[1][0];
            const auto y = a[0][1] + ta * a[1][1];
            if (x >= mindim and x <= maxdim and
                y >= mindim and y <= maxdim) {
              std::cout << "collision at " << x << ", " << y << "\n";
              ++result;
            } else {
              std::cout << "collision OUTSIDE at " << x << ", " << y << "\n";
            }
          }
        } else {
          const auto tb = 
            (a[0][0] - b[0][0] + a[1][0] * ((b[0][0] - a[0][0] - b[0][1] + a[0][1]) / (a[1][0]-a[1][1])))
            /
            (b[1][0] - a[1][0] * ((b[1][0]-b[1][1]) / (a[1][0]-a[1][1])));

          const auto x = b[0][0] + tb * b[1][0];
          const auto y = b[0][1] + tb * b[1][1];

          const auto ta = (a[1][0] == 0
              ? (x - a[0][0]) / a[1][0]
              : (y - a[0][1]) / a[1][1]);

          if (ta < 0 or tb < 0) {
            if (ta < 0)
              std::cout << "(PAST A) ";
            if (tb < 0)
              std::cout << "(PAST B) ";
            std::cout << "\n";
          } else {
            const auto x = b[0][0] + tb * b[1][0];
            const auto y = b[0][1] + tb * b[1][1];
            if (x >= mindim and x <= maxdim and
                y >= mindim and y <= maxdim) {
              std::cout << "collision at " << x << ", " << y << "\n";
              ++result;
            } else {
              std::cout << "collision OUTSIDE at " << x << ", " << y << "\n";
            }
          }
        }
      }
    }
  }

  std::cout << "Intersections within test area: " << result << "\n";

  return EXIT_SUCCESS;
}

