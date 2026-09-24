#ifndef RESAMPLING_H
#define RESAMPLING_H


class Resampling
{
public:
    Resampling();
    ~Resampling();

    void setSamplingRate(double fsOrg, double fsRet);

    // Return true if there are output, otherwise return false
    bool downsampling(double sampleIn, double *sampleOut);

private:
    double _delta;
    double _timeRS;
    double _prioriPoint;
};

#endif // RESAMPLING_H
