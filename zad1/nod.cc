#include <iostream>
#include <string>
#include <map>
#include <regex>

using namespace std;

namespace{

// Some regexes for input validation

#define CAPTURE(re) "(" re ")"
#define CAR_NAME_RE "(?:[a-zA-Z0-9]{3,11})"
#define ROAD_NAME_RE "(?:[AS][1-9][0-9]{0,2})"
#define DISTANCE_RE "(?:[1-9][0-9]{0,7},[0-9]|0,[0-9])"

const regex carEntryRegex("^\\s*" CAPTURE(CAR_NAME_RE) "\\s+" CAPTURE(ROAD_NAME_RE) "\\s+" CAPTURE(DISTANCE_RE) "\\s*$");
const regex queryRegex("^\\s*" "\\?" "\\s*" CAPTURE(CAR_NAME_RE "|" ROAD_NAME_RE)"?" "\\s*$");
const regex roadNameRegex(ROAD_NAME_RE);



// Compound type definitions

// All distances are integers, stored in 100s of meters. We assume that program is used on planet Earth and that distances
// are generally smaller than Earth - Sun distance (which is about 148 000 000 km) (with exception of total distance of a road).

/// Combines raw input line with line number
using InputLine = tuple<string, int>;

/// Accumulates total distance travelled by car. Since total distance can be 0, and it can be printed or not depending on input data,
/// we also add flags indicating whether value should be printed on query. (Road order is A, S)
using Car = tuple<bool, int, bool, int>;

/// Accumulates total distance travelled on road. Also contains road type (A/S) for easier lookup during processing.
using Road = tuple<char, uint64_t>;

/// Type used as key in map storing roads info. Contains road number and its type. Order is important - since road number
/// is first element of the tuple, iterating over map will produce order desired on output.
using RoadName = tuple<int, char>;

/// Type used to store unpaired information about car entering a road.
/// Contains input line, unparsed road name and position on which car entered the road (in 100s of meters).
using EntryEvent = tuple<InputLine, string, int>;



// Global containers for collected data. Since the program is small and use of
// classes/structs is banned this is probably next best approach (especially since
// whole thing is encapsulated in anonymous namespace).

/// Stores information about cars that entered some road but hadn't left yet.
/// Car number plate -> entry event info
map<string, EntryEvent> entries;

/// Car number plate -> car info
map<string, Car> cars;

/// Road name -> road info
map<RoadName, Road> roads;



/// Fetches car and creates one if it hadn't existed.
Car& getCar(const string& carName){
    auto it = cars.find(carName);
    if(it != cars.end()){
        return it->second;
    } else {
        return cars[carName] = {false, 0, false, 0};
    }
}

/// Fetches road and creates one if it hadn't existed.
Road& getRoad(const string& roadName){
    RoadName index = {stoi(roadName.substr(1)), roadName[0]};
    auto it = roads.find(index);
    if(it != roads.end()){
        return it->second;
    } else {
        return roads[index] = {roadName[0], 0};
    }
}

// We use template here to avoid duplicating implementations for long long and int.
// One could create single function for long long but that would result in unnecessary casting
// when int is passed (it is possible that compiler would optimize it tho, we didn't analyze
// machine code)
template<typename T>
string toDecimal(T n){
    stringstream s;
    s << n/10 << "," << n%10;
    return s.str();
}

void printError(const InputLine& line){
    cerr << "Error in line " << get<1>(line) << ": " << get<0>(line) << endl;
}

void printCar(const pair<string, Car>& carMapEntry){
    bool A = get<0>(carMapEntry.second), S = get<2>(carMapEntry.second);
    int A_n = get<1>(carMapEntry.second), S_n = get<3>(carMapEntry.second);
    if(A || S){
        cout << carMapEntry.first << (A ? " A " + toDecimal(A_n): "") << (S ? " S " + toDecimal(S_n): "") << endl;
    }
}

void printRoad(const pair<RoadName, Road>& roadMapEntry){
    cout << get<1>(roadMapEntry.first) << get<0>(roadMapEntry.first) << " " << toDecimal(get<1>(roadMapEntry.second)) << endl;
}

void processEvent(const InputLine& line, const smatch& match){
    string carName = match[1];
    string roadName = match[2];
    int roadPoint = 10 * stoi(match[3]) + match[3].str().back() - '0';

    if(entries.find(carName) != entries.end()){
        auto& [entryLine, entryRoad, entryPoint] = entries[carName];

        if(entryRoad == roadName){
            Car& car = getCar(carName);
            Road& road = getRoad(roadName);
            int distance = abs(roadPoint - entryPoint);
            get<1>(road) += distance;
            if(get<0>(road) == 'A'){
                get<0>(car) = true;
                get<1>(car) += distance;
            } else {
                get<2>(car) = true;
                get<3>(car) += distance;
            }
            entries.erase(carName);
        } else {
            printError(entryLine);
            entries[carName] = {line, roadName, roadPoint};
        }
    } else {
        entries[carName] = {line, roadName, roadPoint};
    }
}

void processQuery(const smatch& match){
    string queryArg = match[1];

    if(queryArg.empty()){
        for(const auto& it: cars){
            printCar(it);
        }
        for(const auto& it: roads){
            printRoad(it);
        }
        return;
    }

    auto carIt = cars.find(queryArg);
    if(carIt != cars.end()){
        printCar(*carIt);
    }

    if(regex_match(queryArg, roadNameRegex)){
        auto roadIt = roads.find({stoi(queryArg.substr(1)), queryArg[0]});
        if(roadIt != roads.end()){
            printRoad(*roadIt);
        }
    }
}

void processLine(const InputLine& line){
    if(get<0>(line).empty()){
        return;
    }

    smatch match;
    if(regex_match(get<0>(line), match, carEntryRegex)){
        processEvent(line, match);
        return;
    }

    if(regex_match(get<0>(line), match, queryRegex)){
        processQuery(match);
        return;
    }

    printError(line);
}

}

int main(){
    int lineNumber = 1;
    while(!cin.eof()){
        string line;
        getline(cin, line);
        InputLine currLine = {line, lineNumber++};
        processLine(currLine);
    }
    return 0;
}
