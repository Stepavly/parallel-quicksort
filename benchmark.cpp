#include <random>
#include <functional>
#include <vector>
#include <iostream>
#include <chrono>
#include "algos.h"

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

static std::mt19937 rnd(0);

std::string smallStringGenerator() {
  std::string s(rnd() % 20, 'a');
  for (size_t i = 0; i < s.size(); i++) {
    s[i] = char(rnd() % 256);
  }
  return s;
}

std::vector<std::string> bigIntTestGenerator() {
  std::vector<std::string> data(1'000'000);
  for (auto &i: data) {
    i = smallStringGenerator();
  }
  return data;
}

static int ITERATIONS = 1;

std::vector<long> benchmark() {
  std::vector<long> iterationTimes(ITERATIONS);
  for (int iter = 0; iter < ITERATIONS; iter++) {
    auto testData = bigIntTestGenerator();
    std::cout << "Test generated" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    quick_sort(testData);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    iterationTimes[iter] = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "Iteration " << (iter + 1) << " took " << iterationTimes[iter] << " ms" << std::endl;
  }
  std::sort(iterationTimes.begin(), iterationTimes.end());
  return iterationTimes;
}

std::vector<long> benchmark_par() {
  std::vector<long> iterationTimes(ITERATIONS);

//#pragma omp parallel num_threads(5) shared(iterationTimes, rnd)
  {
//#pragma omp master
    {
      for (int iter = 0; iter < ITERATIONS; iter++) {
        auto testData = bigIntTestGenerator();
        std::cout << "Test generated" << std::endl;
        double begin = omp_get_wtime();

        quick_sort_par(testData);

        double end = omp_get_wtime();
        iterationTimes[iter] = (long) ((end - begin) * 1000);
        std::cout << "Iteration " << (iter + 1) << " took " << iterationTimes[iter] << " ms" << std::endl;
      }
    }
  }
  std::sort(iterationTimes.begin(), iterationTimes.end());
  return iterationTimes;
}

void prettifyStatistics(std::vector<long> statistics) {
  std::cout.precision(4);
  std::cout << std::fixed;
  std::cout << "All iterations in ms " << statistics << std::endl;
  std::cout << "Max time " << *std::max_element(statistics.begin(), statistics.end()) << " ms" << std::endl;
  std::cout << "Min time " << *std::min_element(statistics.begin(), statistics.end()) << " ms" << std::endl;
  std::cout << "Mean time " << std::accumulate(statistics.begin(), statistics.end(), 0l) / double(statistics.size())
            << " ms" << std::endl;
  std::cout << std::endl;
}

int main() {
//  omp_set_dynamic(0);
//  omp_set_num_threads(4);

  std::cout << "Parallel algo" << std::endl;
  prettifyStatistics(benchmark_par());

  rnd = std::mt19937(0);

  std::cout << "Sequential algo" << std::endl;
  prettifyStatistics(benchmark());
  return 0;
}
