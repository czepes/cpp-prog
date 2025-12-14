#ifndef TUPLE_PRINT_H
#define TUPLE_PRINT_H

#include <iostream>
#include <tuple>
#include <utility>

using namespace std;

template <typename... Args>
void print_tuple(ostream &, const tuple<Args...> &, index_sequence<>) {}

template <typename... Args, size_t I, size_t... Is>
void print_tuple(ostream &os, const tuple<Args...> &t,
                 index_sequence<I, Is...>) {
  os << std::get<I>(t);
  if constexpr (sizeof...(Is) > 0) {
    os << ", ";
    print_tuple(os, t, index_sequence<Is...>{});
  }
}

template <typename... Args>
auto &operator<<(ostream &os, const tuple<Args...> &t) {
  os << "(";
  print_tuple(os, t, index_sequence_for<Args...>{});
  os << ")";
  return os;
}

#endif
