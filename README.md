# Proton Oracles

## Directory
- atom: Oracle Smart Contracts
- js_tests: JS Tests

## Aggregate Function -> {supported types}
"sd"     -> {double}  

"mean"   -> {double}  

"median" -> {double, uint64_t}  

"mode"   -> {double, uint64_t, string}  

"last"   -> {double, uint64_t, string}

Notes: Median will return average of middle two values if an even number of values are present. Mode will return the last mode if there are 2 or more modes in the dataset.

## Feed Config  
{uint64_t} data_window_size (min 1, max 250)  

{uint64_t} data_same_provider_limit (optional)  

{uint64_t} data_freshness_sec (optional)  

{uint64_t} min_provider_wait_sec (optional)

## Data types
"string"   -> std::string  

"uint64_t" -> uint64_t  

"double"   -> double  

## ACTION `setfeed`

Creates or replaces a feed

- **authority**: `get_self()`

### params

- `{optional<uint64_t>} index` - (Optional) Index of the feed if updating
- `{string} name` - name of feed
- `{string} description` - description of feed
- `{string} aggregate_function` - type of aggregate function
- `{string} data_type` - flash loan fee
- `{map<string, uint64_t>} config` - Feed config
- `{vector<name>} providers` - All feed providers


## ACTION `removefeed`

Removes a feed

- **authority**: `get_self()`

### params

- `{uint64_t} index` - Index of the feed to remove


## ACTION `feed`

Feed data as a provider

- **authority**: `account`

### params

- `{name} account` - Provider account
- `{uint64_t} feed_index` - Index of the feed to provide data to
- `{data_variant} data` - Data to provide


## ACTION `createmsig`

Create multisig

- **authority**: `proposer`

### params

- `{name} propose` - Proposer account
- `{Feed} feed` - New feed to propose


## ACTION `votemsig`

Approve / reject multisig

- **authority**: `provider`

### params

- `{name} provider` - Provider account
- `{uint64_t} msig_index` - Index of the msig to approve
- `{bool} vote` - `true` to approve / `false` to reject


## ACTION `executemsig`

execute multisig

- **authority**: no authority needed

### params

- `{uint64_t} msig_index` - Index of the msig to execute



## ACTION `cancelmsig`

Cancel multisig

- **authority**: `provider`

### params

- `{name} proposer` - Proposer account
- `{uint64_t} msig_index` - Index of the msig to cancel


## TABLE `feeds`

- `{uint64_t} index` - index of the feed
- `{string} name` - name of feed
- `{string} description` - description of feed
- `{string} aggregate_function` - type of aggregate function
- `{string} data_type` - flash loan fee
- `{map<string, uint64_t>} config` - Feed config
- `{map<name, time_point>} providers` - Map of provider name to last time they provided data

### example feeds row

```json
{
  "index": 0,
  "name": "XPR/BTC",
  "description": "Oracle for real-time provision of XPR price against BTC",
  "aggregate_function": "mean",
  "data_type": "double",
  "config": {
    "data_window_size": 20,
    "data_same_provider_limit": 5
  },
  "providers": {
    "bot1": "1970-01-01T00:00:00.000",
    "bot2": "1970-01-01T00:00:00.000",
    "bot3": "1970-01-01T00:00:00.000"
  }
}
```


## TABLE `data`

- `{uint64_t} feed_index` - index of the feed
- `{data_variant} aggregate` - aggegrate data point
- `{vector<ProviderPoint>} points` - vector of provider points

### example data row

```json
{
  "index": 0,
  "aggregate": {
    "d_string": null,
    "d_uint64_t": null,
    "d_double": "18293.82000000000334694"
  }	,
  "points": [
    {
      "provider": "bot4",
      "time": "2020-12-12T02:04:37.500",
      "data": {
        "d_string": null,
        "d_uint64_t": null,
        "d_double": "18293.81999999999970896"
      }
    },
  ]
}
```


## TABLE `msigs`

- `{uint64_t} index` - index of the msig
- `{name} proposer` - aggegrate data point
- `{Feed} new_feed` - vector of provider points
- `{map<name, bool>} votes` - Map of providers who have voted to their true/false vote

### example msig row

```json
{
  "index": 0,
  "proposer": "bot1",
  "new_feed": {
    "index": 0,
    "name": "XPR/BTC",
    "description": "NEW DESCRIPTION",
    "aggregate_function": "mean",
    "data_type": "double",
    "config": {
      "data_window_size": 20,
      "data_same_provider_limit": 5
    },
    "providers": ["bot1", "bot2", "bot3"]
  },
  "votes": {
    "bot1": true,
    "bot2": false
  }
}
```