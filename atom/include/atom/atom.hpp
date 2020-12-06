#pragma once

// Standard.
#include <vector>
#include <algorithm>
#include <cmath>

// EOS
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>

// Local
#include "constants.hpp"
#include "tables.hpp"

namespace proton {
  CONTRACT atom : public eosio::contract {
  public:
    using contract::contract;

    atom( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds )
      : contract(receiver, code, ds),
        _feeds(receiver, receiver.value) {}

    ACTION addfeed (
      std::string name,
      std::string description,
      std::string aggregate_function,
      uint8_t data_type_index,
      uint8_t data_window_size,
      uint64_t min_provider_wait_sec,
      std::vector<eosio::name> providers
    );
    ACTION updatefeed ( const Feed& feed                     );
    ACTION removefeed ( const uint64_t& feed_index              );
    ACTION feed (
      const eosio::name& account,
      const uint64_t& feed_index,
      const data_variant& data
    );

    ACTION cleanup () {
      require_auth(get_self());
      
      feeds_table db(get_self(), get_self().value);
      auto itr = db.end();
      while(db.begin() != itr){
        itr = db.erase(--itr);
      }
    }

    // Action wrappers
    // using feed_action = eosio::action_wrapper<"withdraw"_n,     &atom::withdraw>;

    // Initialize tables from tables.hpp
    feeds_table _feeds;

  private:
    void set_data(
      const eosio::name& creator,
      const uint64_t& feed_index,
      const data_variant& data
    );

    data_variant aggregate (
      const std::string& aggregate_function,
      const std::vector<ProviderPoint>& points
    );
    data_variant calculate_mode(const std::vector<ProviderPoint>& data);
    data_variant calculate_mean(const std::vector<ProviderPoint>& data);
    data_variant calculate_median(const std::vector<ProviderPoint>& data);
    data_variant calculate_last(const std::vector<ProviderPoint>& data);
    data_variant calculate_sd(const std::vector<ProviderPoint>& data);
  };
}