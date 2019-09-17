//
//  StateManager.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/15/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "StateManager.hpp"
#include "Mapper.hpp"
#include "Log.hpp"
#include "Cpu.hpp"
#include "Ppu.hpp"

#include <unistd.h>
#include <sys/stat.h>

using namespace nes::system;

StateManager::StateManager(common::Cartridge &cart) : cart(cart) {
    string home = getenv("HOME");
    string rockEmuPath = home + "/.dsemu";
    statePath = rockEmuPath + "/states";
    batteryPath = rockEmuPath + "/saves";
    
    mkdir(rockEmuPath.c_str(), S_IXUSR | S_IWUSR | S_IRUSR);
    mkdir(statePath.c_str(), S_IXUSR | S_IWUSR | S_IRUSR);
    mkdir(batteryPath.c_str(), S_IXUSR | S_IWUSR | S_IRUSR);
}

void StateManager::saveState() {
    cpu->pause();
    usleep(500000);
    
    string stateFN = statePath + getFileName("state");
    
    std::ofstream out(stateFN, std::ios::binary);
    
    if (!out) {
        LOG << "Failed to save state" << endl;
    }
    
    cart.getMapper().saveState(out);
    cpu->saveState(out);
    ppu->saveState(out);
    
    cpu->resume();
    
    out.close();
}

void StateManager::loadState() {
    cpu->pause();
    usleep(500000);
    std::ifstream in(statePath + getFileName("state"), std::ios::binary);
    
    if (!in) {
        LOG << "Failed to load state" << endl;
    }
    
    cart.getMapper().loadState(in);
    cpu->loadState(in);
    ppu->loadState(in);
    
    in.close();
    
    cpu->resume();
}

string StateManager::getFileName(const string &ext) {
    auto lastSlash = cart.getFileName().rfind("/");
    auto fn = cart.getFileName().substr(lastSlash);
    return fn.substr(0, fn.rfind(".")) + "." + ext;
}

void StateManager::saveBattery() {
    string saveFN = batteryPath + getFileName("save");
    std::cout << "FN: " << saveFN << endl;
    std::ofstream out(saveFN, std::ios::binary);
    
    if (!out) {
        std::cout << "Failed" << endl;
        return;
    }
    
    out.write((char *) cart.getMapper().getPrgRam(), 0x2000);
    out.close();
}

void StateManager::loadBattery() {
    std::ifstream in(batteryPath + getFileName("save"), std::ios::binary);
    
    if (!in) {
        printf("FAILED TO LOAD BATTERY\r\n");
        return;
    }
    
    in.read((char *) cart.getMapper().getPrgRam(), 0x2000);
    in.close();
}
