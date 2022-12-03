//
// Created by stepavly on 27.11.2022.
//

#include <random>
#include <functional>
#include <vector>
#include <iostream>
#include "algos.h"
#include "omp.h"

template<class T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &data) {
  bool first = true;
  out << "[";
  for (const T &value: data) {
    if (!first) {
      out << ", ";
    }
    out << value;
    first = false;
  }
  out << "]";
  return out;
}

template<class T>
void runTest(std::function<void(std::vector<T> &)> algo,
             std::function<std::vector<T>()> testGenerator) {

  for (int iter = 0; iter < 10'000; iter++) {
    auto testData = testGenerator();
    auto actualResult = testData;
    auto expectedResult = testData;
    std::sort(expectedResult.begin(), expectedResult.end());
    algo(actualResult);

    if (actualResult != expectedResult) {
      std::cout << "Expected and actual results differs" << std::endl;
      std::cout << "Test data: " << testData << std::endl;
      std::cout << "Expected: " << expectedResult << std::endl;
      std::cout << "Actual: " << actualResult << std::endl;
      exit(1);
    }

    if (iter == 0 || (iter + 1) % 1000 == 0) {
      std::cout << "Passed " << (iter + 1) << " iterations" << std::endl;
    }
  }
}

std::vector<int> smallIntTestGenerator() {
  static std::mt19937 rnd;
  std::vector<int> data(rnd() % 10'000);
  for (int &i: data) {
    i = rnd();
  }
  return data;
}

int main() {
//  std::vector<int> a = {0, 1, 2, 3};
//  quick_sort_par<int>(a);
//  std::cout << a << std::endl;
//  return 0;

//  runTest<int>([](std::vector<int> &data) { quick_sort<int>(data); }, smallIntTestGenerator);
  runTest<int>([](std::vector<int> &data) { quick_sort_par<int>(data); }, smallIntTestGenerator);
}