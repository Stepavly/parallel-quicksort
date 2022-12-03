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
static void move_values(const std::vector<T> &a,
                        std::vector<T> &tempA,
                        const std::vector<char> &flags,
                        int expectedFlag,
                        size_t begin,
                        size_t end,
                        size_t toBegin) {
  for (size_t i = begin; i < end; i++) {
    if (flags[i] == expectedFlag) {
      tempA[toBegin++] = a[i];
    }
  }
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
static void quick_sort_par(std::vector<T> &a,
                           std::vector<T> &tempA,
                           std::vector<char> &flags,
                           size_t begin,
                           size_t end,
                           size_t limit,
                           size_t depth = 0) {
  if (begin == end) {
    return;
  }
  if (end - begin < limit) {
    quick_sort<T>(a, begin, end);
    return;
  }

  if (depth < 2) {
    auto midValue = a[begin];

    size_t cntLessMid = 0;
    size_t cntEqualMid = 0;
#pragma omp parallel for reduction(+:cntLessMid, cntEqualMid) shared(flags) num_threads(4)
    for (size_t i = begin; i < end; i++) {
      if (a[i] < midValue) {
        cntLessMid++;
        flags[i] = 0;
      } else if (a[i] == midValue) {
        cntEqualMid++;
        flags[i] = 1;
      } else {
        flags[i] = 2;
      }
    }

    size_t shifts[] = {begin, begin + cntLessMid, begin + cntLessMid + cntEqualMid};

#pragma omp task shared(a, tempA, flags)
    move_values(a, tempA, flags, 0, begin, end, shifts[0]);

#pragma omp task shared(a, tempA, flags)
    move_values(a, tempA, flags, 1, begin, end, shifts[1]);

#pragma omp task shared(a, tempA, flags)
    move_values(a, tempA, flags, 2, begin, end, shifts[2]);

#pragma omp taskwait

#pragma omp parallel for num_threads(4)
    for (size_t i = begin; i < end; i++) {
      a[i] = tempA[i];
    }

    // Sort part less than midValue
#pragma omp task shared(a, tempA, flags)
    quick_sort_par<T>(a, tempA, flags, begin, begin + cntLessMid, limit, depth + 1);

    // Sort part greater than midValue
#pragma omp task shared(a, tempA, flags)
    quick_sort_par<T>(a, tempA, flags, begin + cntLessMid + cntEqualMid, end, limit, depth + 1);
  } else {
    quick_sort_par<T>(a, begin, end, limit);
  }
}

template<class T>
void quick_sort_par(std::vector<T> &a) {
//  std::vector<T> tempA(a.size());
//  std::vector<char> flags(a.size());

#pragma omp parallel num_threads(4)
  {
#pragma omp single
    {
      size_t limit = 0;
      if (!a.empty()) {
        limit = a.size() / (__builtin_popcount(~size_t(0)) - 1 - __builtin_clz(a.size()));
        limit = std::max(limit, size_t(30));
      }

//#pragma omp task shared(a, tempA, flags)
//      quick_sort_par(a, tempA, flags, 0, a.size(), limit);
#pragma omp task shared(a)
      quick_sort_par(a, 0, a.size(), limit);

#pragma omp taskwait
    }
  }

//    quick_sort_par(a, 0, a.size(), limit);
}

#endif //QUICKSORT__ALGOS_H_