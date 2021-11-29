using namespace std;
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>

class Graph {

    struct City {
        string cityName;
        int numCovidCases;

        City(int _numCovidCases, string _cityName) {
            numCovidCases = _numCovidCases;
            cityName = _cityName;
        }
    };
private:
    map<string, vector<pair<City, int>>> graph; // map<source, vector<destination, distance>>
    map<string, vector<pair<City, int>>>::iterator it;
    map<string, City> mapOfAllCities;
    
public:
    void insertEdge(string from, City to, int distance);
    bool isEdge(string from, string to);
    int getWeight(string from, string to);
    vector<string> getNeighbors(string vertex);
    void printGraph();
    void readCSVFindAllCities(string nameOfFile);
    void readCSVAddtoGraph(string nameOfFile, map<string, City> &mapOfallCities, vector<string> &allCities);

};

void Graph::insertEdge(string from, City to, int distance) {

    //Update: what is the best set up for our map? I think we can do string here right??
    graph[from].push_back(make_pair(to, distance));
}

bool Graph::isEdge(string from, string to) {

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

int Graph::getWeight(string from, string to) {

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

vector<string> Graph::getNeighbors(string vertex) {

    vector<string> neighbors;

    if (graph.find(vertex) == graph.end()) {
        return neighbors;
    }
    for (int i = 0; i < graph[vertex].size(); i++) {
        neighbors.push_back(graph[vertex][i].first.cityName);
    }
    return neighbors;
}

void Graph::printGraph()
{
    int cnt = 0;
    for(it = graph.begin(); it != graph.end(); ++it)
    {
        cnt++;
        cout << "From " << it->first << ": ";
        for(int j = 0; j < it->second.size(); j++) {
            cout << "to " << it->second[j].first.cityName << " is " << it->second[j].second << " miles, ";
        }
        cout << endl;
    }
}

void Graph::readCSVFindAllCities(string nameOfFile) {
    vector<string> allCities;
    map<string, City> mapOfallCities; //map<city name, City node>
    ifstream openFile;
    openFile.open(nameOfFile);
    if (openFile.is_open())
    {
        string infoOfCity;
        getline(openFile, infoOfCity);
        while (getline(openFile,infoOfCity))
        {
            istringstream stream(infoOfCity);
            string cityName, stateId, covidCase="";
            int covidCaseNo = 0;
            
            getline(stream, cityName, ',');
            getline(stream, stateId, ',');
            cityName = cityName + stateId;
            
            //getting the confirmed covid case no
            getline(stream, covidCase, ',');
            covidCaseNo = stoi(covidCase);

            
            

            //Update: no need for third column in csv file, please get rid of it   //Done
            //Now this function's purpose is to create a map of all the cities in the csv file
            City cityData(covidCaseNo, cityName);
            mapOfallCities.emplace(cityName, cityData);
            allCities.push_back(cityName);
        }   
    }
    else
    {
        cout << "File is Not Open!" << endl;
    }

    //Update: call new readCSV file function to insert cities into our graph
    readCSVAddtoGraph(nameOfFile, mapOfallCities, allCities);
}
//Now this function's purpose is to insert cities into our graph
void Graph::readCSVAddtoGraph(string nameOfFile, map<string, City> &mapOfallCities, vector<string> &allCities) {
    ifstream openFile;
    openFile.open(nameOfFile);
    if (openFile.is_open())
    {
        string infoOfCity;
        getline(openFile, infoOfCity);
        while (getline(openFile,infoOfCity))
        {
            istringstream stream(infoOfCity);
            string cityName, stateId, covidCase="";
            int covidCaseNo = 0;

            getline(stream, cityName, ',');
            getline(stream, stateId, ',');
            cityName = cityName + stateId;

            getline(stream, covidCase, ',');
            covidCaseNo = stoi(covidCase);

      
            //Update: no need for third column in csv file, please get rid of it   //Done
            //Now this function's purpose is to insert cities into our graph

            //Update: insert new city into graph using insertEdge
            //randomly generate four neighbors using mapOfAllCities and use random distances
            int index = 0;
            int distance = 0;
            City newCity(covidCaseNo,cityName);
            std::random_device rd; // obtain a random number from hardware
            std::mt19937 gen(rd()); // seed the generator
            std::uniform_int_distribution<> distr(0, names.size()); // define the range
            std::mt19937 gen2(rd()); // seed the generator
            std::uniform_int_distribution<> distr2(0, 10000); // define the range
            //auto temp = mapOfallCities[names.at(index)];
            for(int i = 0; i < 4; i++) {

                index = distr(gen);
                distance = distr2(gen2);
                insertEdge(newCity,mapOfallCities.at(names[index]),distance);
            }


        }
    }
    else
    {
        cout << "File is Not Open!" << endl;
    }
}

int main() {
    /*
    Graph *graph = new Graph;
    graph->insertEdge("Orlando", "LA", 500);
    graph->insertEdge("Orlando", "New Orleans", 200);
    graph->insertEdge("LA", "Orlando", 500);
    graph->insertEdge("New Orleans" , "Orlando", 200);
    graph->printGraph();
     */

}

