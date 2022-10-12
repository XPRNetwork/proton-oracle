#include <atom/atom.hpp>

namespace proton
{
  void atom::createmsig (
    const eosio::name& proposer,
    const Feed& feed
  ) {
    require_auth(proposer);
    
    _msigs.emplace(proposer, [&](auto& m){
      m.index = _msigs.available_primary_key();
      m.proposer = proposer;
      m.new_feed = feed;
      m.votes = {};
    });
  }

  void atom::votemsig (
    const eosio::name& provider,
    const uint64_t& msig_index,
    const bool& vote
  ) {
    require_auth(provider);

    auto msig = _msigs.require_find(msig_index, "msig not found");
    auto feed = _feeds.require_find(msig->new_feed.index, "feed not found");
    eosio::check(feed->providers.find(provider) != feed->providers.end(), "provider not found");

    _msigs.modify(msig, provider, [&](auto& m){
      m.votes[provider] = vote;
    });
  }

  void atom::executemsig (
    const eosio::name& executor,
    const uint64_t& msig_index
  ) {
    // Auth
    require_auth(executor);

    // Ensure existence
    auto msig = _msigs.require_find(msig_index, "msig not found");
    auto feed = _feeds.require_find(msig->new_feed.index, "feed not found");

    // Check # of approvals
    uint8_t min_approvals = ((feed->providers.size() * 2) / 3) + 1;
    uint8_t provided_approvals = std::count_if(msig->votes.begin(), msig->votes.end(), [&](auto& i) {
      return i.second;
    });
    eosio::check(provided_approvals >= min_approvals, "not enough approvals");

    // Set feed
    _setfeed(executor, msig->new_feed);

    // Delete msig
    _msigs.erase(msig);
  }

  void atom::cancelmsig (
    const eosio::name& proposer,
    const uint64_t& msig_index
  ) {
    require_auth(proposer);

    auto msig = _msigs.require_find(msig_index, "msig not found");
    eosio::check(proposer == msig->proposer, "only the msig proposer can cancel the msig");
    
    _msigs.erase(msig);
  }
} // namepsace contract