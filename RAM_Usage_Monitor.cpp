#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

// Function to parse a value from /proc/meminfo
long getMemValue(const string& label, const string& line) {
    long value = 0;
    if (line.find(label) == 0) {
        sscanf(line.c_str(), (label + " %ld kB").c_str(), &value);
    }
    return value;
}

// Draw a simple ASCII bar
void drawBar(double percent) {
    int width = 50;
    int filled = percent * width;

    cout << "[";
    for (int i = 0; i < width; ++i) {
        if (i < filled) cout << "#";
        else cout << "-";
    }
    cout << "] " << int(percent * 100) << "%" << endl;
}

int main() {
    while (true) {
        ifstream meminfo("/proc/meminfo");
        string line;
        long memTotal = 0, memFree = 0, buffers = 0, cached = 0;
        long swapTotal = 0, swapFree = 0;

        if (!meminfo.is_open()) {
            cerr << "Error: Could not open /proc/meminfo" << endl;
            return 1;
        }

        while (getline(meminfo, line)) {
            memTotal  += getMemValue("MemTotal:", line);
            memFree   += getMemValue("MemFree:", line);
            buffers   += getMemValue("Buffers:", line);
            cached    += getMemValue("Cached:", line);
            swapTotal += getMemValue("SwapTotal:", line);
            swapFree  += getMemValue("SwapFree:", line);
        }

        meminfo.close();

        long memUsed = memTotal - memFree - buffers - cached;
        long swapUsed = swapTotal - swapFree;

        double memPercent = double(memUsed) / memTotal;
        double swapPercent = swapTotal ? double(swapUsed) / swapTotal : 0;

        system("clear"); // clear terminal for refreshing effect

        cout << "=== 🧠 RAM USAGE MONITOR (Live) ===" << endl;
        cout << "Total Memory: " << memTotal / 1024 << " MB" << endl;
        cout << "Used Memory : " << memUsed / 1024 << " MB" << endl;
        cout << "Free Memory : " << memFree / 1024 << " MB" << endl;
        cout << "Buffers     : " << buffers / 1024 << " MB" << endl;
        cout << "Cached      : " << cached / 1024 << " MB" << endl;
        cout << "Usage Bar   : ";
        drawBar(memPercent);

        cout << "\nSwap Total  : " << swapTotal / 1024 << " MB" << endl;
        cout << "Swap Used   : " << swapUsed / 1024 << " MB" << endl;
        cout << "Swap Usage  : ";
        drawBar(swapPercent);

        cout << "\nUpdating every 2 seconds. Press Ctrl+C to quit." << endl;

        this_thread::sleep_for(chrono::seconds(2));
    }

    return 0;
}
