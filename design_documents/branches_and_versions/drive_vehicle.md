Drive Vehicle
=========

Implement the following story
-----------
A single 4 lane road (2 lanes per direction) connects two nodes, the program spawns a car on any lane, going in any direction, starting with any speed. The car tries to go full-speed, and tries to use the lowest lane, it drives to the end of the road and leaves (for the time beeing, the car literally drives off the end at full speed, and falls into a portal to hell).

The simples possible story I can tell, which nevertheless requires that I create bare bones classes handling the roads, nodes, and road-vehicles.


Requirements
-------------
The car must be a derived class from the RoadVehicle class (or simply be it)

Roads accept cars, cars can be driven, by telling them to update their current time (based on current position, acceleration and speed, set at key frames). These updates can only be made into the future.

Cars should know their "next update": a future keyframes, they should be able to tell the road and city network which choice is next.
The city network should execute until the next "update", then re-check if that altered any other vehicles updates.

Cars can be added to roads, in this version by "forcing" a spawn (throwing an error if spawning in that lane spawns inside a vehicle)

The roads has a function for getting the first and last vehicle, or the vehicle in front or behind a particular location.



PROBLEMS IN CURRENT IMPLEMENTATION
The Roads, Cars and CityNetwork should share a reference to current simulation time, replace all the many "time" arguments with that

TEST WHAT HAPPENS WHEN YOU CHANGE THE KEY THINGS GET SORTED BY, CAN WE GET THE NEXT OR PREV ELEMENT IF THE CARS MOVE OFF-SCREEN. IF NOT, STD::SETS SHOULD NOT BE USED

NO THIS DOES NOT WORK, THE POSITION SHOULD NOT BE USED AS A BASIS FOR AN STD::SET, THE STANDARD DOES NOT GUARANTEE DEFINED BEHAVIOR

Tests:
Create a car, verify that all stats are set correctly

Add a car to a road, add invalid location, at valid location, and on top of another car. Verify that errors are thrown when not ok

Verify that the car is associated with the road and the road with the car.

Try driving the car by updating: verify that exceptions are thrown when "overstepping" updates for car

Verify that after updating smaller steps, the location, speed etc. of the car is correct.

Try a single car on a lane, try stepping to update:

The function for getting the next update should start accelerating, then cruising. Car should detect that it is about to drive of the road and set an update at exit (in the case of hellhole, setting current road to nullptr). The car should then despawn and be de-associated with the road after.

Try three cars on same lame, try stepping to update:

Verify that the functions for getting front, end, and cars at certain positions works. Verify that cars do not overtake; try to verify that collisions are detected (throwing errors, depending on the implementation this may require intentionally breaking fail-safes)
