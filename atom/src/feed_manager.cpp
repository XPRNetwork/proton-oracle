#include <atom/atom.hpp>

namespace proton
{
  void atom::_setfeed (
    const eosio::name& ram_payer,
    const Feed& feed
  ) {
    // Auth
    require_auth(ram_payer);

    // General validation
    eosio::check(feed.name.size() < 256, "name too long");
    eosio::check(feed.description.size() < 256, "description too long");
    eosio::check(feed.config.size() < 25, "config too long");
    eosio::check(feed.providers.size() < 100, "too many providers");

    // Validate data type
    auto type_index_itr = type_index_to_aggregates.find(feed.data_type);
    eosio::check(type_index_itr != type_index_to_aggregates.end(), "invalid type index");

    // Validate aggregate function
    auto aggregate_itr = std::find(type_index_itr->second.begin(), type_index_itr->second.end(), feed.aggregate_function);
    eosio::check(aggregate_itr != type_index_itr->second.end(), "invalid aggregate function");

    // Validate config
    auto data_window_size = feed.config.find("data_window_size");
    eosio::check(data_window_size != feed.config.end(), "config must contain data_window_size");
    eosio::check(data_window_size->second >= DATA_WINDOW_MINIMUM && data_window_size->second <= DATA_WINDOW_MAXIMUM, "data window too small or too large");

    // Create feed
    auto feed_itr = _feeds.find(feed.index);
    if (feed_itr == _feeds.end())
    {
      feed_itr = _feeds.emplace(ram_payer, [&](auto& f) {
        f = feed;
      });

      _data.emplace(ram_payer, [&](auto& f) {
        f.feed_index = feed_itr->index;
        f.points = {};
      });
    }
    // Replace feed
    else
    {
      _feeds.modify(feed_itr, ram_payer, [&](auto& f) {
        f = feed;
      });
    }
  }

  ACTION atom::setfeed (
    const eosio::name account,
    std::optional<uint64_t> index,
    const std::string& name,
    const std::string& description,
    const std::string& aggregate_function,
    const std::string& data_type,
    const std::map<std::string, uint64_t>& config,
    const std::vector<eosio::name>& providers
  ) {
    // Replace feed (ADMIN ONLY)
    if (index.has_value())
    {
      require_auth(get_self());
    }
    // Create feed (ANYONE)
    else
    {
      require_auth(account);
      index = _feeds.available_primary_key();
    }

    // Create feed
    std::map<eosio::name, eosio::time_point> providers_map;
    auto default_time = eosio::time_point();
    for (auto const& provider : providers) {
      providers_map[provider] = default_time;
    }
    Feed feed = {
      .index = *index,
      .name = name,
      .description = description,
      .aggregate_function = aggregate_function,
      .data_type = data_type,
      .config = config,
      .providers = providers_map
    };
    _setfeed(account, feed);
  }

  ACTION atom::removefeed (const uint64_t& index) {
    require_auth(get_self());

    auto feed = _feeds.require_find(index, "feed not found");
    _feeds.erase(feed);

    auto data = _data.require_find(index, "data not found");
    _data.erase(data);
  }
} // namepsace contract