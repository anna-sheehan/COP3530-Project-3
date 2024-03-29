using namespace std;
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <random>
#include <set>
#include <list>

class Graph {
    //Struct of city for keeping all the city information organized
    struct City {
        string cityName;
        int numCovidCases;

        City(int _numCovidCases, string _cityName) {
            numCovidCases = _numCovidCases;
            cityName = _cityName;
        }
    };
public:
    map<string, vector<pair<City, int>>> graph; // map<source, vector<destination, distance>>
private:
    map<string, vector<pair<City, int>>>::iterator it;
    vector<string> cityList;
    int numEdges = 0;
    int numCities = 0;

public:
    void insertEdge(string from, City to, int distance);
    bool isEdge(string from, string to);
    int getWeight(string from, string to);
    vector<string> getNeighbors(string vertex);
    void printGraph();
    void readCSVFindAllCities(string nameOfFile);
    void readCSVAddtoGraph(string nameOfFile, map<string, City>& mapOfallCities, vector<string>& allCities);
    vector<string> shortestDistance(const Graph& graph, string src, string destination);
    vector<string> safestCovidPath(const Graph& graph, string src, string destination);
    long long bellmanFordShortestPath(Graph& graph, string src, string dest);
    int bellmanFordSafestPath(Graph& graph, string src, string dest);
};

//Insert Edge between two vertex(cities)
void Graph::insertEdge(string from, City to, int distance)
{
    graph[from].push_back(make_pair(to, distance));
    numEdges++;
}

// Checks the existence of the edge between two cities
bool Graph::isEdge(string from, string to) 
{
    if (graph.find(from) == graph.end()) {
        return false;
    }
    else {
        for (int i = 0; i < graph[from].size(); i++) {
            if (graph[from][i].first.cityName == to) {
                return true;
            }
        }
    }
    return false;
}

//Return the weight of the edge between two cities
int Graph::getWeight(string from, string to) 
{
    if (graph.find(from) == graph.end()) {
        return 0;
    }
    else {
        for (int i = 0; i < graph[from].size(); i++) {
            if (graph[from][i].first.cityName == to) {
                return graph[from][i].second;
            }
        }
    }
    return 0;
}

//Get the neigbors of the city 
vector<string> Graph::getNeighbors(string vertex) 
{
    vector<string> neighbors;

    if (graph.find(vertex) == graph.end()) {
        return neighbors;
    }
    for (int i = 0; i < graph[vertex].size(); i++) {
        neighbors.push_back(graph[vertex][i].first.cityName);
    }
    return neighbors;
}

//Print out the graph such as One city to another city with distances
void Graph::printGraph() 
{
    int cnt = 0;
    for (it = graph.begin(); it != graph.end(); ++it)
    {
        cnt++;
        cout << "From " << it->first << ": ";
        for (int j = 0; j < it->second.size(); j++) {
            cout << "to " << it->second[j].first.cityName << " is " << it->second[j].second << " miles, ";
        }
        cout << endl;
    }
}
//Reads the csv file which contains two column: City Name and Positive Covid Cases
void Graph::readCSVFindAllCities(string nameOfFile) 
{       
    vector<string> allCities;         //Creates a vector all cities with name of city
    map<string, City> mapOfallCities; //map<city name, City node>
    ifstream openFile;
    openFile.open(nameOfFile);
    if (openFile.is_open())
    {
        string infoOfCity;
        getline(openFile, infoOfCity);
        while (getline(openFile, infoOfCity))
        {
            istringstream stream(infoOfCity);
            string cityName, stateId, covidCase = "";
            int covidCaseNo = 0;
            getline(stream, cityName, ','); 
         //getting the confirmed covid case no 
            getline(stream, covidCase, ',');
            covidCaseNo = stoi(covidCase);           
            City cityData(covidCaseNo, cityName); //stores the city name and covid case no
            mapOfallCities.emplace(cityName, cityData);   
            allCities.push_back(cityName);     
            numCities++;
        }
    }
    else
    {
        cout << "File is Not Open!" << endl;
    }

    // call readCSVAddtoGraph to insert cities into our graph
    readCSVAddtoGraph(nameOfFile, mapOfallCities, allCities);
    cityList = allCities;
}

//This is the exactly same as as the reading csv file function just with added functioanlity to insert city in our graph 
void Graph::readCSVAddtoGraph(string nameOfFile, map<string, City>& mapOfallCities, vector<string>& allCities) {
    ifstream openFile;
    openFile.open(nameOfFile);
    if (openFile.is_open())
    {
        string infoOfCity;
        getline(openFile, infoOfCity);
        while (getline(openFile, infoOfCity))
        {
            istringstream stream(infoOfCity);
            string cityName, stateId, covidCase = "";
            int covidCaseNo = 0;
            getline(stream, cityName, ',');
            getline(stream, covidCase, ',');
            covidCaseNo = stoi(covidCase);

            //randomly generate four neighbors using mapOfAllCities and use random distances
            int index = 0;
            int distance = 0;
            City newCity(covidCaseNo, cityName);
            std::random_device rd;               // obtain a random number from hardware
            std::mt19937 gen(rd());                  // seed the generator
            std::uniform_int_distribution<> distr(0, allCities.size() - 1); // define the range
            std::mt19937 gen2(rd());                            // seed the generator
            std::uniform_int_distribution<> distr2(0, 5000); // define the range
           
            int adjacentNeighboSize = 4;
            for (int i = 0; i < adjacentNeighboSize; i++) {
                index = distr(gen);
                distance = distr2(gen2);
                //should help us to avoid self loop
                if (cityName == allCities[index])  
                {
                    adjacentNeighboSize++;
                    continue;
                }
                //insert new city into graph using insertEdge
                insertEdge(newCity.cityName, mapOfallCities.at(allCities[index]), distance);
            }
        }
    }
    else
    {
        cout << "File is Not Open!" << endl;
    }
}

//Calculate the shortest distance between source city to destination, using distance as weights 
//Use the Dijekstra's algorithm with adjacency list to give us shortest path that can be followed to travel 
vector<string> Graph::shortestDistance(const Graph& graph, string src, string destination)
{
    int v = graph.numCities;
    int infinity = ~(1 << 31);   //Works similar to INT_MAX
    map<string, long long> d;   //used long long to avoid integer overflow for large distance  //Stores the distances
    map<string, string> p;     //Stores Predecessor
    set<string> S = { src };    //Source vertex
    set<string> V_S;            //vertices that need to be processed

    //iterte through all cities to insert into set 
    for (int i = 0; i < cityList.size(); i++) {
        V_S.insert(cityList[i]);
    }

    //iterating through all cities  
    for (int i = 0; i < cityList.size(); i++) {
        p[cityList[i]] = src;
        for (auto x : graph.graph.at(src))
        {
            if (x.first.cityName == cityList[i])
            {
                d[cityList[i]] = x.second;
            }
            else
            {
                d[cityList[i]] = infinity;
            }
        }
    }
    d[src] = 0;

    while (!V_S.empty())
    {
        int smallVal = infinity;
        string indexOfsmall;

        for (auto j : V_S)
        {
            if (d[j] <= smallVal) {
                smallVal = d[j];
                indexOfsmall = j;
            }
        }
        V_S.erase(indexOfsmall);
        S.insert(indexOfsmall);
        //Go through neigbors at that index
        for (auto x : graph.graph.at(indexOfsmall))
        {
            if ((d[indexOfsmall] + x.second) < d[x.first.cityName]) {
                d[x.first.cityName] = d[indexOfsmall] + x.second;
                p[x.first.cityName] = indexOfsmall;
            }
        }
    }
    //Stores the path in a vector 
    vector<string> shortestPath;
    bool working = true;
    string temp = destination;
    while (working) {
        if (temp == src) {
            shortestPath.push_back(temp);
            working = false;
        }
        else {
            shortestPath.push_back(temp);
            temp = p[temp];
        }
    }
    return shortestPath;  //This stores the path in the backward direction.
}

//Calculate the safest path between source city to destination, using covid cases as weights 
//Use the Dijekstra's algorithm with adjacency list to give us safest path that can be followed to travel to avoid covid hotspot
//Works similar to shortest distance
vector<string> Graph::safestCovidPath(const Graph& graph, string src, string destination) {
    int v = graph.numCities;
    int infinity = ~(1 << 31);
    map<string, int> d;
    map<string, string> p;
    set<string> S = { src };
    set<string> V_S;

    //iterate through all cities
    for (int i = 0; i < cityList.size(); i++) {
        V_S.insert(cityList[i]);
    }

    //iterating through all cities
    for (int i = 0; i < cityList.size(); i++) {
        p[cityList[i]] = src;
        for (auto x : graph.graph.at(src))
        {
            if (x.first.cityName == cityList[i])
            {
                d[cityList[i]] = x.first.numCovidCases;
            }
            else
            {
                d[cityList[i]] = infinity;
            }
        }
    }
    d[src] = 0;

    while (!V_S.empty())
    {
        int smallVal = infinity;
        string indexOfsmall;

        for (auto j : V_S)
        {
            if (d[j] <= smallVal) {
                smallVal = d[j];
                indexOfsmall = j;
            }
        }
        V_S.erase(indexOfsmall);
        S.insert(indexOfsmall);

        for (auto x : graph.graph.at(indexOfsmall))
        {
            if ((d[indexOfsmall] + x.first.numCovidCases) < d[x.first.cityName]) {
                d[x.first.cityName] = d[indexOfsmall] + x.first.numCovidCases;
                p[x.first.cityName] = indexOfsmall;
            }
        }
    }
    vector<string> safestPath;
    bool working = true;
    string temp = destination;
    while (working) {
        if (temp == src) {
            safestPath.push_back(temp);
            working = false;
        }
        else {
            safestPath.push_back(temp);
            temp = p[temp];
        }
    }
    return safestPath; //this stores the path in backeward direction like from desination to source //handled in the main
}

//Gives us the minimum total distances that have to be travelled to reach the destination
long long Graph::bellmanFordShortestPath(Graph& graph, string src, string dest)
{
    int v = graph.numCities;
    int infinity = ~(1 << 31);
    map<string, long long> d;   
    map<string, string> p;
    set<string> S = { src };
    set<string> V_S;

    //iterte through all cities
    for (int i = 0; i < cityList.size(); i++) {
        V_S.insert(cityList[i]);
    }

    //iterating through all cities
    for (int i = 0; i < cityList.size(); i++) {
        p[cityList[i]] = src;
        for (auto x : graph.graph.at(src))
        {
            if (x.first.cityName == cityList[i])
            {
                d[cityList[i]] = x.second;
            }
            else
            {
                d[cityList[i]] = infinity;
            }
        }
    }
    d[src] = 0;

    while (!V_S.empty())
    {
        int smallVal = infinity;
        string indexOfsmall;

        for (auto j : V_S)
        {
            if (d[j] <= smallVal) {
                smallVal = d[j];
                indexOfsmall = j;
            }
        }
        V_S.erase(indexOfsmall);
        S.insert(indexOfsmall);

        for (auto x : graph.graph.at(indexOfsmall))
        {
            if ((d[indexOfsmall] + x.second) < d[x.first.cityName]) {
                d[x.first.cityName] = d[indexOfsmall] + x.second;
                p[x.first.cityName] = indexOfsmall;
            }
        }
    }

    // check for negative cycle
    for (auto x : graph.graph) {
        for (int i = 0; i < x.second.size(); i++) {
            if (d[x.first] == (d[x.second[i].first.cityName] + x.second[i].second)) {
                cout << "Error! Graph Contains a Negative Cycle" << endl;
            }
        }
    }

    return d[dest];  //This stores the path in the backward direction.
}
//Gives the mimimum total number of covid cases in the travel path from source to destination
int Graph::bellmanFordSafestPath(Graph& graph, string src, string dest) {
    int v = graph.numCities;
    int infinity = ~(1 << 31);
    map<string, int> d;
    map<string, string> p;
    set<string> S = { src };
    set<string> V_S;

    //iterate through all cities
    for (int i = 0; i < cityList.size(); i++) {
        V_S.insert(cityList[i]);
    }

    //iterating through all cities
    for (int i = 0; i < cityList.size(); i++) {
        p[cityList[i]] = src;
        for (auto x : graph.graph.at(src))
        {
            if (x.first.cityName == cityList[i])
            {
                d[cityList[i]] = x.first.numCovidCases;
            }
            else
            {
                d[cityList[i]] = infinity;
            }
        }
    }
    d[src] = 0;

    while (!V_S.empty())
    {
        int smallVal = infinity;
        string indexOfsmall;

        for (auto j : V_S)
        {
            if (d[j] <= smallVal) {
                smallVal = d[j];
                indexOfsmall = j;
            }
        }
        V_S.erase(indexOfsmall);
        S.insert(indexOfsmall);

        for (auto x : graph.graph.at(indexOfsmall))
        {
            if ((d[indexOfsmall] + x.first.numCovidCases) < d[x.first.cityName]) {
                d[x.first.cityName] = d[indexOfsmall] + x.first.numCovidCases;
                p[x.first.cityName] = indexOfsmall;
            }
        }
    }

    //check for negative cycle

    for (auto x : graph.graph) {
        for (int i = 0; i < x.second.size(); i++) {
            if (d[x.first] == (d[x.second[i].first.cityName] + x.second[i].first.numCovidCases)) {
                cout << "Error! Graph Contains a Negative Cycle" << endl;
            }
        }
    }

    return d[dest]; //this stores the path in backeward direction like from desination to source
}



int main() {
    Graph* graph = new Graph();
    //switch to this read csv file for full 100k points
    //graph->readCSVFindAllCities("coviddatapoints.csv");
    //switch to this for smaller csv of major cities
    graph->readCSVFindAllCities("smallerdatapoints.csv");
    
    bool runProgram = true;

    string source;
    string destination;
    string option;
    while (runProgram) {
        cout << "Welcome to the Covid travel Calculator " << endl;
        cout << "Please select an option: " << endl;
        cout << "1. Find shortest route between locations" << endl;
        cout << "2. Find safest route between locations" << endl;
        cout << "3. Compare safest route and shortest route between locations" << endl;
        cout << "4. Print Graph" << endl;
        cout << "5. Exit Program" << endl;
        cin >> option;
        if (option == "1") {
            cout << "Please enter a starting location: " << endl;
            cin >> ws;
            getline(cin, source);
            cout << "Please enter a destination: " << endl;
            cin >> ws;
            getline(cin, destination);
            cout << "Calculating..." << endl;
            vector<string> vec;

            vec = graph->shortestDistance(*graph, source, destination);
            cout << "The shortest path from " << source << " to " << destination << " is: " /* put results from func*/ << endl;
            for (int i = 0; i < vec.size(); i++) {
                cout << vec[vec.size() - 1 - i];
                if (i != vec.size() - 1) {
                    cout << " -> ";
                }
            }
            cout << endl;
            int bfsp = graph->bellmanFordShortestPath(*graph, source, destination);
            cout << endl;
            cout << "According to Bellman-ford, from " << source << " to " << destination << " is " << bfsp << " miles." << endl;
        }
        if (option == "2") {

            cout << "Please enter a starting location: " << endl;
            cin >> ws;
            getline(cin, source);
            cout << "Please enter a destination: " << endl;
            cin >> ws;
            getline(cin, destination);
            cout << "Calculating..." << endl;          
            vector<string> vec;            
            cout << "The safest path from " << source << " to " << destination << " is: " /* put results from func*/ << endl;

            vec = graph->safestCovidPath(*graph, source, destination);
            for (int i = 0; i < vec.size(); i++) {
                cout << vec[vec.size() - 1 - i];
                if (i != vec.size() - 1) {
                    cout << " -> ";
                }
            }
            cout << endl;
           
            int bfcovidpath = graph->bellmanFordSafestPath(*graph, source, destination);
            cout << endl;
            cout << "According to Bellman-ford, this route contains " << bfcovidpath << " covid cases." << endl;
            continue;
        }
        if (option == "3") {
            //find quick way to compare, showing cases/miles between two cities
            cout << "Please enter a starting location: " << endl;
            cin >> ws;
            getline(cin, source);
            cout << "Please enter a destination: " << endl;
            cin >> ws;
            getline(cin, destination);
            cout << "Calculating..." << endl;
            vector<string> vec;


            vec = graph->shortestDistance(*graph, source, destination);
            cout << "The shortest path from " << source << " to " << destination << " is: " /* put results from func*/ << endl;
            for (int i = 0; i < vec.size(); i++) {
                cout << vec[vec.size() - 1 - i];
                if (i != vec.size() - 1) {
                    cout << " -> ";
                }
            }
            cout << endl;
            int bfsp = graph->bellmanFordShortestPath(*graph, source, destination);
            cout << endl;
            cout << "According to Bellman-ford, from " << source << " to " << destination << " is " << bfsp << " miles." << endl;
            cout << "The safest path from " << source << " to " << destination << " is: " /* put results from func*/ << endl;

            vec = graph->safestCovidPath(*graph, source, destination);
            for (int i = 0; i < vec.size(); i++) {
                cout << vec[vec.size() - 1 - i];
                if (i != vec.size() - 1) {
                    cout << " -> ";
                }
            }
            cout << endl;
            int bfcovidpath = graph->bellmanFordSafestPath(*graph, source, destination);
            //cout << endl;
            cout << "According to Bellman-ford, this route contains " << bfcovidpath << " covid cases." << endl;
            cout << endl;
            continue;
        }
        if (option == "4") {
            graph->printGraph();
        }
        if (option == "5") {
            break;
        }
        else {
            continue;
        }

    }

}
