#include <atom/atom.hpp>

namespace proton
{
  data_variant atom::aggregate (
    const std::string& aggregate_function,
    const std::vector<ProviderPoint>& points
  ) {
    if (aggregate_function == Aggregates::MODE) {
      return calculate_mode(points);
    } else if (aggregate_function == Aggregates::LAST) {
      return calculate_last(points);
    } else if (aggregate_function == Aggregates::MEDIAN) {
      return calculate_median(points);
    } else if (aggregate_function == Aggregates::MEAN) {
      return calculate_mean(points);
    } else if (aggregate_function == Aggregates::SD) {
      return calculate_sd(points);
    } else {
      eosio::check(false, "aggregate function not set");
    }
  }

  data_variant atom::calculate_mode(const std::vector<ProviderPoint>& data) {
    std::map<data_variant, int8_t> counts;
    int8_t max_count = 0;
    data_variant max_item;
    for (const auto& data_item: data) {
      counts[data_item.data]++;
      if (counts[data_item.data] > max_count) {
        max_item = data_item.data;
        max_count = counts[data_item.data];
      }
    }
    return max_item;
  }

  data_variant atom::calculate_last(const std::vector<ProviderPoint>& data) {
    return data.front().data;
  }

  data_variant atom::calculate_mean(const std::vector<ProviderPoint>& data) {
    // Initialize
    double total;
    double mean;    

    // Add up total
    for (auto& item: data) {
      total += std::get<double>(item.data);
    }
    mean = total / (double)data.size();

    return data_variant{mean};
  }

  template<typename T>
  data_variant calculate_median_impl(const std::vector<ProviderPoint>& data) 
  {
    std::vector<T> vec;
    for (auto& i: data) 
    {
      vec.emplace_back(std::get<T>(i.data));
    }

    if (vec.size() % 2 == 0) {
      const auto median_it1 = vec.begin() + vec.size() / 2 - 1;
      const auto median_it2 = vec.begin() + vec.size() / 2;

      std::nth_element(vec.begin(), median_it1 , vec.end());
      const auto e1 = *median_it1;

      std::nth_element(vec.begin(), median_it2 , vec.end());
      const auto e2 = *median_it2;

      return (e1 + e2) / 2;
    } else {
      const auto median_it = vec.begin() + vec.size() / 2;
      std::nth_element(vec.begin(), median_it , vec.end());
      return *median_it;
    }
  }

  data_variant atom::calculate_median(const std::vector<ProviderPoint>& data) {
    auto index = data.back().data.index();
    if (index == 1) {
      return calculate_median_impl<uint64_t>(data);
    } else if (index == 2) {
      return calculate_median_impl<double>(data);
    } else {
      eosio::check(false, "unsupported median type");
    }
  }

  data_variant atom::calculate_sd(const std::vector<ProviderPoint>& data) {
    auto mean = calculate_mean(data);
    double mean1 = std::get<double>(mean);

    double total = 0;
    for (auto& i: data) {
      total += std::pow(std::get<double>(i.data) - mean1, 2);
    }
    double mean2 = total / (double)data.size();
    
    auto res = std::sqrt(mean2);
    return data_variant{res};
  }
} // namepsace contract