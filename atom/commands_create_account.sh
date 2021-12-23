cleosp push transaction '{
  "actions": [
    {
      "account": "eosio",
      "name": "newaccount",
      "authorization": [{
          "actor": "proton",
          "permission": "active"
      }],
      "data": {
        "creator": "proton",
        "name": "h.oracles",
        "owner": {
          "threshold": 1,
          "keys": [],
          "accounts": [{
              "permission": {
                "actor": "swapscold",
                "permission": "owner"
              },
              "weight": 1
          }],
          "waits": []
        },
        "active": {
          "threshold": 1,
          "keys": [{
            "weight": 1,
            "key": "EOS8UP3m3Kg5dN5MTkJUiWRzPvJxhibjYeoPNPrcopJnnASgrnydy"
          }],
          "accounts": [],
          "waits": []
        }
      }
    },
    {
      "account": "eosio",
      "name": "buyrambsys",
      "authorization": [{
          "actor": "wlcm.proton",
          "permission": "newacc"
      }],
      "data": {
        "payer": "wlcm.proton",
        "receiver": "h.oracles",
        "bytes": 100000
      }
    },
    {
      "account": "eosio.proton",
      "name": "newaccres",
      "authorization": [{
          "actor": "otctest",
          "permission": "active"
      }],
      "data": {
        "account": "h.oracles"
      }
    }
  ]
}';