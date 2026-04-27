#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <set>
#include <limits>
using namespace std;

struct Data{
    pair<double, double> depot;
    vector<pair<double, double>> customers;
    vector<int> demands;
    int vehicle_capacity;
    int num_customers;
};

struct Result{
    vector<vector<int>> routes;
    double total_distance;
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

static double total_solution_distance(const vector<vector<int>>& routes, const Data& data){
    double total = 0.0;
    for(const auto& route : routes){
        total += route_distance(route, data);
    }
    return total;
}

Result greedy_vrp(const Data& data){
    set<int> unvisited;
    for(int i = 0; i < (int)data.customers.size(); i++){
        unvisited.insert(i);
    }

    vector<vector<int>> routes;

    while(!unvisited.empty()){
        pair<double, double> current_location = data.depot;
        int current_load = 0;
        vector<int> current_route;

        while(true){
            int best_customer = -1;
            double best_distance = numeric_limits<double>::infinity();

            for(int customer_idx : unvisited){
                int demand = data.demands[customer_idx];
                if(current_load + demand <= data.vehicle_capacity){
                    double dist = euclidean_distance(current_location, data.customers[customer_idx]);
                    if(dist < best_distance){
                        best_distance = dist;
                        best_customer = customer_idx;
                    }
                }
            }

            if(best_customer == -1){
                break;
            }

            current_route.push_back(best_customer);
            current_load += data.demands[best_customer];
            current_location = data.customers[best_customer];
            unvisited.erase(best_customer);
        }

        routes.push_back(current_route);
    }

    Result result;
    result.routes = routes;
    result.total_distance = total_solution_distance(routes, data);
    return result;
}
