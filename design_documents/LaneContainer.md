A custom container for storing vehicles on a road
----------
no default container really works well in this case, std::set comes close ... but I do not trust it to always work when the sorting key changes.


This container will only be used for the RoadVehicle class, but will work for anything T, with a < operator for T and double

This must be a fully functioning C++ container, compatible with all algorithms in the \<algorithm\> header (it will not be a sequence container, since the order of insertion fundamentally does not matter)




The pre-requisit for this container is that vehicles are sorted based on position.
position must be between 0 and length.

position can change without the container knowing about it (e.g. someone else stores a pointer to the vehicle and tell it to update), but we require that the relative ordering of the cars never change (overtaking requires deletion of the car on one lane, and insertion in another), the container should optionally have a "detect sorting errors" function

Vehicles can be inserted in O(log(N)) time based on position, a version of lower bound and upper bound should exist (to get get vehicles before or after an target position, or before or after any other vehicle), all this should use the CURRENT position

Random insertion and deletion must be possible (corresponding to overtaking)
allow fast insertion and deletion of vehicles at any location (should be O(log n)
very fast insertion of vehicles at one end, and deletion in other (this is the main mode vehicles enter and leave) O(1)

Getting next and previous car should be O(1)

For this reason, a AVL tree + double Linked List system will be used
