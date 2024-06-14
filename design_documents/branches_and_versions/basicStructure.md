Basic structure
=========
Implement the basic structure of roads and nodes in a city, which can be loaded from JSon

Requirements
-------------
Test suite: In order to even add a vehicle, we must test that loading nodes and roads even work, we need a test suite which verifies that:
Verify that a "Hellhole" Node, addRoad and getRoad on empty Node throws error when setting nullptr road
Given two "Hellhole" nodes in some list: Try joining them with a road defined by a JSon object. Test that JSON errors work:
A JSon object with an invalid node ID throws an error,
A JSon object with invalid road type throws an error,
A JSon object with missing required elements throws an error (missing "first", "second" or "type"),
Given a correct JSon object, verify that the RoadID is correct from the Road
Verify that the two "Hellhole"s have the correct number of Roads
Verify that the two "Hellhole"s have the correct RoadID (test getRoadID)
Verify that the two "Hellhole"s have the correct neighbours

