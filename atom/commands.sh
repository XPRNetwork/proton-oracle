LONGSTAKE_ORACLE='{
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

LEND_XPR_ORACLE='{
        "account": "oracles",
        "index": "3",
        "name": "XPR/USD",
        "description": "Tracks 10 min average price of the XPR/USD pair",
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 210 },
                { "key": "min_provider_wait_sec", "value": 60 },
                { "key": "data_same_provider_limit", "value": 10 }
        ],
        "providers": ["a.oracles", "b.oracles", "c.oracles", "d.oracles", "e.oracles"]
}'

LEND_XBTC_ORACLE='{
        "account": "oracles",
        "index": "4",
        "name": "BTC/USD",
        "description": "Tracks 10 min average price of the XBTC/USD pair",
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 210 },
                { "key": "min_provider_wait_sec", "value": 60 },
                { "key": "data_same_provider_limit", "value": 10 }
        ],
        "providers": ["a.oracles", "b.oracles", "c.oracles", "d.oracles", "e.oracles"]
}'


LEND_XUSDC_ORACLE='{
        "account": "oracles",
        "index": "5",
        "name": "USDC/USD",
        "description": "Tracks 10 min average price of the XUSDC/USD pair",
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 210 },
                { "key": "min_provider_wait_sec", "value": 60 },
                { "key": "data_same_provider_limit", "value": 10 }
        ],
        "providers": ["swapscold"]
}'

MSIG_PERM='[
        { "actor": "swapsledger2", "permission": "active" },
        { "actor": "swapsledger3", "permission": "active" },
        { "actor": "swapsledger4", "permission": "active" },
]'

ORACLES_PERM='[{ "actor": "oracles", "permission": "active" }]'

TEST_FEED='{
        "account": "swapscold",
        "feed_index": 5,
        "data": {
                "d_double": 1,
                "d_string": null,
                "d_uint64_t": null
        }
}'

# ACTIONS
cleospt push action oracles setfeed $LEND_XUSDC_ORACLE -p oracles;
cleospt multisig propose prop1 $MSIG_PERM $ORACLES_PERM oracles setfeed $ACTION swapsledger4;
cleospt push action oracles feed $TEST_FEED -p swapscold;