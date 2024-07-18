interface Oracle {
    "account": string,
    "index": number,
    "name": string,
    "description": string,
    "aggregate_function": string,
    "data_type": string,
    "config": {
        "key": string,
        "value": number
    }[],
    "providers": string[]
}

const bots = ["a.oracles", "b.oracles", "c.oracles", "d.oracles", "e.oracles", "f.oracles", "g.oracles", "h.oracles"]

const lendOracleFormat = (oracleIndex: number, symbol: string, providers = bots) => ({
    "account": "oracles",
    "index": oracleIndex,
    "name": `${symbol}/USD`,
    "description": `Tracks 10 min average price of the ${symbol}/USD pair`,
    "aggregate_function": "mean_median",
    "data_type": "double",
    "config": [
            { "key": "data_window_size", "value": 210 },
            { "key": "min_provider_wait_sec", "value": 60 },
            { "key": "data_same_provider_limit", "value": 10 }
    ],
    "providers": providers
})

export const oracles: { [key: string]: Oracle } = {
    "XPR/BTC": {
        "account": "oracles",
        "index": 2,
        "name": "XPR/BTC",
        "description": "Tracks 14 day average price of the XPR/BTC pair",
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 210 },
                { "key": "min_provider_wait_sec", "value": 86400 },
                { "key": "data_same_provider_limit", "value": 14 }
        ],
        "providers": ["bot1", "bot2", "bot3", "bot4"]
    },

    "XPR/USD" : lendOracleFormat(3, "XPR"),
    "BTC/USD" : lendOracleFormat(4, "BTC"),
    "USDC/USD": {
        "account": "oracles",
        "index": 5,
        "name": `USDC/USD`,
        "description": `Tracks 10 min average price of the USDC/USD pair`,
        "aggregate_function": "mean_median",
        "data_type": "double",
        "config": [
                { "key": "data_window_size", "value": 1 },
                { "key": "min_provider_wait_sec", "value": 60 },
                { "key": "data_same_provider_limit", "value": 1 }
        ],
        "providers": ["swapscold"]
    },
    "MTL/USD" : lendOracleFormat(6, "MTL"),
    "ETH/USD" : lendOracleFormat(7, "ETH"),
    "DOGE/USD": lendOracleFormat(8, "DOGE"),
    "USDT/USD": lendOracleFormat(9, "USDT"),
    "UST/USD" : lendOracleFormat(10, "UST"),
    "LUNA/USD": lendOracleFormat(11, "LUNA"),
    "XMD/USD" : lendOracleFormat(12, "XMD", ["swapscold"]),
    "BUSD/USD": lendOracleFormat(13, "BUSD"),
    "PAX/USD" : lendOracleFormat(14, "PAX"),
    "TUSD/USD": lendOracleFormat(15, "TUSD"),
    "LTC/USD": lendOracleFormat(16, "LTC"),
    "PYUSD/USD": lendOracleFormat(17, "PYUSD"),
    "XRP/USD": lendOracleFormat(18, "XRP"),
}

