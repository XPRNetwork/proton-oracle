#include <atom/atom.hpp>

namespace proton
{
  void atom::addfeed (
    const std::string& name,
    const std::string& description,
    const std::string& aggregate_function,
    const std::string& data_type,
    const std::map<std::string, uint64_t>& config,
    const std::vector<eosio::name>& providers
  ) {
    require_auth(get_self());

    // Validation
    auto type_index_itr = type_index_to_aggregates.find(data_type);
    eosio::check(type_index_itr != type_index_to_aggregates.end(), "invalid type index");
    auto aggregate_itr = std::find(type_index_itr->second.begin(), type_index_itr->second.end(), aggregate_function);
    eosio::check(aggregate_itr != type_index_itr->second.end(), "invalid aggregate function");

    auto feed_itr = _feeds.emplace(get_self(), [&](auto& f) {
      f.index = _feeds.available_primary_key();
      f.name = name;
      f.description = description;
      f.aggregate_function = aggregate_function;
      f.data_type = data_type;
      f.config = config;

      auto default_time = eosio::time_point();
      for (auto& provider: providers) {
        f.providers[provider] = default_time;
      }
    });

    _data.emplace(get_self(), [&](auto& f) {
      f.feed_index = feed_itr->index;
      f.points = {};
    });
  }

  void atom::updatefeed (
    const uint64_t& feed_index,
    const std::string& name,
    const std::string& description,
    const std::string& aggregate_function,
    const std::string& data_type,
    const std::map<std::string, uint64_t>& config,
    const std::vector<eosio::name>& providers
  ) {
    require_auth(get_self());

    auto feed_itr = _feeds.require_find(feed_index, "feed does not exist");

    // Validation
    auto type_index_itr = type_index_to_aggregates.find(feed_itr->data_type);
    eosio::check(type_index_itr != type_index_to_aggregates.end(), "invalid type index");
    auto aggregate_itr = std::find(type_index_itr->second.begin(), type_index_itr->second.end(), feed_itr->aggregate_function);
    eosio::check(aggregate_itr != type_index_itr->second.end(), "invalid aggregate function");

    _feeds.modify(feed_itr, get_self(), [&](auto& f) {
      f.name = name;
      f.description = description;
      f.aggregate_function = aggregate_function;
      f.data_type = data_type;
      f.config = config;

      auto default_time = eosio::time_point();
      for (auto& provider: providers) {
        f.providers[provider] = default_time;
      }
    });
  }

  void atom::removefeed (const uint64_t& feed_index) {
    require_auth(get_self());

    auto feed = _feeds.require_find(feed_index, "feed not found");
    _feeds.erase(feed);
  }


  void atom::feed (
    const eosio::name& account,
    const uint64_t& feed_index,
    const data_variant& data
  ) {
    require_auth(account);

    auto feed_itr = _feeds.require_find(feed_index, "feed not found");
    auto data_itr = _data.require_find(feed_itr->index, "data not found");

    // Check data
    eosio::check(feed_itr->data_type == data.data_type(), "incorrect data type");

    // Find provider and last provider entry
    auto provider_itr = feed_itr->providers.find(account);
    eosio::check(provider_itr != feed_itr->providers.end(), "not a registered provider");

    // Minimum wait
    auto min_provider_wait_sec = feed_itr->config.find("min_provider_wait_sec");
    bool check_min_wait = min_provider_wait_sec != feed_itr->config.end() && min_provider_wait_sec->second > 0;
    if (check_min_wait) {
      uint64_t seconds_ago = eosio::current_time_point().sec_since_epoch() - provider_itr->second.sec_since_epoch();
      eosio::check(seconds_ago >= min_provider_wait_sec->second, "wait time too short between inserting data");
    }

    // Create point
    auto points = (*data_itr).points;
    const ProviderPoint point = { account, eosio::current_time_point(), data };

    // Insert point
    points.insert(points.begin(), point);

    // Remove points if:
    //  1. Oversize: data_window_size
    //  2. Old: data_freshness_sec
    auto data_window_size = feed_itr->config.find("data_window_size");
    bool check_window = data_window_size != feed_itr->config.end() && data_window_size->second > 0;
    auto data_freshness_sec = feed_itr->config.find("data_freshness_sec");
    bool check_freshness = data_freshness_sec != feed_itr->config.end() && data_freshness_sec->second > 0;

    while (
      points.begin() != points.end() &&
      (
        (check_window && points.size() > data_window_size->second) ||
        (check_freshness && eosio::current_time_point().sec_since_epoch() - points.back().time.sec_since_epoch() > data_freshness_sec->second)
      )
    ) {
      points.pop_back();
    }

    // Delete old entries if over provider limit
    auto data_same_provider_limit = feed_itr->config.find("data_same_provider_limit");
    bool check_same_provider_limit = data_same_provider_limit != feed_itr->config.end() && data_same_provider_limit->second > 0;
    if (check_same_provider_limit) {
      std::map<eosio::name, uint8_t> point_counts;
      for (int i = 0; i <= points.size(); i++) {
        point_counts[points[i].provider]++;
        if (point_counts[points[i].provider] > data_same_provider_limit->second) {
          points.erase(points.begin() + i);
        }
      }
    }

    // Determine and save aggregate
    _data.modify(data_itr, get_self(), [&](auto& f) {
      f.points = points;
      f.aggregate = aggregate(feed_itr->aggregate_function, points);
    });

    // Update provider last updated time
    _feeds.modify(feed_itr, get_self(), [&](auto& f) {
      f.providers[account] = eosio::current_time_point();
    });
  }
} // namepsace contract