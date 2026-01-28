# Snap7Helper Framework for C++

C++ wrapper for the **Snap7** library. Designed for **Siemens S7-1200/1500** PLC integration. 

This framework eliminates the complexity of byte-swapping (Endianness) and manual offset calculation by providing a human-readable string-based interface.

---
## IMPORTANT WARNING & DISCLAIMER

> **THIS FRAMEWORK IS PROVIDED FOR TESTING, EDUCATIONAL, AND NON-PRODUCTION USE ONLY.**

**Snap7Helper is NOT intended for real-life or production industrial applications.**  
It runs on a non-real-time PC operating system and communicates over standard TCP/IP, making it unsuitable for any system where failure could result in injury, equipment damage, production loss, or environmental harm.

### DO NOT use this framework for:
- Machine safety functions  
- Emergency stop logic (E-Stops)  
- Personnel protection systems  
- Certified, regulated, or safety-critical industrial environments  

### Liability Disclaimer
The **author assumes NO responsibility or liability** for any damage, malfunction, data loss, downtime, injury, or legal consequences resulting from the use or misuse of this framework.

By using **Snap7Helper**, **you acknowledge that you do so entirely at your own risk**.

## Key Features
---
* **String Address Parsing**: Use addresses like `%I0.0`, `%MW10`, or `%DB1.DBD4` directly.
* **Automatic Endianness Correction**: Handles the Big-Endian (Siemens) vs. Little-Endian (PC) conversion internally.
* **No Linker Hassle**: Core conversion functions are defined inline within the helper to avoid "Undefined External Symbol" errors.
* **Factory IO Ready**: Perfect for connecting C++ control algorithms to 3D plant simulations.

---

## Project Structure

Your project should be organized as follows:
```
project_root/
├── CMakeLists.txt
├── main.cpp
├── Snap7Helper.h      <-- The Framework
├── snap7.cpp          <-- Snap7 Source
├── snap7.h            <-- Snap7 Header
├── snap7.dll          <-- Shared Library
└── libs/
    └── snap7.lib      <-- Static Library
```
API Reference
```
bool Connect(std::string ip, int rack = 0, int slot = 1);
void Disconnect();

Data Access Methods
Method	Address Format	PLC Type	C++ Type
GetBool / WriteBool	%I0.0, %Q0.5, %M1.0	BOOL	bool
GetInt / WriteInt	%MW10, %DB1.DBW20	INT	int16_t
GetReal / WriteReal	%MD30, %DB1.DBD40	REAL	float
```
Usage Example in C++
```
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
```

PLC Configuration (TIA Portal)

To allow this library to communicate with your PLC, you must follow these steps:

    Enable PUT/GET: Go to Device Configuration > Select CPU > Properties > Protection & Security > Connection Mechanisms and check "Permit access with PUT/GET communication".

    Disable Optimized Access: Right-click your Data Block (DB) > Properties > Attributes and uncheck "Optimized block access".

    Network: Ensure your PC is on the same subnet as the PLC (e.g., PC: 192.168.60.10, PLC: 192.168.60.201).

Addressing Notes

    Alignment: Always use even byte numbers for Integers (%MW0, %MW2, %MW4) to avoid memory overlapping.

    Real/Float: Real values occupy 4 bytes. If you use %MD10, the next available address is %MD14.

    Inputs: When using Factory IO, ensure the driver is set to "Internal Bits" if you intend to write to %I addresses from your C++ code.

License

This framework is provided "as-is". It requires the Snap7 Library to function.