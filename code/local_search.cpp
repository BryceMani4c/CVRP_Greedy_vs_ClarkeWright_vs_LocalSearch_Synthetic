#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
using namespace std;

struct Data{
    pair<double, double> depot;
    vector<pair<double, double>> customers;
    vector<int> demands;
    int vehicle_capacity;
    int num_customers;
};

static double euclidean_distance(pair<double, double> p1, pair<double, double> p2){
    double dx = p1.first - p2.first;
    double dy = p1.second - p2.second;
    return sqrt(dx*dx + dy*dy);
}

static double route_distance(const vector<int>& route, const Data& data){
    if(route.empty()){
        return 0.0;
    }
    double total = 0.0;
    total += euclidean_distance(data.depot, data.customers[route[0]]);
    for(size_t i = 0; i < route.size() - 1; i++){
        total += euclidean_distance(data.customers[route[i]], data.customers[route[i+1]]);
    }
    total += euclidean_distance(data.customers[route.back()], data.depot);
    return total;
}

vector<int> two_opt_route(const vector<int>& route, const Data& data){
    vector<int> best_route = route;
    double best_distance = route_distance(best_route, data);
    bool improved = true;

    while(improved){
        improved = false;
        for(int i = 0; i < (int)best_route.size() - 1; i++){
            for(int j = i + 1; j < (int)best_route.size(); j++){
                vector<int> new_route = best_route;
                reverse(new_route.begin() + i, new_route.begin() + j + 1);
                double new_distance = route_distance(new_route, data);
                if(new_distance < best_distance){
                    best_route = new_route;
                    best_distance = new_distance;
                    improved = true;
                }
            }
        }
    }

    return best_route;
}

vector<vector<int>> local_search(const vector<vector<int>>& routes, const Data& data){
    vector<vector<int>> improved_routes;
    for(const auto& route : routes){
        improved_routes.push_back(two_opt_route(route, data));
    }
    return improved_routes;
}
