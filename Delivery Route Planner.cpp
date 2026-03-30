#include <iostream>
#include <queue>
#include <climits>
#include <string>
#include <unordered_map>

using namespace std;

const int MAX_LOCATIONS = 20;

struct Road {
    string destination;
    int distance;    // in meters
    int cost;        // in TK
    Road* next;
};

struct Location {
    string name;
    Road* roads;
};

struct QueueNode {
    string name;
    int value;       // can represent distance or cost

    bool operator>(const QueueNode& other) const {
        return value > other.value;
    }
};

class DeliveryRoutePlanner {
private:
    Location locations[MAX_LOCATIONS];
    int locationCount;

    int findLocationIndex(const string& name) {
        for (int i = 0; i < locationCount; i++) {
            if (locations[i].name == name) return i;
        }
        return -1;
    }

    void findBestPath(const string& start, const string& end, bool useCost,
                     unordered_map<string, string>& previous,
                     unordered_map<string, int>& distances,
                     unordered_map<string, int>& costs) {
        priority_queue<QueueNode, vector<QueueNode>, greater<QueueNode>> pq;

        // Initialize
        for (int i = 0; i < locationCount; i++) {
            distances[locations[i].name] = INT_MAX;
            costs[locations[i].name] = INT_MAX;
        }
        distances[start] = 0;
        costs[start] = 0;
        pq.push({start, 0});

        while (!pq.empty()) {
            QueueNode current = pq.top();
            pq.pop();

            if (current.name == end) break;

            int currentIdx = findLocationIndex(current.name);
            if (currentIdx == -1) continue;

            Road* road = locations[currentIdx].roads;
            while (road != nullptr) {
                int newDistance = distances[current.name] + road->distance;
                int newCost = costs[current.name] + road->cost;

                bool shouldUpdate = useCost ?
                    (newCost < costs[road->destination]) :
                    (newDistance < distances[road->destination]);

                if (shouldUpdate) {
                    distances[road->destination] = newDistance;
                    costs[road->destination] = newCost;
                    previous[road->destination] = current.name;
                    pq.push({road->destination, useCost ? newCost : newDistance});
                }
                road = road->next;
            }
        }
    }

    void printPath(const string& start, const string& end,
                  const unordered_map<string, string>& previous,
                  int distance, int cost, const string& mode) {
        if (previous.find(end) == previous.end()) {
            cout << "No " << mode << " route found!" << endl;
            return;
        }

        cout << "\n" << mode << " Delivery Route:" << endl;
        cout << "Total distance: " << distance << " meters" << endl;
        cout << "Total cost: " << cost << " TK" << endl;
        cout << "Path: ";

        string path[MAX_LOCATIONS];
        int pathLength = 0;
        string current = end;

        while (current != start) {
            path[pathLength++] = current;
            current = previous.at(current);
        }
        path[pathLength++] = start;

        for (int i = pathLength - 1; i >= 0; i--) {
            cout << path[i];
            if (i > 0) cout << " -> ";
        }
        cout << endl;
    }

public:
    DeliveryRoutePlanner() : locationCount(0) {
        for (int i = 0; i < MAX_LOCATIONS; i++) {
            locations[i].roads = nullptr;
        }
    }

    void addRoad(const string& from, const string& to, int distance, int cost = 0) {
        // Ensure locations exist
        bool fromExists = false, toExists = false;
        for (int i = 0; i < locationCount; i++) {
            if (locations[i].name == from) fromExists = true;
            if (locations[i].name == to) toExists = true;
        }

        if (!fromExists && locationCount < MAX_LOCATIONS) {
            locations[locationCount].name = from;
            locations[locationCount].roads = nullptr;
            locationCount++;
        }

        if (!toExists && locationCount < MAX_LOCATIONS) {
            locations[locationCount].name = to;
            locations[locationCount].roads = nullptr;
            locationCount++;
        }

        // Add road (bidirectional)
        int fromIdx = findLocationIndex(from);
        Road* newRoad = new Road{to, distance, cost, locations[fromIdx].roads};
        locations[fromIdx].roads = newRoad;

        int toIdx = findLocationIndex(to);
        newRoad = new Road{from, distance, cost, locations[toIdx].roads};
        locations[toIdx].roads = newRoad;
    }

    void findAndShowAllRoutes(const string& start, const string& end) {
        // For shortest distance
        unordered_map<string, string> dist_previous;
        unordered_map<string, int> dist_distances;
        unordered_map<string, int> dist_costs;

        // For lowest cost
        unordered_map<string, string> cost_previous;
        unordered_map<string, int> cost_distances;
        unordered_map<string, int> cost_costs;

        // Find shortest distance path
        findBestPath(start, end, false, dist_previous, dist_distances, dist_costs);

        // Find lowest cost path
        findBestPath(start, end, true, cost_previous, cost_distances, cost_costs);

        // Display both results
        printPath(start, end, dist_previous, dist_distances[end], dist_costs[end], "Shortest Distance");
        printPath(start, end, cost_previous, cost_distances[end], cost_costs[end], "Lowest Cost");
    }

    ~DeliveryRoutePlanner() {
        for (int i = 0; i < locationCount; i++) {
            Road* current = locations[i].roads;
            while (current != nullptr) {
                Road* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }
};

int main() {
    DeliveryRoutePlanner planner;

    // Add roads with distance (meters) and cost (TK)
    planner.addRoad("Restaurant", "Park", 500, 20);
    planner.addRoad("Restaurant", "Market", 300, 0);
    planner.addRoad("Market", "School", 400, 10);
    planner.addRoad("Park", "School", 600, 0);
    planner.addRoad("Park", "Office", 700, 50);
    planner.addRoad("School", "Office", 300, 0);
    planner.addRoad("Market", "Home", 800, 0);
    planner.addRoad("Office", "Home", 500, 20);

    string start, end;
    cout << "Enter starting location (default: Restaurant): ";
    getline(cin, start);
    if (start.empty()) start = "Restaurant";

    cout << "Enter delivery destination: ";
    getline(cin, end);

    // Automatically find and show both routes
    planner.findAndShowAllRoutes(start, end);

    return 0;
}
