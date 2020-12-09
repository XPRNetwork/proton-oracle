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
        _feeds(receiver, receiver.value),
        _data(receiver, receiver.value) {}

    ACTION addfeed (
      const std::string& name,
      const std::string& description,
      const std::string& aggregate_function,
      const std::string& data_type,
      const std::map<std::string, uint64_t>& config,
      const std::vector<eosio::name>& providers
    );
    ACTION updatefeed (
      const uint64_t& feed_index,
      const std::string& name,
      const std::string& description,
      const std::string& aggregate_function,
      const std::string& data_type,
      const std::map<std::string, uint64_t>& config,
      const std::vector<eosio::name>& providers
    );
    ACTION removefeed ( const uint64_t& feed_index );
    
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

      data_table db2(get_self(), get_self().value);
      auto itr2 = db2.end();
      while(db2.begin() != itr2){
        itr2 = db2.erase(--itr2);
      }
    }

    // Action wrappers
    // using feed_action = eosio::action_wrapper<"withdraw"_n,     &atom::withdraw>;

    // Initialize tables from tables.hpp
    feeds_table _feeds;
    data_table _data;

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