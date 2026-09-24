#ifndef STRATEGYFACTORY_H
#define STRATEGYFACTORY_H
#include "stimstrategy.h"
#include <QStringList>

#include "nsdbs.h"
#include "thradbs.h"
#include "dualthresholds.h"
#include "amplmodulation.h"
#include "ns2dbs.h"

using namespace std;

/*
The NUM_STIM_STRATEGY must match the number of the items in StimStrategy.
The items in StimStrategy and StrategyList must match.
*/

#define NUM_STIM_STRATEGY 5

enum StimStrategyList{
    NSDBS = 0,
    NS2DBS = 1,
    THRaDBS = 2,
    DualThresholds = 3,
    AmplModulation = 4
};

static const string StimStrategyName[]{
    "NSDBS",
    "NS2DBS",
    "THRaDBS",
    "DualThresholds",
    "AmplModulation"
};

class StrategyFactory
{
public:
    static StimStrategy *getStimSuper(StimStrategyList stimStrategy, QString configFile){
        StimStrategy *stimSuper = nullptr;
        switch (stimStrategy) {
        case NSDBS:
            stimSuper = new class NSDBS(configFile);
            break;
        case NS2DBS:
            stimSuper = new class NS2DBS(configFile);
            break;
        case THRaDBS:
            stimSuper = new class THRaDBS(configFile);
            break;
        case DualThresholds:
            stimSuper = new class DualThresholds(configFile);
            break;
        case AmplModulation:
            stimSuper = new class AmplModulation(configFile);
            break;
        }
        return stimSuper;
    }

    static bool isValid(const string strategyStr){
        for(int i=0;i<NUM_STIM_STRATEGY;i++){
            if(strcmp(strategyStr.c_str(),StimStrategyName[i].c_str())){
                return true;
            }
        }
        return false;
    }

    static StimStrategyList strToEnum(const string strategyStr){
        StimStrategyList strategy = StimStrategyList(0);
        for(int i=0;i<NUM_STIM_STRATEGY;i++){
            if(strcmp(strategyStr.c_str(),StimStrategyName[i].c_str())){
                 strategy = StimStrategyList(i);
            }
        }
        return strategy;
    }

    static string enumToStr(StimStrategyList strategyEnum){
        int idx = static_cast<StimStrategyList>(strategyEnum);
        return StimStrategyName[idx];
    }
};

#endif // STRATEGYFACTORY_H
