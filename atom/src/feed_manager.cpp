#include <atom/atom.hpp>

namespace proton
{
  ACTION atom::setfeed (
    std::optional<uint64_t> index,
    const std::string& name,
    const std::string& description,
    const std::string& aggregate_function,
    const std::string& data_type,
    const std::map<std::string, uint64_t>& config,
    const std::vector<eosio::name>& providers
  ) {
    // Feed already exists (replacing)
    if (index.has_value())
    {
      require_auth(get_self());
    }
    // Feed does not exist (creating)
    else
    {
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

    // Find feed
    auto feed_itr = _feeds.require_find(feed.index, "feed does not exist");

    // Validation
    auto type_index_itr = type_index_to_aggregates.find(feed_itr->data_type);
    eosio::check(type_index_itr != type_index_to_aggregates.end(), "invalid type index");
    auto aggregate_itr = std::find(type_index_itr->second.begin(), type_index_itr->second.end(), feed_itr->aggregate_function);
    eosio::check(aggregate_itr != type_index_itr->second.end(), "invalid aggregate function");

    // Validate config
    auto config_data_window_size = feed.config.find("config_data_window_size");
    eosio::check(config_data_window_size != feed.config.end(), "config must contain data_window_size ");
    eosio::check(config_data_window_size->second >= DATA_WINDOW_MINIMUM && config_data_window_size->second <= DATA_WINDOW_MAXIMUM, "data window too small or too large");

    // Create feed
    if (feed_itr == _feeds.end())
    {
      feed_itr = _feeds.emplace(get_self(), [&](auto& f) { f = feed; });

      _data.emplace(get_self(), [&](auto& f) {
        f.feed_index = feed_itr->index;
        f.points = {};
      });
    }
    // Replace feed
    else
    {
      _feeds.modify(feed_itr, get_self(), [&](auto& f) { f = feed; });
    }
  }

  ACTION atom::removefeed (const uint64_t& index) {
    require_auth(get_self());

    auto feed = _feeds.require_find(index, "feed not found");
    _feeds.erase(feed);

    auto data = _data.require_find(index, "data not found");
    _data.erase(data);
  }
} // namepsace contract