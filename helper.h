#ifndef HELPER
#define HELPER
#include <iostream>
#include <chrono>
#include <thread>
class F_TRIG {
private:
    bool lastState; 

public:
    F_TRIG() : lastState(false) {}
    bool Check(bool CLK) {
        bool Q = !CLK && lastState;
        lastState = CLK;
        return Q;
    }
};
class R_TRIG {
private:
    bool lastState = false;

public:
    bool Check(bool CLK) {
        bool Q = CLK && !lastState;
        lastState = CLK;
        return Q;
    }
};
class CTU {
private:
    bool lastCU = false;

public:
    int CV = 0;
    bool Q = false;
    // Main Function
    // CU: Count Up signal
    // R:  Reset signal
    // PV: Preset Value (Target)
    void Check(bool CU, bool R, int PV) {
        if (R) {
            CV = 0;
        }
        else if (CU && !lastCU) {
            if (CV < 32767) { //overflow check
                CV++;
            }
        }
        lastCU = CU;
        Q = (CV >= PV);
    }
};
class CTD {
private:
    bool lastCD = false;

public:
    int CV = 0;  
    bool Q = false;

    // Main Function
    // CD: Count Down signal
    // LD: Load signal (Sets CV to PV)
    // PV: Preset Value (Start value)
    void Check(bool CD, bool LD, int PV) {
        if (LD) {
            CV = PV;
        }
        else if (CD && !lastCD) {
            if (CV > -32768) {
                CV--;
            }
        }

        lastCD = CD;
        Q = (CV <= 0);
    }
};
void sleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
void log(const std::string& message) {
	std::cout << message << std::endl;
}











#endif