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
    
    bool operator < ( const data_variant& rhs ) const { return true; }
    bool operator > ( const data_variant& rhs ) const { return true; }

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
      if constexpr (std::is_same<T, std::string>::value && data_type == "string") {
        return *d_string;
      } else if (std::is_same<T, uint64_t>::value && data_type == "uint64_t") {
        return *d_uint64_t;
      } else if (std::is_same<T, double>::value && data_type == "double") {
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

  /**
   * CONFIG:
    uint64_t data_window_size = 20;
    uint64_t data_same_provider_limit = 0;
    uint64_t data_freshness_sec = 0;
    uint64_t min_provider_wait_sec = 0;
  */
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
}