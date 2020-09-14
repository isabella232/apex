#include <apex/detail/prelude/begin.hpp>
#include <initializer_list>
#include <vector>
#include <string>

TEST_CASE("ranges::begin member function") {
  std::vector<int> x { 1, 2, 3, 4 };
  CHECK(apex::ranges::begin(x) == x.begin());
}

TEST_CASE("ranges::begin array access") {
  int x[4] = { 1, 2, 3, 4 };
  CHECK(*apex::ranges::begin(x) == x[0]);
}
