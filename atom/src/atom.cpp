#include <atom/atom.hpp>

namespace proton
{
  void atom::addfeed (
    std::string name,
    std::string description,
    std::string aggregate_function,
    uint8_t data_type_index,
    uint8_t data_window_size,
    uint64_t min_provider_wait_sec,
    std::vector<eosio::name> providers
  ) {
    require_auth(get_self());

    // Validation
    auto type_index_itr = type_index_to_aggregates.find(data_type_index);
    eosio::check(type_index_itr != type_index_to_aggregates.end(), "invalid type index");
    auto aggregate_itr = std::find(type_index_itr->second.begin(), type_index_itr->second.end(), aggregate_function);
    eosio::check(aggregate_itr != type_index_itr->second.end(), "invalid aggregate function");
    eosio::check(data_window_size > 0, "data window size must be positive");

    auto feed_itr = _feeds.emplace(get_self(), [&](auto& f) {
      f.index = _feeds.available_primary_key();
      f.name = name;
      f.description = description;
      f.aggregate_function = aggregate_function;
      f.data_type_index = data_type_index;
      f.data_window_size = data_window_size;
      f.min_provider_wait_sec = min_provider_wait_sec;

      auto default_time = eosio::time_point();
      for (auto& provider: providers) {
        f.providers[provider] = default_time;
      }

      f.points = {};
    });
  }

  void atom::updatefeed (const Feed& feed) {
    require_auth(get_self());

    auto feed_itr = _feeds.require_find(feed.index, "feed does not exist");

    // Validation
    auto type_index_itr = type_index_to_aggregates.find(feed.data_type_index);
    eosio::check(type_index_itr != type_index_to_aggregates.end(), "invalid type index");
    auto aggregate_itr = std::find(type_index_itr->second.begin(), type_index_itr->second.end(), feed.aggregate_function);
    eosio::check(aggregate_itr != type_index_itr->second.end(), "invalid aggregate function");
    eosio::check(feed.data_window_size > 0, "data window size must be positive");

    _feeds.modify(feed_itr, get_self(), [&](auto& p) {
      p = feed;
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

    // Check data
    eosio::check(feed_itr->data_type_index == data.index(), "incorrect data type");

    // Find provider and last provider entry
    auto provider_itr = feed_itr->providers.find(account);
    eosio::check(provider_itr != feed_itr->providers.end(), "not a registered provider");
    eosio::check(eosio::current_time_point().sec_since_epoch() - provider_itr->second.sec_since_epoch() >= feed_itr->min_provider_wait_sec, "wait time too short between inserting data");

    // Enter points
    auto points = (*feed_itr).points;
    const ProviderPoint point = { account, data, eosio::current_time_point() };
    points.insert(points.begin(), point);
    if (points.size() > feed_itr->data_window_size) {
      points.pop_back();
    }

    // Determine and save aggregate
    _feeds.modify(feed_itr, get_self(), [&](auto& f) {
      f.points = points;
      f.aggregate = aggregate(feed_itr->aggregate_function, points);
    });
  }
} // namepsace contract