#include "resampling.h"

Resampling::Resampling()
{

}

void Resampling::setSamplingRate(double fsOrg, double fsRet){
    _timeRS = 0;
    _prioriPoint = 0;
    _delta = fsRet/fsOrg;
}

bool Resampling::downsampling(double sampleIn, double *sampleOut){
    // Real-time downsampling without anti-alias filter
    // Linear interpolation
    _timeRS = _timeRS + _delta;
    if(_timeRS>=1){
        double temp = (_timeRS-1)/_delta;
        *sampleOut = sampleIn - temp*(sampleIn-_prioriPoint);
        _timeRS = _timeRS-1;
        return true;
    }else{
        _prioriPoint = sampleIn;
        return false;
    }
}

Resampling::~Resampling(){

}
