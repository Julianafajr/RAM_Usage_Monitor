#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

struct CPUStats {
    long long user = 0, nice = 0, system = 0, idle = 0,
              iowait = 0, irq = 0, softirq = 0;

    long long total() const {
        return user + nice + system + idle + iowait + irq + softirq;
    }

    long long active() const {
        return user + nice + system + irq + softirq;
    }
};

CPUStats readCPUStats() {
    ifstream file("/proc/stat");
    CPUStats stats;
    string label;

    if (file.is_open()) {
        file >> label >> stats.user >> stats.nice >> stats.system >> stats.idle
             >> stats.iowait >> stats.irq >> stats.softirq;
        file.close();
    }

    return stats;
}

long getMemValue(const string& label, const string& line) {
    long value = 0;
    if (line.find(label) == 0) {
        sscanf(line.c_str(), (label + " %ld kB").c_str(), &value);
    }
    return value;
}

void drawBar(double percent, int width = 40) {
    int filled = percent * width;
    cout << "[";
    for (int i = 0; i < width; ++i) {
        cout << (i < filled ? "#" : "-");
    }
    cout << "] " << fixed << setprecision(1) << (percent * 100) << "%" << endl;
}

void printStats(long memTotal, long memUsed, long swapTotal, long swapUsed, double cpuUsage) {
    system("clear");

    double memPercent = double(memUsed) / memTotal;
    double swapPercent = swapTotal > 0 ? double(swapUsed) / swapTotal : 0;

    cout << "┌──────────────────────────────────────────────────────┐" << endl;
    cout << "│                🧠 SYSTEM RESOURCE MONITOR            │" << endl;
    cout << "├──────────────────────────────────────────────────────┤" << endl;
    cout << "│ Memory Usage:                                        │" << endl;
    cout << "│ Total: " << setw(5) << memTotal / 1024 << " MB  Used: " << setw(5) << memUsed / 1024 << " MB               │" << endl;
    cout << "│ "; drawBar(memPercent);
    cout << "├──────────────────────────────────────────────────────┤" << endl;
    cout << "│ Swap Usage:                                          │" << endl;
    cout << "│ Total: " << setw(5) << swapTotal / 1024 << " MB  Used: " << setw(5) << swapUsed / 1024 << " MB               │" << endl;
    cout << "│ "; drawBar(swapPercent);
    cout << "├──────────────────────────────────────────────────────┤" << endl;
    cout << "│ CPU Usage:                                           │" << endl;
    cout << "│ "; drawBar(cpuUsage);
    cout << "└──────────────────────────────────────────────────────┘" << endl;

    cout << "Refreshing every 1 second... Press Ctrl+C to exit." << endl;
}

int main() {
    CPUStats prevCPU = readCPUStats();

    while (true) {
        this_thread::sleep_for(chrono::seconds(1));

        // RAM info
        ifstream meminfo("/proc/meminfo");
        string line;
        long memTotal = 0, memFree = 0, buffers = 0, cached = 0;
        long swapTotal = 0, swapFree = 0;

        while (getline(meminfo, line)) {
            memTotal  += getMemValue("MemTotal:", line);
            memFree   += getMemValue("MemFree:", line);
            buffers   += getMemValue("Buffers:", line);
            cached    += getMemValue("Cached:", line);
            swapTotal += getMemValue("
