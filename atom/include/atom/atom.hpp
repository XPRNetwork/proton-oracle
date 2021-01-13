#pragma once

// Standard.
#include <vector>
#include <algorithm>
#include <cmath>
#include <optional>

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
        _data(receiver, receiver.value),
        _msigs(receiver, receiver.value) {}

    ACTION setfeed (
      const eosio::name account,
      std::optional<uint64_t> index,
      const std::string& name,
      const std::string& description,
      const std::string& aggregate_function,
      const std::string& data_type,
      const std::map<std::string, uint64_t>& config,
      const std::vector<eosio::name>& providers
    );
    ACTION removefeed ( const uint64_t& index );

    ACTION feed (
      const eosio::name& account,
      const uint64_t& feed_index,
      const data_variant& data
    );

    ACTION createmsig (
      const eosio::name& proposer,
      const Feed& feed
    );
    ACTION votemsig (
      const eosio::name& provider,
      const uint64_t& msig_index,
      const bool& approve
    );
    ACTION executemsig (
      const eosio::name& executor,
      const uint64_t& msig_index
    );
    ACTION cancelmsig (
      const eosio::name& proposer,
      const uint64_t& msig_index
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
    // using setfeed_action = eosio::action_wrapper<"setfeed"_n, &atom::setfeed>;
    using feed_action = eosio::action_wrapper<"feed"_n, &atom::feed>;

    // Initialize tables from tables.hpp
    feeds_table _feeds;
    data_table _data;
    msigs_table _msigs;

  private:
    void _setfeed (
      const eosio::name& ram_payer,
      const Feed& feed
    );
    data_variant aggregate (
      const std::string& aggregate_function,
      const std::vector<ProviderPoint>& points
    );
    data_variant calculate_mode(const std::vector<ProviderPoint>& data);
    data_variant calculate_mean_median(const std::vector<ProviderPoint>& data);
    data_variant calculate_mean(const std::vector<ProviderPoint>& data);
    data_variant calculate_median(const std::vector<ProviderPoint>& data);
    data_variant calculate_last(const std::vector<ProviderPoint>& data);
    data_variant calculate_sd(const std::vector<ProviderPoint>& data);
  };
}