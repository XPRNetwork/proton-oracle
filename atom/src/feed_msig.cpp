#include <atom/atom.hpp>

namespace proton
{
  ACTION atom::createmsig (
    const eosio::name& proposer,
    const Feed& feed
  ) {
    require_auth(proposer);
    
    _msigs.emplace(proposer, [&](auto& m){
      m.index = _msigs.available_primary_key();
      m.proposer = proposer;
      m.new_feed = feed;
      m.approved_providers = {};
    });
  }

  ACTION atom::approvemsig (
    const eosio::name& provider,
    const uint64_t& msig_index,
    const bool& approve
  ) {
    require_auth(provider);

    auto msig = _msigs.require_find(msig_index, "msig not found");
    auto feed = _feeds.require_find(msig->new_feed.index, "feed not found");
    eosio::check(feed->providers.find(provider) != feed->providers.end(), "provider not found");

    _msigs.modify(msig, provider, [&](auto& m){
      m.approved_providers[provider] = approve;
    });
  }

  ACTION atom::executemsig (
    const uint64_t& msig_index
  ) {
    // Ensure existence
    auto msig = _msigs.require_find(msig_index, "msig not found");
    auto feed = _feeds.require_find(msig->new_feed.index, "feed not found");

    // Check # of approvals
    uint8_t min_approvals = ((feed->providers.size() * 2) / 3) + 1;
    uint8_t provided_approvals = std::count_if(msig->approved_providers.begin(), msig->approved_providers.end(), [&](auto& i) {
      return i.second;
    });
    eosio::check(min_approvals >= provided_approvals, "not enough approvals");

    // Create providers list
    std::vector<eosio::name> providers;
    for (const auto& [k, v]: msig->new_feed.providers) {
      providers.emplace_back(k);
    }

    // Set feed
    setfeed_action s_action(get_self(), {get_self(), "active"_n});
    s_action.send(
      msig->new_feed.index,
      msig->new_feed.name,
      msig->new_feed.description,
      msig->new_feed.aggregate_function,
      msig->new_feed.data_type,
      msig->new_feed.config,
      providers
    );
  }

  ACTION atom::cancelmsig (
    const eosio::name& proposer,
    const uint64_t& msig_index
  ) {
    require_auth(proposer);

    auto msig = _msigs.require_find(msig_index, "msig not found");
    eosio::check(proposer == msig->proposer, "only the msig proposer can cancel the msig");
    
    _msigs.erase(msig);
  }
} // namepsace contract