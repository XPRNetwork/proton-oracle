#pragma once

#define SYSTEM_CONTRACT eosio::name("eosio")

namespace proton
{
  static constexpr auto SECONDS_IN_MINUTE = 60;
  static constexpr auto SECONDS_IN_HOUR   = SECONDS_IN_MINUTE * 60;
  static constexpr auto SECONDS_IN_DAY    = SECONDS_IN_HOUR * 24;

  static constexpr auto DATA_WINDOW_MINIMUM = 1;
  static constexpr auto DATA_WINDOW_MAXIMUM = 250;

  namespace Aggregates
  {
    const std::string MODE        = "mode";
    const std::string MEDIAN      = "median";
    const std::string MEAN        = "mean";
    const std::string MEAN_MEDIAN = "mean_median";
    const std::string LAST        = "last";
    const std::string SD          = "sd";
  }

  static std::map<std::string, std::vector<std::string>> type_index_to_aggregates = {
    { "string",   { Aggregates::MODE, Aggregates::LAST } },
    { "uint64_t", { Aggregates::MODE, Aggregates::LAST, Aggregates::MEDIAN } },
    { "double",   { Aggregates::MODE, Aggregates::LAST, Aggregates::MEDIAN, Aggregates::MEAN, Aggregates::SD, Aggregates::MEAN_MEDIAN } },
  };
} // namespace proton