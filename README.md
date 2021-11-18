# COP3530-Project-3

To Do List:
- create a .csv file with 100,000 data points so that we can upload that data easily to our code base


- create our graph class with a graph constructor for the 100,000 covid data points using ADJACENCY LIST
-   each element in the adjacency list should be an Node so that it can hold the following data:
-       Node should include city name and vector of pairs for its neighbors/edge weight and number of covid cases
-   edges hold the distance between cities
-   
![image](https://user-images.githubusercontent.com/73550658/142418417-05684c9f-abc2-450e-9141-dd929f068f83.png)


- functions:
-   find shortest path from A to B      ** maybe we could offer two different options, "do you (the user) want the shortest path or the safest path?"
-   find safest path from A to B        ** this function would have to access the vertex's covid case number rather than comparing edge weight 
