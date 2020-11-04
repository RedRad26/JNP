#include<bits/stdc++.h>
using namespace std;

const string CAR_RGX = "([a-zA-Z0-9]{3,11})";
const string ROAD_RGX = "([AS][1-9][0-9]{0,2})";
const string DISTANCE_RGX = "([1-9][0-9]{0,5},[0-9]|0,[0-9])";
const string OPTIONAL_SPACES_RGX = "\\s*";
const string SPACES_RGX = "\\s+";

const regex transitLine("^" + OPTIONAL_SPACES_RGX +
                              CAR_RGX + SPACES_RGX +
                              ROAD_RGX + SPACES_RGX +
                              DISTANCE_RGX + OPTIONAL_SPACES_RGX + "$");
const regex queryLine("^" + OPTIONAL_SPACES_RGX +
                            "\\?" + OPTIONAL_SPACES_RGX +
                            "(" + CAR_RGX + "|" + ROAD_RGX + ")?" +
                            OPTIONAL_SPACES_RGX + "$");
const regex roadName(ROAD_RGX);

using Car = tuple <string /*registration*/, bool /*drove on A-type road*/, long long /*distance on A-type road*/,
                                            bool /*drove on S-type road*/, long long /*distance on S-Type road*/>;

using Road = tuple <string /*name*/, bool /*road type: false-A, true-S*/, long long /*total distance driven*/>;
using InputLine = tuple <string /*line content*/, int /*line number*/>;
using Transit = tuple <InputLine, string /*car registration*/, string /*road number*/, long long /*distance point on road*/>;
using Query = tuple <InputLine, string /*queried string*/>;
using RoadName = tuple <int /*road number*/, char /*road type*/>;

namespace {
    map<string /*car registration*/, Transit> activeEntries;
    map<RoadName, Road> roads;
    map<string /*car registry*/, Car> cars;

    RoadName encodeRoadName (string& name) {
        return {stoi(name.substr(1)), name[0]};
    }

    string decodeRoadName (RoadName name) {
        return get<1>(name) + to_string(get<0>(name));
    }

    bool isProperRoadName (string& name) {
        smatch match;
        return regex_match(name, match, roadName);
    }

    Car& getCar(string& carRegistration) {
        auto carLoc = cars.find(carRegistration);
        if (carLoc != cars.end())
            return carLoc->second;
        return cars[carRegistration] = {carRegistration, false, 0, false, 0};
    }

    Road& getRoad(RoadName roadName) {
        auto roadLoc = roads.find(roadName);
        if (roadLoc != roads.end())
            return roadLoc->second;
        if (get<1>(roadName) == 'A')
            return roads[roadName] = {decodeRoadName(roadName), false, 0};
        return roads[roadName] = {decodeRoadName(roadName), true, 0};
    }

    string printDecimal (long long num) {
        string res;
        res += to_string(num / 10) + "," + to_string(num % 10);
        return res;
    }

    string carDescription(Car& car) {
        string res;
        res += get<0>(car);
        res += (get<1>(car) ? " A " + printDecimal(get<2>(car)) : "");
        res += (get<3>(car) ? " S " + printDecimal(get<4>(car)) : "");

        if (!get<1>(car) and !get<3>(car)) return "";
        return res;
    }

    string roadDescription(Road& road) {
        string res;
        res += get<0>(road) + " " + printDecimal(get<2>(road));
        return res;
    }

    string errorLine(InputLine& line) {
        return "Error in line " + to_string(get<1>(line)) + ": " + get<0>(line);
    } 

    void manageTransit(Transit& transit) {
        string &carRegistry = get<1>(transit);
        string &roadName = get<2>(transit);
        long long leavingPoint = get<3>(transit);

        auto lastOccurence = activeEntries.find(carRegistry);
        if (lastOccurence != activeEntries.end()) {
            if (get<2>(lastOccurence->second) == roadName) {
                Road& exitedRoad = getRoad(encodeRoadName(roadName));
                Car& exitingCar = getCar(carRegistry);
                long long drivenDistance = abs(leavingPoint - get<3>(lastOccurence->second));

                get<2>(exitedRoad) += drivenDistance;

                if (get<1>(exitedRoad) == false) {
                    get<1>(exitingCar) = true;
                    get<2>(exitingCar) += drivenDistance;
                }
                else {
                    get<3>(exitingCar) = true;
                    get<4>(exitingCar) += drivenDistance;
                }

                activeEntries.erase(carRegistry);
            }
            else {
                cerr << errorLine(get<0>(lastOccurence->second)) << "\n";
                activeEntries[carRegistry] = transit;
            }
        }
        else {
            activeEntries[carRegistry] = transit;
        }
    }

    void processQuery (Query query) {
        string &queriedName = get<1>(query);
        if (get<1>(query) == "") {
            for (auto car : cars)
                cout << carDescription(car.second) << "\n";
            for (auto road : roads)
                cout << roadDescription(road.second) << "\n";
        }
        else {
            auto queriedCar = cars.find(queriedName);
            if (queriedCar != cars.end()) {
                cout << carDescription(queriedCar->second) << "\n";
            }
            if (isProperRoadName(queriedName)) {
                auto queriedRoad = roads.find(encodeRoadName(queriedName));
                if (queriedRoad != roads.end()) {
                    cout << roadDescription(queriedRoad->second) << "\n";
                }

            }
        }
    }

    void processLine (InputLine& line) {
        if (get<0>(line) == "") return;

        smatch match;
        if (regex_match(get<0>(line), match, transitLine)) {
            long long transitDistance = 10 * stoi(match[3]) + (match[3].str().back() - '0');
            Transit transit = {line, match[1], match[2], transitDistance};
            manageTransit(transit);
            return;
        }
        if (regex_match(get<0>(line), match, queryLine)) {
            Query query = {line, match[1].str()};
            processQuery(query);
            return;
        }
        cerr << errorLine(line) << "\n";
        return;
    }
}



int main() {
    iostream::sync_with_stdio(0);
    cin.tie(0);
    int lineNumber = 1;
    while(!cin.eof()) {
        string input;
        getline(cin, input);
        InputLine line = {input, lineNumber++};
        processLine(line);
        if (cin.eof()) return 0;

    }
}