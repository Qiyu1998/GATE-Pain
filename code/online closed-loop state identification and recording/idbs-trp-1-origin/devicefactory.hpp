#ifndef DEVICEFACTORY_H
#define DEVICEFACTORY_H

#include "stimdevice.hpp"
#include "alphalabsnr.h"
#include "neuroomega.h"
#include "aotest.h"

using namespace std;

/*
The NUM_STIM_STRATEGY must match the number of the items in StimStrategy.
The items in StimStrategy and StrategyList must match.
*/

#define NUM_STIM_DEVICE 3
enum StimDeviceList
{
    AlphaLabSNR = 0,
    NeuroOmega = 1,
    AOTest = 2,
};

static const string StimDeviceName[]{
    "AlphaLab",
    "NeuroOmega",
    "AOTest",
};

class DeviceFactory
{
public:
    static StimDevice *getAlphaOmegaDevice(StimDeviceList device){
        StimDevice *aoDevice;

        switch (device) {
        case AlphaLabSNR:
            aoDevice = new class AlphaLabSNR;
            break;
        case NeuroOmega:
            aoDevice = new class NeuroOmega;
            break;
        case AOTest:
            aoDevice = new class AOTest;
            break;
        }

        return aoDevice;
    }

    static bool isValid(const string deviceStr){
        for(int i=0;i<NUM_STIM_DEVICE;i++){
            if(!strcmp(deviceStr.c_str(),StimDeviceName[i].c_str())){
                return true;
            }
        }
        return false;
    }

    static StimDeviceList strToEnum(const string deviceStr){
        StimDeviceList device = StimDeviceList(0);
        for(int i=0;i<NUM_STIM_DEVICE;i++){
            if(!strcmp(deviceStr.c_str(),StimDeviceName[i].c_str())){
                 device = StimDeviceList(i);
            }
        }
        return device;
    }

    static string enumToStr(StimDeviceList deviceEnum){
        int idx = static_cast<StimDeviceList>(deviceEnum);
        return StimDeviceName[idx];
    }
};

#endif // DEVICEFACTORY_H
