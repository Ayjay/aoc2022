#include "aoc.h"

#include <tuple>
#include <unordered_set>
#include <cmath>
#include <array>

#include <boost/container_hash/hash.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using ascii::space;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(R 4
U 4
L 3
D 1
R 4
D 1
L 5
R 2)", 13, 1},
{R"(R 5
U 8
L 8
D 3
R 17
D 10
L 25
U 20)", -1, 36}
};

using point_t = std::tuple<int, int>;
int& x(point_t& p) { return std::get<0>(p); };
int& y(point_t& p) { return std::get<1>(p); };

auto run_a(std::string_view s) {
    auto moves = std::vector<std::tuple<char, int>>{};
    phrase_parse(s.begin(), s.end(),
        *(char_ >> int_),
        space, moves);

    std::unordered_set<point_t, boost::hash<point_t>> tail_log;

    auto head = point_t{};
    auto tail = point_t{};
    tail_log.insert(tail);

    for (auto [dir,steps] : moves) {
        for (auto _ : rv::iota(0,steps)) {
            switch (dir) {
            case 'U': y(head) += 1; break;
            case 'D': y(head) -= 1; break;
            case 'R': x(head) += 1; break;
            case 'L': x(head) -= 1; break;
            }

            auto x_dist = x(head) - x(tail);
            auto y_dist = y(head) - y(tail);
            if (abs(x_dist) > 1 || abs(y_dist) > 1) {
                if (x_dist)
                    x(tail) += x_dist / abs(x_dist);
                if (y_dist)
                    y(tail) += y_dist / abs(y_dist);
            }
            tail_log.insert(tail);
        }
    }

    return tail_log.size();
}

auto run_b(std::string_view s) {
    auto moves = std::vector<std::tuple<char, int>>{};
    phrase_parse(s.begin(), s.end(),
        *(char_ >> int_),
        space, moves);

    std::unordered_set<point_t, boost::hash<point_t>> tail_log;

    std::array<point_t, 10> knots;
    tail_log.insert(knots.back());

    auto drag = [](auto leader, auto& follower) {
        auto x_dist = x(leader) - x(follower);
        auto y_dist = y(leader) - y(follower);
        if (abs(x_dist) > 1 || abs(y_dist) > 1) {
            if (x_dist)
                x(follower) += x_dist / abs(x_dist);
            if (y_dist)
                y(follower) += y_dist / abs(y_dist);
        }
    };

    for (auto [dir,steps] : moves) {
        for (auto _ : rv::iota(0,steps)) {
            switch (dir) {
            case 'U': y(knots[0]) += 1; break;
            case 'D': y(knots[0]) -= 1; break;
            case 'R': x(knots[0]) += 1; break;
            case 'L': x(knots[0]) -= 1; break;
            }
            for (auto pair : rv::zip(
                ranges::subrange(knots.begin(), knots.end() - 1),
                ranges::subrange(knots.begin() + 1, knots.end()))) {
                auto& [leader, follower] = pair;
                drag(leader, follower);
            }
            tail_log.insert(knots.back());
        }
    }

    return tail_log.size();
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
