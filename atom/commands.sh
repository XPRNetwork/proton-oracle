cleospt set contract test14 atom 

cleospt push action test14 cleanup '[]' -p test14 

cleospt push action test14 addfeed '{
        "name": "Test",
        "description": "Feed 1",
        "aggregate_function": "sd",
        "data_type_index": 2,
        "data_window_size": 20,
        "min_provider_wait_sec": 5,
        "providers": ["test14"]
}' -p test14;

cleospt push action test14 feed '{
        "account": "test14",
        "feed_index": 0,
        "data": ["float64", 1]
}' -p test14;
cleospt push action test14 feed '{
        "account": "test14",
        "feed_index": 0,
        "data": ["float64", 2]
}' -p test14;
cleospt push action test14 feed '{
        "account": "test14",
        "feed_index": 0,
        "data": ["float64", 7]
}' -p test14;
cleospt push action test14 feed '{
        "account": "test14",
        "feed_index": 0,
        "data": ["float64", 8]
}' -p test14;