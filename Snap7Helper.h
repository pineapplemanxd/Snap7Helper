#ifndef SNAP7_HELPER_H
#define SNAP7_HELPER_H

#include <iostream>
#include <string>
#include <regex>
#include <thread>
#include <chrono>
#include <stdint.h>
#include "snap7.h"

//extern "C" {
//    void S7API S7_SetRealAt(void* Buffer, int Pos, float Value);
//    float S7API S7_GetRealAt(void* Buffer, int Pos);
//    void S7API S7_SetIntAt(void* Buffer, int Pos, int16_t Value);
//    int16_t S7API S7_GetIntAt(void* Buffer, int Pos);
//}
// --- linker fix ---
inline int16_t My_GetIntAt(void* Buffer, int Pos) {
    uint8_t* p = (uint8_t*)Buffer + Pos;
    return (int16_t)((p[0] << 8) | p[1]);
}

inline float My_GetRealAt(void* Buffer, int Pos) {
    union { float f; uint8_t b[4]; } u;
    uint8_t* p = (uint8_t*)Buffer + Pos;
    u.b[3] = p[0]; u.b[2] = p[1]; u.b[1] = p[2]; u.b[0] = p[3];
    return u.f;
}
inline void My_SetIntAt(void* Buffer, int Pos, int16_t Value) {
    uint8_t* p = (uint8_t*)Buffer + Pos;
    p[0] = (uint8_t)(Value >> 8);
    p[1] = (uint8_t)(Value & 0xFF);
}

inline int32_t My_GetDIntAt(void* Buffer, int Pos) {
    uint8_t* p = (uint8_t*)Buffer + Pos;
    return (int32_t)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

inline void My_SetDIntAt(void* Buffer, int Pos, int32_t Value) {
    uint8_t* p = (uint8_t*)Buffer + Pos;
    p[0] = (uint8_t)((Value >> 24) & 0xFF);
    p[1] = (uint8_t)((Value >> 16) & 0xFF);
    p[2] = (uint8_t)((Value >> 8) & 0xFF);
    p[3] = (uint8_t)(Value & 0xFF);
}

inline void My_SetRealAt(void* Buffer, int Pos, float Value) {
    union {
        float f;
        uint8_t b[4];
    } u;
    u.f = Value;
    uint8_t* p = (uint8_t*)Buffer + Pos;
    p[0] = u.b[3];
    p[1] = u.b[2];
    p[2] = u.b[1];
    p[3] = u.b[0];
}
class Snap7Helper {
private:
    TS7Client* Client;
    bool ParseWordAddress(std::string addr, int& area, int& db, int& byt) {
        std::regex re_std("^%([IQM])([WD])(\\d+)$", std::regex_constants::icase);
        std::regex re_db("^%DB(\\d+)\\.DB([WD])(\\d+)$", std::regex_constants::icase);
        std::smatch m;

        if (std::regex_match(addr, m, re_std)) {
            char t = std::toupper(m[1].str()[0]);
            byt = std::stoi(m[3].str());
            db = 0;
            if (t == 'I') area = S7AreaPE;
            else if (t == 'Q') area = S7AreaPA;
            else area = S7AreaMK;
            return true;
        }
        else if (std::regex_match(addr, m, re_db)) {
            area = S7AreaDB;
            db = std::stoi(m[1].str());
            byt = std::stoi(m[3].str());
            return true;
        }
        return false;
    }

	// pareser for bit addresses
    bool ParseBitAddress(std::string addr, int& area, int& db, int& byt, int& bit) {
        std::regex re_std("^%([IQM])(\\d+)\\.(\\d+)$", std::regex_constants::icase);
        std::regex re_db("^%DB(\\d+)\\.DBX(\\d+)\\.(\\d+)$", std::regex_constants::icase);
        std::smatch m;

        if (std::regex_match(addr, m, re_std)) {
            char t = std::toupper(m[1].str()[0]);
            byt = std::stoi(m[2].str());
            bit = std::stoi(m[3].str());
            db = 0;
            if (t == 'I') area = S7AreaPE;
            else if (t == 'Q') area = S7AreaPA;
            else area = S7AreaMK;
            return true;
        }
        else if (std::regex_match(addr, m, re_db)) {
            area = S7AreaDB;
            db = std::stoi(m[1].str());
            byt = std::stoi(m[2].str());
            bit = std::stoi(m[3].str());
            return true;
        }
        return false;
    }

public:
    Snap7Helper() { Client = new TS7Client(); }
    ~Snap7Helper() { Disconnect(); delete Client; }

    bool Connect(std::string ip, int rack = 0, int slot = 1) {
        int res = Client->ConnectTo(ip.c_str(), rack, slot);
        return res == 0;
    }

    void Disconnect() { if (Client->Connected()) Client->Disconnect(); }

    // --- BOOL (Bit) ---
    bool GetBool(std::string address) {
        int area, db, byt, bit;
        if (!ParseBitAddress(address, area, db, byt, bit)) return false;
        byte buf[1];
        Client->ReadArea(area, db, (byt * 8) + bit, 1, S7WLBit, buf);
        return buf[0] != 0;
    }

    bool WriteBool(std::string address, bool value) {
        int area, db, byt, bit;
        if (!ParseBitAddress(address, area, db, byt, bit)) return false;
        byte val = value ? 1 : 0;
        return Client->WriteArea(area, db, (byt * 8) + bit, 1, S7WLBit, &val) == 0;
    }

    // --- REAL (Float) ---
    float GetReal(std::string address) {
        int area, db, byt;
        if (!ParseWordAddress(address, area, db, byt)) return 0.0f;

        byte buffer[4];
        int res = Client->ReadArea(area, db, byt, 4, S7WLByte, buffer);

        if (res == 0) return ::My_GetRealAt(buffer, 0);
        return 0.0f;
    }

    bool WriteReal(std::string address, float value) {
        int area, db, byt;
        if (!ParseWordAddress(address, area, db, byt)) return false;

        byte buffer[4];
        My_SetRealAt(buffer, 0, value);

        int res = Client->WriteArea(area, db, byt, 4, S7WLByte, buffer);
        return res == 0;
    }

    // --- INT (16-bit) ---
    int16_t GetInt(std::string address) {
        int area, db, byt;
        if (!ParseWordAddress(address, area, db, byt)) return 0;

        byte buffer[2];
        int res = Client->ReadArea(area, db, byt, 2, S7WLByte, buffer);

        if (res == 0) return My_GetIntAt(buffer, 0); 
        return 0;
    }

    bool WriteInt(std::string address, int16_t value) {
        int area, db, byt;
        if (!ParseWordAddress(address, area, db, byt)) return false;

        byte buffer[2];
        My_SetIntAt(buffer, 0, value);

        int res = Client->WriteArea(area, db, byt, 2, S7WLByte, buffer);
        return res == 0;
    }
    // --- DINT (32-bit Integer) ---
    int32_t GetDInt(std::string address) {
        int area, db, byt;
        // Parse address (supports %MD10, %DB1.DBD10, etc.)
        if (!ParseWordAddress(address, area, db, byt)) return 0;

        byte buffer[4];
        // Read 4 bytes from the PLC
        int res = Client->ReadArea(area, db, byt, 4, S7WLByte, buffer);

        if (res == 0) return My_GetDIntAt(buffer, 0);
        return 0;
    }

    bool WriteDInt(std::string address, int32_t value) {
        int area, db, byt;
        if (!ParseWordAddress(address, area, db, byt)) return false;

        byte buffer[4];
        My_SetDIntAt(buffer, 0, value);
        int res = Client->WriteArea(area, db, byt, 4, S7WLByte, buffer);
        return res == 0;
    }
};

#endif
