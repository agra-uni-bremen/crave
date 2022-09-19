#include "../crave/utils/RandObjGenerator.hpp"

namespace crave {

rand_obj_gen::rand_obj_gen(unsigned int num, rand_obj_base* obj)
    : number_(num), obj_(obj), solutions_(), elapsed_gen_times_(), values_generated_(false) {
  solutions_.resize(num);
  elapsed_gen_times_.resize(num);
}

bool rand_obj_gen::generate() {
  std::cout << "Generate " << number_ << " solutions" << std::endl;

  std::clock_t start;

  for (unsigned int i = 0; i < number_; ++i) {
    start = std::clock();
    if (!obj_->next()) throw std::runtime_error("Generation error.\n");
    elapsed_gen_times_[i] = std::clock() - start;

    std::vector<int64_t> values;
    obj_->gather_values(&values);
    solutions_[i].insert(solutions_[i].begin(), values.begin(), values.end());
  }

  values_generated_ = true;

  return true;
}

void rand_obj_gen::print_bench_values() {
  if (!values_generated_) generate();
  std::size_t sum = 0, min = std::numeric_limits<std::size_t>::max(), max = std::numeric_limits<std::size_t>::min();

  unsigned int word_sum = 0, word_min = std::numeric_limits<unsigned int>::max(),
               word_max = std::numeric_limits<unsigned int>::min();

  std::clock_t elapsed_gen_time = 0, elapsed_min = std::numeric_limits<std::clock_t>::max(),
               elapsed_max = std::numeric_limits<std::clock_t>::min();

  for (unsigned int j = 0; j < number_; ++j) {
    elapsed_gen_time += elapsed_gen_times_[j];
    elapsed_min = (elapsed_min < elapsed_gen_times_[j]) ? elapsed_min : elapsed_gen_times_[j];
    elapsed_max = (elapsed_max > elapsed_gen_times_[j]) ? elapsed_max : elapsed_gen_times_[j];

    for (unsigned int k = j + 1; k < number_; ++k) {
      std::size_t total_distance = 0;
      std::size_t word_distance = 0;

      if (solutions_[j].size() != solutions_[k].size()) {
        throw std::runtime_error("Number of variables mismatch.\n");
      }

      for (unsigned int i = 0; i < solutions_[j].size(); ++i) {
        std::size_t distance = (solutions_[j][i] ^ solutions_[k][i]).count();
        word_distance += solutions_[j][i] != solutions_[k][i];
        total_distance += distance;
      }

      sum += total_distance;
      min = (min < total_distance) ? min : total_distance;
      max = (max > total_distance) ? max : total_distance;
      word_sum += word_distance;
      word_min = (word_min < word_distance) ? word_min : word_distance;
      word_max = (word_max > word_distance) ? word_max : word_distance;
    }
  }

  if (number_ <= 0) return;

  std::cout << "Number of Variables:\t\t " << solutions_[0].size() << std::endl;
  std::cout << "Bitwise Hamming Distance (avg):\t " << sum / ((number_ - 1) * number_ / 2.) << std::endl;
  std::cout << "Bitwise Hamming Distance (min):\t " << min << std::endl;
  std::cout << "Bitwise Hamming Distance (max):\t " << max << std::endl;
  std::cout << "Wordwise Hamming Distance (avg): " << word_sum / ((number_ - 1) * number_ / 2.) << std::endl;
  std::cout << "Wordwise Hamming Distance (min): " << word_min << std::endl;
  std::cout << "Wordwise Hamming Distance (max): " << word_max << std::endl;
  std::cout << "Elapsed generation time (min):\t " << elapsed_min / static_cast<double>(CLOCKS_PER_SEC) << std::endl;
  std::cout << "Elapsed generation time (max):\t " << elapsed_max / static_cast<double>(CLOCKS_PER_SEC) << std::endl;
  std::cout << "Elapsed generation time (sum):\t " << elapsed_gen_time / static_cast<double>(CLOCKS_PER_SEC)
            << std::endl;
}
}  // namespace crave
