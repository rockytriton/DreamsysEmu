//
//  Cartridge.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "Cartridge.hpp"
#include "Log.hpp"
#include "Mapper.hpp"
#include "StateManager.hpp"

#include <fstream>

using namespace nes::common;
using namespace nes::mapper;

void Cartridge::loadBattery() {
    stateManager.loadBattery();
}

void Cartridge::saveBattery() {
    stateManager.saveBattery();
}

bool Cartridge::load(const string &fileName) {
    this->fileName = fileName;
    
    std::ifstream romFile (fileName, std::ios::in | std::ios::binary);
    
    if (!romFile) {
        LOG << "Failed to open file: " << fileName << endl;
        return false;
    }
    
    if (!romFile.read((char *)&header, sizeof(NesHeader))) {
        LOG << "Invalid File: " << fileName << endl;
        return false;
    }
    
    header.type[3] = 0;
    
    if (string("NES") != header.type) {
        LOG << "Invalid header" << endl;
        return false;
    }
    
    LOG << "PRG Banks: " << std::hex << header.prgSize << endl << "CHR Banks: " << header.chrSize << endl;
    
    if (header.mainFlags.Bit.tr) {
        //skip trainer data...
        romFile.seekg(sizeof(NesHeader) + 0x200);
    }
    
    prgData = new Byte[0x4000 * header.prgSize];
    romFile.read((char *)prgData, 0x4000 * header.prgSize);
    
    LOG << "Read " << hex16 << (0x4000 * header.prgSize) << " bytes" << endl;
    
    if (header.chrSize) {
        chrData = new Byte[0x2000 * header.chrSize];
        romFile.read((char *)chrData, 0x2000 * header.chrSize);
    }
    
    mapper = MapperFactory::CreateMapper(*this);
    mapper->init();
    
    if (mapper == nullptr) {
        LOG << "Invalid Mapper: " << header.mainFlags.Bit.mt << endl;
        return false;
    }
    
    return true;
}
