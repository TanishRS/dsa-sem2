// =============================================================
//  Toll Plaza Traffic Analyzer
//  Beginner-level C++ (single file, terminal-based)
//  Compile: g++ -std=c++17 toll_plaza.cpp -o toll_plaza
//  Run:     ./toll_plaza      (Linux/Mac)
//           toll_plaza.exe    (Windows)
// =============================================================

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>     // for sort()
#include <unordered_map> // for hash map (O(1) lookups)
#include <chrono>        // for timing O(n) vs O(n^2)
#include <fstream>       // for exporting report to .txt
#include <ctime>         // for timestamp validation
#include <sstream>       // for parsing timestamps
#include <iomanip>       // for formatting output

using namespace std;

// =============================================================
//  CONSTANTS
// =============================================================
const int MAX_LANES = 12;          // toll plaza has at most 12 lanes
const int MIN_ID_LENGTH = 3;       // shortest plausible vehicle ID
const int MAX_ID_LENGTH = 15;      // longest plausible vehicle ID

// =============================================================
//  VehicleEntry STRUCT
//  Holds one vehicle record. Plain struct = beginner-friendly.
// =============================================================
struct VehicleEntry {
    string id;        // vehicle ID (e.g. "MH12AB1234")
    int    lane;      // lane number (1 .. MAX_LANES)
    long   timeEpoch; // time as epoch seconds (easier to compare than strings)
    string timeStr;   // human-readable time "YYYY-MM-DD HH:MM:SS"
    string type;      // CAR / TRUCK / MOTORCYCLE / BUS / VAN / UNKNOWN
};

// Global storage (kept simple for a beginner project).
// In a bigger project we'd pass this around instead of using a global.
vector<VehicleEntry> vehicleEntries;

// =============================================================
//  HELPER: convert "YYYY-MM-DD HH:MM:SS" to epoch seconds
//  Returns -1 if the string cannot be parsed.
//  Time complexity: O(1)  -- fixed-size string parsing.
// =============================================================
long parseTimeToEpoch(const string& s) {
    struct tm t = {};
    int y, mo, d, h, mi, se;
    char dash1, dash2, col1, col2;
    stringstream ss(s);
    ss >> y >> dash1 >> mo >> dash2 >> d >> h >> col1 >> mi >> col2 >> se;
    (void)dash1; (void)dash2; (void)col1; (void)col2;
    if (ss.fail()) return -1;
    t.tm_year = y - 1900;
    t.tm_mon  = mo - 1;
    t.tm_mday = d;
    t.tm_hour = h;
    t.tm_min  = mi;
    t.tm_sec  = se;
    return (long)mktime(&t);
}

// =============================================================
//  VALIDATION FUNCTIONS
//  (Mirrors the Validation Framework image:
//   ID -> Lane -> Time -> Type)
// =============================================================

// Time complexity: O(L) where L = length of id string. Effectively O(1).
bool isValidId(const string& id) {
    if (id.empty()) return false;
    if ((int)id.length() < MIN_ID_LENGTH) return false;
    if ((int)id.length() > MAX_ID_LENGTH) return false;
    // every char must be alphanumeric
    for (char c : id) {
        if (!isalnum((unsigned char)c)) return false;
    }
    return true;
}

// Time complexity: O(1)
bool isValidLane(int lane) {
    return (lane >= 1 && lane <= MAX_LANES);
}

// Time complexity: O(1) -- string parse + simple range check.
bool isValidTime(const string& timeStr, long& outEpoch) {
    long e = parseTimeToEpoch(timeStr);
    if (e < 0) return false;
    long now = (long)time(nullptr);
    // not in the future, not before year 2000
    long year2000 = 946684800L;
    if (e > now) return false;
    if (e < year2000) return false;
    outEpoch = e;
    return true;
}

// Time complexity: O(1)
bool isValidType(const string& type) {
    return (type == "CAR" || type == "TRUCK" || type == "MOTORCYCLE" ||
            type == "BUS" || type == "VAN"   || type == "UNKNOWN");
}

// =============================================================
//  REQUIREMENT 1: Record a vehicle entry
//  Time complexity: O(n) worst case (because of duplicate-in-window
//                    check). Without that check it would be O(1) amortized.
// =============================================================
bool recordVehicleEntry(const string& id, int lane,
                        const string& timeStr, const string& type) {
    // ---- run validations in the order shown in the framework image ----
    if (!isValidId(id)) {
        cout << "  [REJECTED] Invalid vehicle ID.\n";
        return false;
    }
    if (!isValidLane(lane)) {
        cout << "  [REJECTED] Invalid lane (must be 1.." << MAX_LANES << ").\n";
        return false;
    }
    long epoch = 0;
    if (!isValidTime(timeStr, epoch)) {
        cout << "  [REJECTED] Invalid timestamp (format YYYY-MM-DD HH:MM:SS, "
             << "not future, not before 2000).\n";
        return false;
    }
    if (!isValidType(type)) {
        cout << "  [REJECTED] Invalid type. Use CAR/TRUCK/MOTORCYCLE/BUS/VAN/UNKNOWN.\n";
        return false;
    }

    // append -- O(1) amortized for vector
    VehicleEntry e{ id, lane, epoch, timeStr, type };
    vehicleEntries.push_back(e);
    return true;
}

// =============================================================
//  REQUIREMENT 2: Find busiest lane -- THREE different algorithms
//  We deliberately pick algorithms with DIFFERENT complexities so the
//  Big-O table actually has something to compare.
// =============================================================

// ---- Algorithm A: brute force ----
// For every lane number, scan the whole list and count.
// Outer loop = MAX_LANES (constant), but we treat it as a separate factor
// to make the nested-loop pattern visible.
// Time complexity: O(n * MAX_LANES) -> if we treated MAX_LANES as variable
//                  it would be O(n*k). For teaching purposes: O(n^2)-style
//                  nested traversal.
int findBusiestLane_BruteForce() {
    if (vehicleEntries.empty()) return -1;
    int bestLane  = -1;
    int bestCount = -1;
    for (int lane = 1; lane <= MAX_LANES; lane++) {
        int count = 0;
        for (size_t i = 0; i < vehicleEntries.size(); i++) {
            if (vehicleEntries[i].lane == lane) count++;
        }
        if (count > bestCount) {
            bestCount = count;
            bestLane  = lane;
        }
    }
    return bestLane;
}

// ---- Algorithm B: sort then count consecutive runs ----
// Copy lanes into a vector, sort it, walk through counting runs.
// Time complexity: O(n log n) due to sort.
int findBusiestLane_SortBased() {
    if (vehicleEntries.empty()) return -1;
    vector<int> lanes;
    lanes.reserve(vehicleEntries.size());
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        lanes.push_back(vehicleEntries[i].lane);
    }
    sort(lanes.begin(), lanes.end()); // O(n log n)

    int bestLane  = lanes[0];
    int bestCount = 1;
    int curLane   = lanes[0];
    int curCount  = 1;
    for (size_t i = 1; i < lanes.size(); i++) {
        if (lanes[i] == curLane) {
            curCount++;
        } else {
            curLane  = lanes[i];
            curCount = 1;
        }
        if (curCount > bestCount) {
            bestCount = curCount;
            bestLane  = curLane;
        }
    }
    return bestLane;
}

// ---- Algorithm C: count-array (single pass) ----
// One pass to fill a small fixed-size counter array, one pass to find max.
// Time complexity: O(n)
int findBusiestLane_CountArray() {
    if (vehicleEntries.empty()) return -1;
    int counts[MAX_LANES + 1] = {0}; // index 0 unused, lanes 1..MAX_LANES
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        int ln = vehicleEntries[i].lane;
        if (ln >= 1 && ln <= MAX_LANES) counts[ln]++;
    }
    int bestLane  = 1;
    int bestCount = counts[1];
    for (int lane = 2; lane <= MAX_LANES; lane++) {
        if (counts[lane] > bestCount) {
            bestCount = counts[lane];
            bestLane  = lane;
        }
    }
    return bestLane;
}

// =============================================================
//  REQUIREMENT 3: Find peak hour -- O(n) scan vs O(n^2) nested
//  "Peak hour" = the hour-of-day (0..23) with the most entries.
// =============================================================

// Helper: extract hour-of-day from epoch. O(1).
int hourOfDay(long epoch) {
    time_t t = (time_t)epoch;
    struct tm* lt = localtime(&t);
    return lt->tm_hour;
}

// ---- Fast version: single pass + 24-slot counter ----
// Time complexity: O(n)
int findPeakHour_Fast() {
    if (vehicleEntries.empty()) return -1;
    int hourCounts[24] = {0};
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        int h = hourOfDay(vehicleEntries[i].timeEpoch);
        if (h >= 0 && h < 24) hourCounts[h]++;
    }
    int bestHour  = 0;
    int bestCount = hourCounts[0];
    for (int h = 1; h < 24; h++) {
        if (hourCounts[h] > bestCount) {
            bestCount = hourCounts[h];
            bestHour  = h;
        }
    }
    return bestHour;
}

// ---- Slow version: nested loops, no counter array ----
// For every entry, scan the whole list to count entries with the same hour.
// Time complexity: O(n^2)
int findPeakHour_Slow() {
    if (vehicleEntries.empty()) return -1;
    int bestHour  = -1;
    int bestCount = -1;
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        int hi = hourOfDay(vehicleEntries[i].timeEpoch);
        int count = 0;
        for (size_t j = 0; j < vehicleEntries.size(); j++) {
            int hj = hourOfDay(vehicleEntries[j].timeEpoch);
            if (hi == hj) count++;
        }
        if (count > bestCount) {
            bestCount = count;
            bestHour  = hi;
        }
    }
    return bestHour;
}

// =============================================================
//  REQUIREMENT 5: Count vehicles by type -- O(n) traversal
// =============================================================
// Time complexity: O(n)
void countVehiclesByType() {
    unordered_map<string,int> counter;
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        counter[vehicleEntries[i].type]++;
    }
    cout << "\n  Vehicle counts by type:\n";
    if (counter.empty()) { cout << "  (no data)\n"; return; }
    cout << "  -----------------------------\n";
    for (auto& p : counter) {
        cout << "    " << left << setw(12) << p.first
             << " : " << p.second << "\n";
    }
    cout << "  -----------------------------\n";
}

// =============================================================
//  REQUIREMENT 6: Sort entries by time -- O(n log n)
//  Uses std::sort (introsort). We sort a copy so we don't disturb
//  the original insertion order used by other features.
// =============================================================
// Time complexity: O(n log n)
vector<VehicleEntry> sortEntriesByTime() {
    vector<VehicleEntry> copy = vehicleEntries; // O(n) copy
    sort(copy.begin(), copy.end(),
         [](const VehicleEntry& a, const VehicleEntry& b) {
             return a.timeEpoch < b.timeEpoch;
         });
    return copy;
}

// =============================================================
//  THREE SEARCH STRATEGIES for vehicle ID
//  (referenced by the problem statement: "compare three different
//   search strategies for a vehicle ID")
// =============================================================

// ---- Strategy 1: Linear search ----
// Time complexity: O(n)
int searchById_Linear(const string& id) {
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        if (vehicleEntries[i].id == id) return (int)i;
    }
    return -1;
}

// ---- Strategy 2: Binary search on a sorted-by-ID copy ----
// Sorting cost is O(n log n) but we count the SEARCH cost, which is O(log n).
// Note: building/maintaining the sorted copy is the trade-off.
// Time complexity (search step only): O(log n)
int searchById_Binary(const vector<VehicleEntry>& sortedById, const string& id) {
    int lo = 0, hi = (int)sortedById.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (sortedById[mid].id == id) return mid;
        if (sortedById[mid].id < id)  lo = mid + 1;
        else                          hi = mid - 1;
    }
    return -1;
}

// ---- Strategy 3: Hash map ----
// Build cost O(n), lookup O(1) average.
// Time complexity (lookup): O(1) average, O(n) worst case.
int searchById_Hash(const unordered_map<string,int>& idIndex, const string& id) {
    auto it = idIndex.find(id);
    if (it == idIndex.end()) return -1;
    return it->second;
}

// =============================================================
//  REQUIREMENT 4: Big-O comparison table for ALL functions
// =============================================================
void printBigOTable() {
    cout << "\n  +----------------------------------------+----------------+\n";
    cout << "  | Function                                | Time Complexity|\n";
    cout << "  +----------------------------------------+----------------+\n";
    cout << "  | recordVehicleEntry                      | O(1) amortized |\n";
    cout << "  | findBusiestLane_BruteForce              | O(n * k)       |\n";
    cout << "  | findBusiestLane_SortBased               | O(n log n)     |\n";
    cout << "  | findBusiestLane_CountArray              | O(n)           |\n";
    cout << "  | findPeakHour_Fast                       | O(n)           |\n";
    cout << "  | findPeakHour_Slow                       | O(n^2)         |\n";
    cout << "  | countVehiclesByType                     | O(n)           |\n";
    cout << "  | sortEntriesByTime                       | O(n log n)     |\n";
    cout << "  | searchById_Linear                       | O(n)           |\n";
    cout << "  | searchById_Binary (search step)         | O(log n)       |\n";
    cout << "  | searchById_Hash   (lookup)              | O(1) average   |\n";
    cout << "  | searchByTimeRange (custom feature)      | O(n)           |\n";
    cout << "  | exportReportToFile (custom feature)     | O(n)           |\n";
    cout << "  +----------------------------------------+----------------+\n";
    cout << "  Note: k = MAX_LANES (constant = " << MAX_LANES << ")\n";
}

// =============================================================
//  CUSTOM FEATURE 1: Search vehicles by time range
//  Returns all entries whose timestamp lies in [startStr, endStr].
//  Time complexity: O(n)
// =============================================================
void searchByTimeRange(const string& startStr, const string& endStr) {
    long startE = parseTimeToEpoch(startStr);
    long endE   = parseTimeToEpoch(endStr);
    if (startE < 0 || endE < 0) {
        cout << "  Invalid time format. Use YYYY-MM-DD HH:MM:SS\n";
        return;
    }
    if (startE > endE) {
        cout << "  Start time must be before end time.\n";
        return;
    }
    int found = 0;
    cout << "\n  Vehicles between " << startStr << " and " << endStr << ":\n";
    cout << "  -----------------------------------------------------------\n";
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        long e = vehicleEntries[i].timeEpoch;
        if (e >= startE && e <= endE) {
            cout << "    " << vehicleEntries[i].id
                 << "  Lane=" << vehicleEntries[i].lane
                 << "  " << vehicleEntries[i].timeStr
                 << "  " << vehicleEntries[i].type << "\n";
            found++;
        }
    }
    cout << "  -----------------------------------------------------------\n";
    cout << "  Total found: " << found << "\n";
}

// =============================================================
//  CUSTOM FEATURE 2: Export full report to .txt file
//  Time complexity: O(n)
// =============================================================
void exportReportToFile(const string& filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cout << "  Could not open file for writing.\n";
        return;
    }
    out << "=========================================\n";
    out << "  TOLL PLAZA TRAFFIC REPORT\n";
    out << "=========================================\n\n";
    out << "Total entries: " << vehicleEntries.size() << "\n\n";

    // raw entries
    out << "--- All Entries ---\n";
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        out << (i+1) << ". "
            << vehicleEntries[i].id   << " | "
            << "Lane=" << vehicleEntries[i].lane << " | "
            << vehicleEntries[i].timeStr << " | "
            << vehicleEntries[i].type << "\n";
    }

    // summaries
    out << "\n--- Busiest Lane ---\n";
    out << "Brute force : Lane " << findBusiestLane_BruteForce() << "\n";
    out << "Sort-based  : Lane " << findBusiestLane_SortBased()  << "\n";
    out << "Count-array : Lane " << findBusiestLane_CountArray() << "\n";

    out << "\n--- Peak Hour ---\n";
    out << "Fast O(n)   : " << findPeakHour_Fast() << ":00\n";
    out << "Slow O(n^2) : " << findPeakHour_Slow() << ":00\n";

    out << "\n--- Vehicle Type Counts ---\n";
    unordered_map<string,int> counter;
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        counter[vehicleEntries[i].type]++;
    }
    for (auto& p : counter) {
        out << p.first << " : " << p.second << "\n";
    }

    out.close();
    cout << "  Report exported to: " << filename << "\n";
}

// =============================================================
//  HELPER: print all entries
// =============================================================
void printAllEntries() {
    if (vehicleEntries.empty()) {
        cout << "  (no entries yet)\n";
        return;
    }
    cout << "\n  All recorded entries (" << vehicleEntries.size() << "):\n";
    cout << "  ---------------------------------------------------------------\n";
    cout << "  #   ID              Lane  Time                   Type\n";
    cout << "  ---------------------------------------------------------------\n";
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        cout << "  " << left << setw(3) << (i+1) << " "
             << setw(15) << vehicleEntries[i].id << " "
             << setw(5)  << vehicleEntries[i].lane << " "
             << setw(22) << vehicleEntries[i].timeStr << " "
             << vehicleEntries[i].type << "\n";
    }
    cout << "  ---------------------------------------------------------------\n";
}

// =============================================================
//  DEMO DATA LOADER
//  Loads ~1200 synthetic entries so the O(n) vs O(n^2) timing
//  comparison actually shows a measurable difference.
// =============================================================
void loadDemoDataset() {
    // a baseline date: 2024-05-08 00:00:00
    long base = parseTimeToEpoch("2024-05-08 00:00:00");
    if (base < 0) base = 1715126400L; // fallback

    string types[] = {"CAR","TRUCK","MOTORCYCLE","BUS","VAN","UNKNOWN"};
    int    typeWeight[] = {60, 15, 10, 5, 8, 2}; // rough realistic mix
    int totalWeight = 0;
    for (int w : typeWeight) totalWeight += w;

    int loaded = 0;
    for (int i = 0; i < 1200; i++) {
        // pseudo-random but deterministic
        int laneVal = (i * 7) % MAX_LANES + 1;
        long offset = ((i * 53) % (12 * 3600)); // spread within first 12 hours
        // bias toward hour 8..10 to create a clear peak
        if (i % 3 == 0) offset = 8*3600 + (i % 7200);

        long epoch = base + offset;
        time_t tt = (time_t)epoch;
        struct tm* lt = localtime(&tt);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);

        // pick type by weighted slot
        int r = (i * 17) % totalWeight;
        string chosen = "CAR";
        int acc = 0;
        for (int t = 0; t < 6; t++) {
            acc += typeWeight[t];
            if (r < acc) { chosen = types[t]; break; }
        }

        // build id
        char idbuf[16];
        snprintf(idbuf, sizeof(idbuf), "DEMO%05d", i);

        VehicleEntry e{ string(idbuf), laneVal, epoch, string(buf), chosen };
        vehicleEntries.push_back(e);
        loaded++;
    }
    cout << "  Loaded " << loaded << " demo entries.\n";
}

// =============================================================
//  MENU HANDLERS
// =============================================================
void handleAddEntry() {
    string id, timeStr, type;
    int lane;
    cout << "\n  Enter vehicle ID (alphanumeric, 3-15 chars): ";
    cin >> id;
    cout << "  Enter lane number (1-" << MAX_LANES << "): ";
    cin >> lane;
    cin.ignore(); // flush newline before getline
    cout << "  Enter timestamp (YYYY-MM-DD HH:MM:SS): ";
    getline(cin, timeStr);
    cout << "  Enter type (CAR/TRUCK/MOTORCYCLE/BUS/VAN/UNKNOWN): ";
    cin >> type;

    if (recordVehicleEntry(id, lane, timeStr, type)) {
        cout << "  [OK] Entry recorded. Total entries = "
             << vehicleEntries.size() << "\n";
    }
}

void handleBusiestLaneCompare() {
    if (vehicleEntries.empty()) {
        cout << "  No data. Add entries or load demo dataset first.\n";
        return;
    }
    cout << "\n  Busiest lane (3 algorithms):\n";
    cout << "    Brute force  -> Lane " << findBusiestLane_BruteForce() << "\n";
    cout << "    Sort-based   -> Lane " << findBusiestLane_SortBased()  << "\n";
    cout << "    Count-array  -> Lane " << findBusiestLane_CountArray() << "\n";
    cout << "  All three should agree.\n";
}

void handlePeakHourTimingCompare() {
    if (vehicleEntries.empty()) {
        cout << "  No data. Add entries or load demo dataset first.\n";
        return;
    }
    using namespace std::chrono;

    auto t1 = high_resolution_clock::now();
    int fast = findPeakHour_Fast();
    auto t2 = high_resolution_clock::now();

    auto t3 = high_resolution_clock::now();
    int slow = findPeakHour_Slow();
    auto t4 = high_resolution_clock::now();

    long long fastUs = duration_cast<microseconds>(t2 - t1).count();
    long long slowUs = duration_cast<microseconds>(t4 - t3).count();

    cout << "\n  Peak-hour comparison (n = " << vehicleEntries.size() << "):\n";
    cout << "    O(n)   fast result : " << fast << ":00   ("
         << fastUs << " microseconds)\n";
    cout << "    O(n^2) slow result : " << slow << ":00   ("
         << slowUs << " microseconds)\n";
    if (vehicleEntries.size() < 100) {
        cout << "  Note: with very few entries the difference may look small.\n"
             << "  Load the demo dataset (option 9) to see a clearer gap.\n";
    }
}

void handleSearchById() {
    if (vehicleEntries.empty()) {
        cout << "  No data.\n"; return;
    }
    string id;
    cout << "\n  Enter vehicle ID to search: ";
    cin >> id;

    using namespace std::chrono;

    // Linear
    auto a1 = high_resolution_clock::now();
    int r1 = searchById_Linear(id);
    auto a2 = high_resolution_clock::now();

    // Binary (need sorted copy)
    vector<VehicleEntry> sortedById = vehicleEntries;
    sort(sortedById.begin(), sortedById.end(),
         [](const VehicleEntry& a, const VehicleEntry& b){ return a.id < b.id; });
    auto b1 = high_resolution_clock::now();
    int r2 = searchById_Binary(sortedById, id);
    auto b2 = high_resolution_clock::now();

    // Hash
    unordered_map<string,int> idIndex;
    for (size_t i = 0; i < vehicleEntries.size(); i++) {
        idIndex[vehicleEntries[i].id] = (int)i;
    }
    auto c1 = high_resolution_clock::now();
    int r3 = searchById_Hash(idIndex, id);
    auto c2 = high_resolution_clock::now();

    cout << "\n  Search results for ID = " << id << "\n";
    cout << "    Linear O(n)        : "
         << (r1 >= 0 ? "FOUND at index " + to_string(r1) : "NOT FOUND")
         << "   (" << duration_cast<microseconds>(a2-a1).count() << " us)\n";
    cout << "    Binary O(log n)    : "
         << (r2 >= 0 ? "FOUND in sorted copy at " + to_string(r2) : "NOT FOUND")
         << "   (" << duration_cast<microseconds>(b2-b1).count() << " us)\n";
    cout << "    Hash O(1) avg      : "
         << (r3 >= 0 ? "FOUND at index " + to_string(r3) : "NOT FOUND")
         << "   (" << duration_cast<microseconds>(c2-c1).count() << " us)\n";
}

void handleSortByTime() {
    if (vehicleEntries.empty()) {
        cout << "  No data.\n"; return;
    }
    vector<VehicleEntry> sorted = sortEntriesByTime();
    cout << "\n  Entries sorted by time (oldest first):\n";
    cout << "  ---------------------------------------------------------------\n";
    for (size_t i = 0; i < sorted.size() && i < 20; i++) {
        cout << "    " << sorted[i].timeStr << "  "
             << sorted[i].id << "  Lane=" << sorted[i].lane
             << "  " << sorted[i].type << "\n";
    }
    if (sorted.size() > 20) {
        cout << "    ... (" << (sorted.size() - 20) << " more rows hidden)\n";
    }
}

void handleSearchByTimeRange() {
    if (vehicleEntries.empty()) { cout << "  No data.\n"; return; }
    string s, e;
    cin.ignore();
    cout << "\n  Enter START time (YYYY-MM-DD HH:MM:SS): ";
    getline(cin, s);
    cout << "  Enter END   time (YYYY-MM-DD HH:MM:SS): ";
    getline(cin, e);
    searchByTimeRange(s, e);
}

void handleExport() {
    if (vehicleEntries.empty()) { cout << "  No data.\n"; return; }
    string fname;
    cout << "\n  Enter output filename (e.g. report.txt): ";
    cin >> fname;
    exportReportToFile(fname);
}

// =============================================================
//  MAIN MENU
// =============================================================
void printMenu() {
    cout << "\n=================================================\n";
    cout << "         TOLL PLAZA TRAFFIC ANALYZER\n";
    cout << "=================================================\n";
    cout << "  1.  Record a new vehicle entry\n";
    cout << "  2.  Show all entries\n";
    cout << "  3.  Find busiest lane (3 algorithms)\n";
    cout << "  4.  Find peak hour (O(n) vs O(n^2) timing)\n";
    cout << "  5.  Count vehicles by type\n";
    cout << "  6.  Sort entries by time\n";
    cout << "  7.  Search vehicle by ID (3 strategies)\n";
    cout << "  8.  Show Big-O comparison table\n";
    cout << "  9.  Load demo dataset (~1200 entries)\n";
    cout << " 10.  [Custom] Search vehicles by time range\n";
    cout << " 11.  [Custom] Export full report to .txt file\n";
    cout << "  0.  Exit\n";
    cout << "  Choice: ";
}

int main() {
    cout << "Welcome to the Toll Plaza Traffic Analyzer.\n";
    int choice;
    while (true) {
        printMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "  Please enter a number.\n";
            continue;
        }
        switch (choice) {
            case 1:  handleAddEntry();              break;
            case 2:  printAllEntries();             break;
            case 3:  handleBusiestLaneCompare();    break;
            case 4:  handlePeakHourTimingCompare(); break;
            case 5:  countVehiclesByType();         break;
            case 6:  handleSortByTime();            break;
            case 7:  handleSearchById();            break;
            case 8:  printBigOTable();              break;
            case 9:  loadDemoDataset();             break;
            case 10: handleSearchByTimeRange();     break;
            case 11: handleExport();                break;
            case 0:
                cout << "Goodbye.\n";
                return 0;
            default:
                cout << "  Unknown option.\n";
        }
    }
}
