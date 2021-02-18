ACTION='{
        "account": "oracles",
        "index": "2",
        "name": "XPR/BTC",
        "description": "Tracks 14 day average price of the XPR/BTC pair",
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 210 },
                { "key": "min_provider_wait_sec", "value": 86400 },
                { "key": "data_same_provider_limit", "value": 14 }
        ],
        "providers": ["bot1", "bot2", "bot3", "bot4", "cryptolions", "eosusa", "protonnz", "eosrio"]
}'

cleospt push action oracles setfeed $ACTION -p oracles;

cleospt multisig propose prop1 '[
        { "actor": "syed", "permission": "partner" },
        { "actor": "marshallbits", "permission": "partner" },
        { "actor": "jacob", "permission": "partner" },
        { "actor": "glenn", "permission": "partner" },
        { "actor": "fred", "permission": "partner" }
]' '[
        { "actor": "oracles", "permission": "active" },
]' oracles setfeed $ACTION syed;

cleosp push action oracles feed '{
        "account": "oracles",
        "feed_index": 0,
        "data": ["float64", 0.00000048]
}' -p oracles;