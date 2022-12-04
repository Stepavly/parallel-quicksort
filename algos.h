//
// Created by stepavly on 20.11.2022.
//

#ifndef QUICKSORT__ALGOS_H_
#define QUICKSORT__ALGOS_H_

#include <type_traits>
#include <iterator>
#include <iostream>
#include "omp.h"

template<class T>
static void quick_sort(std::vector<T> &a, size_t begin, size_t end) {
  if (begin == end) {
    return;
  }
  auto midValue = a[begin];
  size_t lessMidPos = begin;
  for (size_t i = begin; i < end; i++) {
    if (a[i] < midValue) {
      std::swap(a[i], a[lessMidPos]);
      lessMidPos++;
    }
  }
  size_t equalMidPos = lessMidPos;
  for (size_t i = equalMidPos; i < end; i++) {
    if (a[i] == midValue) {
      std::swap(a[i], a[equalMidPos]);
      equalMidPos++;
    }
  }
  quick_sort<T>(a, begin, lessMidPos);
  quick_sort<T>(a, equalMidPos, end);
}

template<class T>
void quick_sort(std::vector<T> &a) {
  quick_sort(a, 0, a.size());
}

template<class T>
static void quick_sort_par(std::vector<T> &a,
                           size_t begin,
                           size_t end,
                           size_t limit) {
  if (begin == end) {
    return;
  }
  if (end - begin < limit) {
    quick_sort<T>(a, begin, end);
    return;
  }

  auto midValue = a[begin];
  size_t lessMidPos = begin;
  for (size_t i = begin; i < end; i++) {
    if (a[i] < midValue) {
      std::swap(a[i], a[lessMidPos]);
      lessMidPos++;
    }
  }

  // Sort part less than midValue
#pragma omp task shared(a)
  quick_sort_par<T>(a, begin, lessMidPos, limit);

  size_t equalMidPos = lessMidPos;
  for (size_t i = equalMidPos; i < end; i++) {
    if (a[i] == midValue) {
      std::swap(a[i], a[equalMidPos]);
      equalMidPos++;
    }
  }

  // Sort part greater than midValue
#pragma omp task shared(a)
  quick_sort_par<T>(a, equalMidPos, end, limit);
}

template<class T>
void quick_sort_par(std::vector<T> &a) {
#pragma omp parallel num_threads(4)
  {
#pragma omp single
    {
      size_t limit = 0;
      if (!a.empty()) {
        limit = a.size() / (__builtin_popcount(~size_t(0)) - 1 - __builtin_clz(a.size()));
        limit = std::max(limit, size_t(30));
      }
#pragma omp task shared(a)
      quick_sort_par(a, 0, a.size(), limit);
    }
  }
}

#endif //QUICKSORT__ALGOS_H_