Drive a single Vehicles
=========
I misspelled the name when I made the branch, and now I am stuck with it.





Implement the following story
-----------
A single 4 lane road (2 lanes per direction) connects two nodes, the program spawns a car on any lane, going in any direction, starting with any speed. The car tries to go full-speed, and tries to use the lowest lane, it drives to the end of the road and leaves (for the time beeing, the car literally drives off the end at full speed, and falls into a portal to hell).

The simples possible story I can tell, which nevertheless requires that I create bare bones classes handling the roads, nodes, and road-vehicles.


Requirements
-------------
The car must be a derived class from the RoadVehicle class

At the end, the car must receive a signal that it is done

The program should save the journey of this car in a Keyframe

Tests:
Verify that the end-nodes and the road should be associated with each other.

Add a car to a road, advance time and verify that the car
verify that exceptions are thrown when "overstepping" updates for car
Adding a car to the road, should have both the car and the road associated with each other.
The number of cars exiting the road, should match the number of cars entering the road
At the end, the car should know that it is not on the road anymore.
The keyframe JSON file must have the expected keyframes

