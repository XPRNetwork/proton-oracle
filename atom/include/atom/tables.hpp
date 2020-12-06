#pragma once

namespace proton {
  struct ProviderPoint {
    eosio::name provider;
    data_variant data;
    eosio::time_point time;

    EOSLIB_SERIALIZE( ProviderPoint, (provider)(data)(time) )
  };

  struct [[eosio::table, eosio::contract("atom")]] Feed {
    uint64_t index;
    std::string name;
    std::string description;
    std::string aggregate_function;
    uint8_t data_type_index = 0;
    uint8_t data_window_size = 1;
    uint64_t min_provider_wait_sec = 0;
    std::map<eosio::name, eosio::time_point> providers;
    data_variant aggregate;
    std::vector<ProviderPoint> points;

    uint64_t primary_key() const { return index; };

    EOSLIB_SERIALIZE( Feed, (index)(name)
                            (description)(aggregate_function)
                            (data_type_index)(data_window_size)
                            (min_provider_wait_sec)(providers)
                            (aggregate)(points) )
  };
  typedef eosio::multi_index<"feeds"_n, Feed> feeds_table;
}