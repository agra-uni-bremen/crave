#pragma once

#include "../RandomBase.hpp"

#include <boost/foreach.hpp>
#include <boost/type_traits/is_signed.hpp>

#include <algorithm>
#include <bitset>
#include <limits>
#include <vector>

#include <ctime>

namespace crave {

template<typename T>
class rand_obj_gen {

  typedef std::bitset<sizeof(T) << 3> Bitset;
  typedef std::vector<Bitset> Solution;
  typedef std::vector<Solution> Solutions;
  typedef std::vector<std::clock_t> Times;

public:
  rand_obj_gen(unsigned int, rand_obj_base*);

  bool generate();

  virtual bool next() { return obj_->next(); }
  void print_bench_values();

private:
  unsigned int number_;
  rand_obj_base* obj_;
  Solutions solutions_;
  Times elapsed_gen_times_;
  bool values_generated_;
};

template<typename T>
rand_obj_gen<T>::rand_obj_gen(unsigned int num, rand_obj_base* obj)
: number_(num), obj_(obj), solutions_(), elapsed_gen_times_(), values_generated_(false) {

  solutions_.resize(num);
  elapsed_gen_times_.resize(num);
}

template<typename T>
inline bool rand_obj_gen<T>::generate() {

  std::clock_t start;
  std::size_t num_val = obj_->numValues();

  for (unsigned int i = 0; i < number_; ++i) {

    start = std::clock();
    obj_->next();
    elapsed_gen_times_[i] = std::clock() - start;

    solutions_[i].resize(num_val);

    if (boost::is_signed<T>::value) {

      std::vector<long> values;

      obj_->gatherValues(values);
      std::copy(values.begin(), values.end(), solutions_[i].begin());

    } else {

      std::vector<unsigned long> values;

      obj_->gatherValues(values);
      std::copy(values.begin(), values.end(), solutions_[i].begin());

    }
  }
  values_generated_ = true;

  return true;
}

template<typename T>
void rand_obj_gen<T>::print_bench_values() {

  if (!values_generated_)
    throw std::runtime_error("generate values before print bench values.\n");

  unsigned int num_vars = obj_->numValues();
  std::size_t sum = 0,
              min = std::numeric_limits<std::size_t>::max(),
              max = std::numeric_limits<std::size_t>::min();
  unsigned int word_sum = 0,
               word_min = std::numeric_limits<unsigned int>::max(),
               word_max = std::numeric_limits<unsigned int>::min();
  std::clock_t elapsed_gen_time = 0,
               elapsed_min = std::numeric_limits<std::clock_t>::max(),
               elapsed_max = std::numeric_limits<std::clock_t>::min();

  for (unsigned int j = 0; j < number_; ++j) {

    elapsed_gen_time += elapsed_gen_times_[j];
    elapsed_min = (elapsed_min < elapsed_gen_times_[j])? elapsed_min: elapsed_gen_times_[j];
    elapsed_max = (elapsed_max > elapsed_gen_times_[j])? elapsed_max: elapsed_gen_times_[j];

    for (unsigned int k = j + 1; k < number_; ++k) {

      std::size_t total_distance = 0;
      std::size_t word_distance = 0;

      for (unsigned int i = 0; i < num_vars; ++i) {

        std::size_t distance = (solutions_[j][i] ^ solutions_[k][i]).count();
        word_distance  += solutions_[j][i] != solutions_[k][i];
        total_distance += distance;
      }

      sum += total_distance;
      min = (min < total_distance)? min: total_distance;
      max = (max > total_distance)? max: total_distance;
      word_sum += word_distance;
      word_min = (word_min < word_distance)? word_min: word_distance;
      word_max = (word_max > word_distance)? word_max: word_distance;
    }

  }

  std::cout << "Bitwise Hamming Distance (avg):\t " << sum / ((number_ - 1) * number_ / 2) << std::endl;
  std::cout << "Bitwise Hamming Distance (min):\t " << min << std::endl;
  std::cout << "Bitwise Hamming Distance (max):\t " << max << std::endl;
  std::cout << "Number of Variables:\t\t " << num_vars << std::endl;
  std::cout << "Wordwise Hamming Distance (avg): " << word_sum / ((number_ - 1) * number_ / 2) << std::endl;
  std::cout << "Wordwise Hamming Distance (min): " << word_min << std::endl;
  std::cout << "Wordwise Hamming Distance (max): " << word_max << std::endl;
  std::cout << "Elapsed genenration time (min):\t " << elapsed_min / double(CLOCKS_PER_SEC) << std::endl;
  std::cout << "Elapsed genenration time (max):\t " << elapsed_max / double(CLOCKS_PER_SEC) << std::endl;
  std::cout << "Elapsed genenration time (sum):\t " << elapsed_gen_time / double(CLOCKS_PER_SEC) << std::endl;
}

} // end namespace crave
