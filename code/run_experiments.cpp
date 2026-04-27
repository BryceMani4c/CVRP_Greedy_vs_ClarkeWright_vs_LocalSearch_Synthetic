/*
    File: run_experiments.cpp
    Authors: Bryson Bargas and Noah Boggess

    How to compile (in the current folder cmd): g++ greedy.cpp clarke_wright.cpp local_search.cpp run_experiments.cpp -o run_experiments.exe
    How to run: run_experiments.exe
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <iomanip>
using namespace std;
namespace fs = std::filesystem;

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

Result greedy_vrp(const Data& data);
Result clarke_wright(const Data& data);
vector<vector<int>> local_search(const vector<vector<int>>& routes, const Data& data);

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

Data load_solomon(const string& filepath){
    ifstream f(filepath);
    vector<string> lines;
    string line;
    while(getline(f, line)){
        lines.push_back(line);
    }

    istringstream vehicle_iss(lines[4]);
    string vehicle_token;
    int capacity = 0;
    vehicle_iss >> vehicle_token >> capacity;

    pair<double, double> depot;
    vector<pair<double, double>> customers;
    vector<int> demands;

    for(size_t idx = 9; idx < lines.size(); idx++){
        string s = lines[idx];

        size_t start = s.find_first_not_of(" \t\r\n");
        if(start == string::npos){
            continue;
        }
        size_t end = s.find_last_not_of(" \t\r\n");
        s = s.substr(start, end - start + 1);
        if(s.empty()){
            continue;
        }

        istringstream iss(s);
        vector<string> parts;
        string token;
        while(iss >> token){
            parts.push_back(token);
        }
        if(parts.size() < 4){
            continue;
        }

        int cust_id = stoi(parts[0]);
        double x = stod(parts[1]);
        double y = stod(parts[2]);
        int demand = stoi(parts[3]);

        if(cust_id == 0){
            depot = make_pair(x, y);
        }
        else{
            customers.push_back(make_pair(x, y));
            demands.push_back(demand);
        }
    }

    Data data;
    data.depot = depot;
    data.customers = customers;
    data.demands = demands;
    data.vehicle_capacity = capacity;
    data.num_customers = (int)customers.size();
    return data;
}

void print_separator(){
    cout << string(60, '-') << endl;
}

void print_routes(const vector<vector<int>>& routes, const Data& data){
    for(size_t i = 0; i < routes.size(); i++){
        const vector<int>& route = routes[i];
        double dist = route_distance(route, data);
        int demand = 0;
        for(int c : route){
            demand += data.demands[c];
        }
        int stops = (int)route.size();
        cout << "  Route " << (i + 1) << ": " << stops << " stops, demand=" << demand
             << ", distance=" << fixed << setprecision(2) << dist << endl;
        cout << "           customers: [";
        for(size_t k = 0; k < route.size(); k++){
            cout << route[k];
            if(k + 1 < route.size()){
                cout << ", ";
            }
        }
        cout << "]" << endl;
    }
}

void run_test(const Data& data){
    int n = data.num_customers;
    int capacity = data.vehicle_capacity;
    int total_demand = 0;
    for(int d : data.demands){
        total_demand += d;
    }
    cout << "Dataset: " << n << " customers, capacity=" << capacity
         << ", total demand=" << total_demand << endl;
    print_separator();

    cout << "\n[1] GREEDY NEAREST NEIGHBOR" << endl;
    print_separator();

    auto start = chrono::high_resolution_clock::now();
    Result greedy_result = greedy_vrp(data);
    auto stop = chrono::high_resolution_clock::now();
    double greedy_time = chrono::duration<double>(stop - start).count();

    cout << "  Total distance : " << fixed << setprecision(2) << greedy_result.total_distance << endl;
    cout << "  Vehicles used  : " << greedy_result.routes.size() << endl;
    cout << "  Runtime        : " << fixed << setprecision(6) << greedy_time << " seconds" << endl;
    cout << endl;
    print_routes(greedy_result.routes, data);

    cout << "\n[2] CLARKE-WRIGHT SAVINGS" << endl;
    print_separator();

    start = chrono::high_resolution_clock::now();
    Result cw_result = clarke_wright(data);
    stop = chrono::high_resolution_clock::now();
    double cw_time = chrono::duration<double>(stop - start).count();

    cout << "  Total distance : " << fixed << setprecision(2) << cw_result.total_distance << endl;
    cout << "  Vehicles used  : " << cw_result.routes.size() << endl;
    cout << "  Runtime        : " << fixed << setprecision(6) << cw_time << " seconds" << endl;
    cout << endl;
    print_routes(cw_result.routes, data);

    cout << "\n[3] LOCAL SEARCH (2-OPT) improving Greedy solution" << endl;
    print_separator();

    start = chrono::high_resolution_clock::now();
    vector<vector<int>> ls_routes = local_search(greedy_result.routes, data);
    stop = chrono::high_resolution_clock::now();
    double ls_time = chrono::duration<double>(stop - start).count();

    double ls_distance = total_solution_distance(ls_routes, data);

    cout << "  Total distance : " << fixed << setprecision(2) << ls_distance << endl;
    cout << "  Vehicles used  : " << ls_routes.size() << endl;
    cout << "  Runtime        : " << fixed << setprecision(6) << ls_time << " seconds" << endl;
    cout << endl;
    print_routes(ls_routes, data);

    cout << "\n" << string(60, '=') << endl;
    cout << "SUMMARY COMPARISON" << endl;
    cout << string(60, '=') << endl;
    cout << left << setw(30) << "Algorithm" << right << setw(10) << "Distance"
         << setw(10) << "Vehicles" << setw(10) << "Time (s)" << endl;
    cout << string(60, '-') << endl;
    cout << left << setw(30) << "Greedy Nearest Neighbor" << right << fixed << setprecision(2)
         << setw(10) << greedy_result.total_distance
         << setw(10) << greedy_result.routes.size()
         << setprecision(6) << setw(10) << greedy_time << endl;
    cout << left << setw(30) << "Clarke-Wright Savings" << right << fixed << setprecision(2)
         << setw(10) << cw_result.total_distance
         << setw(10) << cw_result.routes.size()
         << setprecision(6) << setw(10) << cw_time << endl;
    cout << left << setw(30) << "Local Search (2-opt)" << right << fixed << setprecision(2)
         << setw(10) << ls_distance
         << setw(10) << ls_routes.size()
         << setprecision(6) << setw(10) << ls_time << endl;
    cout << string(60, '-') << endl;

    double greedy_dist = greedy_result.total_distance;
    if(greedy_dist > 0){
        double cw_improvement = ((greedy_dist - cw_result.total_distance) / greedy_dist) * 100;
        double ls_improvement = ((greedy_dist - ls_distance) / greedy_dist) * 100;
        cout << "\n  Clarke-Wright vs Greedy : " << showpos << fixed << setprecision(2)
             << cw_improvement << "% distance" << noshowpos << endl;
        cout << "  Local Search vs Greedy : " << showpos << fixed << setprecision(2)
             << ls_improvement << "% distance" << noshowpos << endl;
    }
    cout << endl;
}

string pick_dataset(){
    fs::path candidates[] = {
        fs::current_path() / "data",
        fs::current_path() / ".." / "data",
        fs::current_path().parent_path() / "data"
    };

    fs::path data_dir;
    bool found = false;
    for(const auto& c : candidates){
        if(fs::is_directory(c)){
            data_dir = c;
            found = true;
            break;
        }
    }

    if(!found){
        cout << "Error: data/ folder not found." << endl;
        return "";
    }

    vector<string> files;
    for(const auto& entry : fs::directory_iterator(data_dir)){
        if(entry.path().extension() == ".txt"){
            files.push_back(entry.path().filename().string());
        }
    }
    sort(files.begin(), files.end());

    if(files.empty()){
        cout << "Error: No .txt datasets found in data/ folder." << endl;
        return "";
    }

    cout << "Available datasets in data/:" << endl;
    cout << string(40, '-') << endl;
    for(size_t i = 0; i < files.size(); i++){
        cout << "  [" << (i + 1) << "] " << files[i] << endl;
    }
    cout << endl;

    while(true){
        cout << "Select a dataset (1-" << files.size() << "): ";
        string choice;
        getline(cin, choice);

        size_t start = choice.find_first_not_of(" \t\r\n");
        if(start != string::npos){
            size_t end = choice.find_last_not_of(" \t\r\n");
            choice = choice.substr(start, end - start + 1);
        }
        else{
            choice = "";
        }

        try{
            int idx = stoi(choice);
            if(idx >= 1 && idx <= (int)files.size()){
                return (data_dir / files[idx - 1]).string();
            }
            else{
                cout << "  Please enter a number between 1 and " << files.size() << "." << endl;
            }
        }
        catch(...){
            cout << "  Please enter a valid number." << endl;
        }
    }
}

void save_results(const string& dataset_name, const string& report){
    fs::path candidates[] = {
        fs::current_path() / "results",
        fs::current_path() / ".." / "results",
        fs::current_path().parent_path() / "results"
    };

    fs::path results_dir;
    bool found = false;
    for(const auto& c : candidates){
        if(fs::is_directory(c)){
            results_dir = c;
            found = true;
            break;
        }
    }

    if(!found){
        cout << "Could not find results/ folder." << endl;
        return;
    }

    string base = dataset_name;
    size_t dot = base.find_last_of('.');
    if(dot != string::npos){
        base = base.substr(0, dot);
    }
    fs::path outpath = results_dir / (base + "_results.txt");

    ofstream of(outpath);
    if(!of){
        cout << "Could not open " << outpath.string() << " for writing." << endl;
        return;
    }
    of << report;
    of.close();
    cout << "Saved results to " << outpath.string() << endl;
}

int main(){
    string filepath = pick_dataset();
    if(filepath.empty()){
        return 0;
    }

    string name = fs::path(filepath).filename().string();
    cout << "\nLoading Solomon instance: " << name << "\n" << endl;
    Data data = load_solomon(filepath);

    stringstream ss;
    streambuf* old_cout = cout.rdbuf(ss.rdbuf());
    run_test(data);
    cout.rdbuf(old_cout);

    cout << ss.str();

    cout << "Save results to results/ folder? (y/n): ";
    string choice;
    getline(cin, choice);
    if(choice == "y" || choice == "Y"){
        save_results(name, ss.str());
    }

    return 0;
}
