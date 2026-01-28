#include "Snap7Helper.h"
#include <conio.h>
#include <thread>
#include <chrono>
int main() {
    Snap7Helper plc;
    plc.Connect("192.168.0.1");
	int booltest = plc.GetBool("%I1.1");
	int inttest = plc.GetInt("%MW1");
	float realtest = plc.GetReal("%QD4");
	std::cout << "Bool: " << booltest << ", Int: " << inttest << ", Real: " << realtest << std::endl;
    return 0;
}