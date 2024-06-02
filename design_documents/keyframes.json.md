Keyframe data format
======
All physical units are in SI units

These files store sthe location of all vehicles, at all critical time-points (between which the vehicles travel at constant acceleration)

Each Vehicle element contains some basic, constant information: specifically `"length"` (an integer, default 3 in meters) and `"type"` which is a string, either car, bus, or truck, (this is for display purposes only)



    {
        "vehicles":
        [
            {
                "type":"car",
                "length":3,
                "keyframes":
                [
                    {
                        "time":0,
                        "road":1,
                        "direction":true,
                        "lane":0,
                        "pos":0,
                        "speed":0,
                        "acc":0
                    },
                    ...

                ]
            },
            ...
        ]
    }

The only required element in a key-frame is `"time"` which must be greater than last key-frame.


`"road"` is the current road ID in the network, `"direction"` is true when travelling from the first node to the second, and the `"lane"` is whatever lane we are on now. `"pos"` is measured from the start of that road. `"speed"` and `"acc"` are speed and acceleration.

Anything other than `"time"` is optional data, if it is not supplied it is calculated from previous data, i.e. if we at `t=0` has all set to 0 and `acc=1`, and at `t=1` has a new key-frame only with `acc=0`, then at that point we will have `speed=1` and `pos=0.5`.

The very final keyframe is always treated as a "despawn" command, so only the time will matter
