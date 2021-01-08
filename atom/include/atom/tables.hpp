#pragma once

namespace proton {
  struct data_variant {
    std::optional<std::string> d_string;
    std::optional<uint64_t> d_uint64_t;
    std::optional<double> d_double;

    data_variant () {};
    data_variant (std::string val) { d_string = val; };
    data_variant (uint64_t val) { d_uint64_t = val; };
    data_variant (double val) { d_double = val; };
    
    bool operator < ( const data_variant& rhs ) const {
      std::string data_type = this->data_type();
      eosio::check(this->data_type() == rhs.data_type(), "not equivalent data types");
      if (data_type == "string") {
        return this->get<std::string>() < rhs.get<std::string>();
      } else if (data_type == "uint64_t") {
        return this->get<uint64_t>() < rhs.get<uint64_t>();
      } else if (data_type == "double") {
        return this->get<double>() < rhs.get<double>();
      } else {
        eosio::check(false, "invalid data_variant <");
        return false;
      }
    }

    std::string data_type ()const {
      if (d_string.has_value()) {
        return "string";
      } else if (d_uint64_t.has_value()) {
        return "uint64_t";
      } else if (d_double.has_value()) {
        return "double";
      } else {
        eosio::check(false, "invalid data_variant type");
        return {};
      }
    };

    template<typename T>
    T get ()const {
      std::string data_type = this->data_type();
      if constexpr (std::is_same<T, std::string>::value) {
        eosio::check(data_type == "string", "invalid data_variant get");
        return *d_string;
      } else if constexpr (std::is_same<T, uint64_t>::value) {
        eosio::check(data_type == "uint64_t", "invalid data_variant get");
        return *d_uint64_t;
      } else if constexpr (std::is_same<T, double>::value) {
        eosio::check(data_type == "double", "invalid data_variant get");
        return *d_double;
      } else {
        eosio::check(false, "invalid data_variant get");
        return {};
      }
    };

    EOSLIB_SERIALIZE( data_variant, (d_string)(d_uint64_t)(d_double) )
  };

  struct ProviderPoint {
    eosio::name provider;
    eosio::time_point time;
    data_variant data;

    EOSLIB_SERIALIZE( ProviderPoint, (provider)(time)(data) )
  };

  struct [[eosio::table, eosio::contract("atom")]] Feed {
    uint64_t index;
    std::string name;
    std::string description;
    std::string aggregate_function;
    std::string data_type;
    std::map<std::string, uint64_t> config;
    std::map<eosio::name, eosio::time_point> providers;

    uint64_t primary_key() const { return index; };

    EOSLIB_SERIALIZE( Feed, (index)(name)
                            (description)(aggregate_function)
                            (data_type)(config)
                            (providers) )
  };
  typedef eosio::multi_index<"feeds"_n, Feed> feeds_table;

  struct [[eosio::table, eosio::contract("atom")]] Data {
    uint64_t feed_index;
    data_variant aggregate;
    std::vector<ProviderPoint> points;

    uint64_t primary_key() const { return feed_index; };

    EOSLIB_SERIALIZE( Data, (feed_index)(aggregate)(points) )
  };
  typedef eosio::multi_index<"data"_n, Data> data_table;

  struct [[eosio::table, eosio::contract("atom")]] Msig {
    uint64_t index;
    eosio::name proposer;
    Feed new_feed;
    std::map<eosio::name, bool> votes;

    uint64_t primary_key() const { return index; };

    EOSLIB_SERIALIZE( Msig, (index)(proposer)(new_feed)(votes) )
  };
  typedef eosio::multi_index<"msigs"_n, Msig> msigs_table;
}