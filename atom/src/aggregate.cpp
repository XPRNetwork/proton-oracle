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
    } else if (aggregate_function == Aggregates::MEAN_MEDIAN) {
      return calculate_mean_median(points);
    }  else if (aggregate_function == Aggregates::MEAN) {
      return calculate_mean(points);
    } else if (aggregate_function == Aggregates::SD) {
      return calculate_sd(points);
    } else {
      eosio::check(false, "aggregate function not set");
      return {};
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

  // Take mean per provider, and take the median of means
  data_variant atom::calculate_mean_median(const std::vector<ProviderPoint>& data) {
    std::map<eosio::name, std::vector<ProviderPoint>> pointsByProvider;
    for (auto const & x : data) {
      pointsByProvider[x.provider].emplace_back(x);
    }

    std::vector<ProviderPoint> providerMeans;
    for (auto const& [provider, providerPoints] : pointsByProvider) {
      ProviderPoint point = {
        .provider = provider,
        .time = eosio::current_time_point(),
        .data = calculate_mean(providerPoints)
      };
      providerMeans.emplace_back(point);
    }

    return calculate_median(providerMeans);
  }

  data_variant atom::calculate_mean(const std::vector<ProviderPoint>& data) {
    // Initialize
    double total;
    double mean;    

    // Add up total
    for (auto& item: data) {
      total += item.data.get<double>();
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
      vec.emplace_back(i.data.get<T>());
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
    auto data_type = data.back().data.data_type();
    if (data_type == "uint64_t") {
      return calculate_median_impl<uint64_t>(data);
    } else if (data_type == "double") {
      return calculate_median_impl<double>(data);
    } else {
      eosio::check(false, "unsupported median type");
      return {};
    }
  }

  data_variant atom::calculate_sd(const std::vector<ProviderPoint>& data) {
    auto mean = calculate_mean(data);
    double mean1 = mean.get<double>();

    double total = 0;
    for (auto& i: data) {
      total += std::pow(i.data.get<double>() - mean1, 2);
    }
    double mean2 = total / (double)data.size();
    
    auto res = std::sqrt(mean2);
    return data_variant{res};
  }
} // namepsace contract