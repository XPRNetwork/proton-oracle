#include <atom/atom.hpp>

namespace proton
{
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
      for (int i = 0; i < points.size(); i++) {
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