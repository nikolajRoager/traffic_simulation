City data format
======
The city file stores the road network, it is a json file with the following elements:

    {
        "nodes":
        [
            {
                "type":"Hellhole",
                "pos":[-1000, -1000]
            },
            ...
        ],
        "auto_roads":
        [
            {
                "type":"Motortrafikvej",
                "first":0,
                "second":1,
                "lanes":3,
                "one_way":false
                "no_overtake":false
            }
        ]
    }


`"nodes"`
-----------
The node type is stored as a string, valid types are `"hellhole"` (For debugging purpose only, the car jumps of the end of the road and goes to hell, this can only connect to one road.

`"auto_roads"`
---------------
The road type is stored as a string, the types follow danish designations, `"gaagade"` are pedestrianized streets, speed limit is here 30 km/h. `"byvej"` are urban streets, speed limit 50 km/h, `"landevej"` are rural roads, speed limit 80 km/h, `"motortrafikvej"` are often multi-lane inter-city roads speed limit 90 km/h where biking and walking at the side is illegal, `"motorvej"` are the European highway network, speed limit 130 km/h.

The roads all use the same elements: `"first"` and `"second"` are required integer indices of the nodes at the start or end.

`"lanes"` (default 1) is the number of lanes each direction, the `"one_way"` (default `false`) flag say that the road only goes from first to second, not the other way, the `"no_overtake"` flag (default false) prevents cars from overtaking through the opposing lane (overtaking using lanes already on the road is always legal).

