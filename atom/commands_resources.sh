cleosp push transaction '{
  "actions": [
    {
      "account": "eosio",
      "name": "delegatebw",
      "authorization": [{
          "actor": "wlcm.proton",
          "permission": "newacc"
        }
      ],
      "data": {
        "from": "wlcm.proton",
        "receiver": "h.oracles",
        "stake_net_quantity": "1000.0000 SYS",
        "stake_cpu_quantity": "1000.0000 SYS",
        "transfer": 0
      }
    }
  ]
}'