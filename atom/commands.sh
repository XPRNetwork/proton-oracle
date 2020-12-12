cleosp set contract oracles atom 

cleosp push action oracles cleanup '[]' -p oracles 

cleosp push action oracles addfeed '{
        "name": "XPR/BTC",
        "description": "Tracks real-time price of the XPR/BTC pair",
        "aggregate_function": "mean",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 20 },
                { "key": "min_provider_wait_sec", "value": 0 },
                { "key": "data_freshness_sec", "value": 0 },
                { "key": "data_same_provider_limit", "value": 0 }
        ],
        "providers": ["oracles", "bot1", "bot2", "bot3", "bot4"]
}' -p oracles;

cleosp push action oracles addfeed '{
        "name": "BTC/USDT",
        "description": "Tracks real-time price of the BTC/USDT pair",
        "aggregate_function": "mean",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 20 },
                { "key": "min_provider_wait_sec", "value": 0 },
                { "key": "data_freshness_sec", "value": 0 },
                { "key": "data_same_provider_limit", "value": 5 }
        ],
        "providers": ["oracles", "bot1", "bot2", "bot3", "bot4"]
}' -p oracles;

cleosp push action oracles updatefeed '{
        "feed_index": 0,
        "name": "XPR/BTC",
        "description": "Tracks real-time price of the XPR/BTC pair",
        "aggregate_function": "mean",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 20 },
                { "key": "min_provider_wait_sec", "value": 0 },
                { "key": "data_freshness_sec", "value": 0 },
                { "key": "data_same_provider_limit", "value": 0 }
        ],
        "providers": ["oracles", "bottest", "bot1"]
}' -p oracles;

cleosp push action oracles feed '{
        "account": "oracles",
        "feed_index": 0,
        "data": ["float64", 0.00000048]
}' -p oracles;