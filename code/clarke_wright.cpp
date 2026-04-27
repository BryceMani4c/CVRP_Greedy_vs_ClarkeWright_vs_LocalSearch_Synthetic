#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
#include <tuple>
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

static int route_demand(const vector<int>& route, const vector<int>& demands){
    int total = 0;
    for(int i : route){
        total += demands[i];
    }
    return total;
}

Result clarke_wright(const Data& data){
    int n = data.customers.size();

    vector<vector<int>> routes;
    for(int i = 0; i < n; i++){
        routes.push_back({i});
    }

    vector<tuple<double, int, int>> savings;
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            double s = euclidean_distance(data.depot, data.customers[i])
                     + euclidean_distance(data.depot, data.customers[j])
                     - euclidean_distance(data.customers[i], data.customers[j]);
            savings.push_back(make_tuple(s, i, j));
        }
    }

    sort(savings.begin(), savings.end(), [](const tuple<double, int, int>& a, const tuple<double, int, int>& b){
        return get<0>(a) > get<0>(b);
    });

    for(const auto& entry : savings){
        int i = get<1>(entry);
        int j = get<2>(entry);

        int route_i_idx = -1;
        int route_j_idx = -1;

        for(int k = 0; k < (int)routes.size(); k++){
            if(find(routes[k].begin(), routes[k].end(), i) != routes[k].end()){
                route_i_idx = k;
            }
            if(find(routes[k].begin(), routes[k].end(), j) != routes[k].end()){
                route_j_idx = k;
            }
        }

        if(route_i_idx == -1 || route_j_idx == -1 || route_i_idx == route_j_idx){
            continue;
        }

        vector<int> route_i = routes[route_i_idx];
        vector<int> route_j = routes[route_j_idx];

        bool i_at_start = (route_i.front() == i);
        bool i_at_end = (route_i.back() == i);
        bool j_at_start = (route_j.front() == j);
        bool j_at_end = (route_j.back() == j);

        if(!(i_at_start || i_at_end)){
            continue;
        }
        if(!(j_at_start || j_at_end)){
            continue;
        }

        int combined_demand = route_demand(route_i, data.demands) + route_demand(route_j, data.demands);
        if(combined_demand > data.vehicle_capacity){
            continue;
        }

        vector<int> merged_route;
        bool merged = false;

        if(i_at_end && j_at_start){
            merged_route = route_i;
            merged_route.insert(merged_route.end(), route_j.begin(), route_j.end());
            merged = true;
        }
        else if(i_at_start && j_at_end){
            merged_route = route_j;
            merged_route.insert(merged_route.end(), route_i.begin(), route_i.end());
            merged = true;
        }
        else if(i_at_start && j_at_start){
            merged_route = vector<int>(route_i.rbegin(), route_i.rend());
            merged_route.insert(merged_route.end(), route_j.begin(), route_j.end());
            merged = true;
        }
        else if(i_at_end && j_at_end){
            merged_route = route_i;
            merged_route.insert(merged_route.end(), route_j.rbegin(), route_j.rend());
            merged = true;
        }

        if(merged){
            int first = min(route_i_idx, route_j_idx);
            int second = max(route_i_idx, route_j_idx);
            routes.erase(routes.begin() + second);
            routes.erase(routes.begin() + first);
            routes.push_back(merged_route);
        }
    }

    Result result;
    result.routes = routes;
    result.total_distance = total_solution_distance(routes, data);
    return result;
}
