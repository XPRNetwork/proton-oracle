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
}';

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
        "providers": ["a.oracles", "b.oracles", "c.oracles", "d.oracles", "e.oracles", "f.oracles", "g.oracles", "h.oracles"]
}';

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
        "providers": ["a.oracles", "b.oracles", "c.oracles", "d.oracles", "e.oracles", "f.oracles", "g.oracles", "h.oracles"]
}';


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
}';


LEND_XMT_ORACLE='{
        "account": "oracles",
        "index": "6",
        "name": "MTL/USD",
        "description": "Tracks 10 min average price of the XMT/USD pair",
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 210 },
                { "key": "min_provider_wait_sec", "value": 60 },
                { "key": "data_same_provider_limit", "value": 10 }
        ],
        "providers": ["a.oracles", "b.oracles", "c.oracles", "d.oracles", "e.oracles", "f.oracles", "g.oracles", "h.oracles"]
}';

LEND_XETH_ORACLE='{
        "account": "oracles",
        "index": "7",
        "name": "ETH/USD",
        "description": "Tracks 10 min average price of the XETH/USD pair",
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 210 },
                { "key": "min_provider_wait_sec", "value": 60 },
                { "key": "data_same_provider_limit", "value": 10 }
        ],
        "providers": ["a.oracles", "b.oracles", "c.oracles", "d.oracles", "e.oracles", "f.oracles", "g.oracles", "h.oracles"]
}';

LEND_XDOGE_ORACLE='{
        "account": "oracles",
        "index": "8",
        "name": "DOGE/USD",
        "description": "Tracks 10 min average price of the XDOGE/USD pair",
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 210 },
                { "key": "min_provider_wait_sec", "value": 60 },
                { "key": "data_same_provider_limit", "value": 10 }
        ],
        "providers": ["a.oracles", "b.oracles", "c.oracles", "d.oracles", "e.oracles", "f.oracles", "g.oracles", "h.oracles"]
}';


MSIG_PERM='[
        { "actor": "swapsledger2", "permission": "active" },
        { "actor": "swapsledger3", "permission": "active" },
        { "actor": "swapsledger4", "permission": "active" },
]';

ORACLES_PERM='[{ "actor": "oracles", "permission": "active" }]';
SWAPSCOLD_PERM='[{ "actor": "swapscold", "permission": "active" }]';
SWAPSLEDGER_PERM='{ "actor": "swapsledger4", "permission": "active" }';

TEST_FEED='{
        "account": "swapscold",
        "feed_index": 5,
        "data": {
                "d_double": 1,
                "d_string": null,
                "d_uint64_t": null
        }
}';

# ACTIONS
cleosp push action oracles setfeed $LEND_XUSDC_ORACLE -p oracles;

# XUSDC (special 1)
cleosp multisig propose prop1 $MSIG_PERM $ORACLES_PERM oracles setfeed $LEND_XUSDC_ORACLE swapsledger4;
cleosp multisig propose prop2 $MSIG_PERM $SWAPSCOLD_PERM oracles feed $TEST_FEED swapsledger4;

# Others
cleosp multisig propose prop3 $MSIG_PERM $ORACLES_PERM oracles setfeed $LEND_XPR_ORACLE swapsledger4;
cleosp multisig propose prop4 $MSIG_PERM $ORACLES_PERM oracles setfeed $LEND_XMT_ORACLE swapsledger4;
cleosp multisig propose prop5 $MSIG_PERM $ORACLES_PERM oracles setfeed $LEND_XETH_ORACLE swapsledger4;
cleosp multisig propose prop11 $MSIG_PERM $ORACLES_PERM oracles setfeed $LEND_XBTC_ORACLE swapsledger4;
cleosp multisig propose prop12 $MSIG_PERM $ORACLES_PERM oracles setfeed $LEND_XDOGE_ORACLE swapsledger4;

# Approve
cleosp multisig approve swapsledger4 prop1 $SWAPSLEDGER_PERM;
cleosp multisig approve swapsledger4 prop2 $SWAPSLEDGER_PERM;
cleosp multisig approve swapsledger4 prop3 $SWAPSLEDGER_PERM;
cleosp multisig approve swapsledger4 prop4 $SWAPSLEDGER_PERM;
cleosp multisig approve swapsledger4 prop5 $SWAPSLEDGER_PERM;
cleosp multisig approve swapsledger4 prop11 $SWAPSLEDGER_PERM;
cleosp multisig approve swapsledger4 prop12 $SWAPSLEDGER_PERM;

# Execute
cleosp multisig exec swapsledger4 prop1 swapsledger4;
cleosp multisig exec swapsledger4 prop2 swapsledger4;
cleosp multisig exec swapsledger4 prop3 swapsledger4;
cleosp multisig exec swapsledger4 prop4 swapsledger4;
cleosp multisig exec swapsledger4 prop5 swapsledger4;
cleosp multisig exec swapsledger4 prop11 swapsledger4;
cleosp multisig exec swapsledger4 prop12 swapsledger4;

# Standard
cleospt push action oracles setfeed $LEND_XPR_ORACLE -p oracles;
cleospt push action oracles setfeed $LEND_XMT_ORACLE -p oracles;
cleospt push action oracles feed $TEST_FEED -p swapsledger4;