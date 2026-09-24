#include "cascadeiir.h"

CascadeIIR::CascadeIIR(int nSOS, double **sosMat, StateType type){
    m_num = nSOS;
    m_stages = new Biquad*[nSOS];
    m_states = new State*[nSOS];
    for(int i=0;i<m_num;i++){
        m_stages[i] = new Biquad;
        m_stages[i]->setCoefficients(
                    sosMat[i][3],
                    sosMat[i][4],
                    sosMat[i][5],
                    sosMat[i][0],
                    sosMat[i][1],
                    sosMat[i][2]);
        m_states[i] = StateFactory::getStateInstance(type);
    }
}

CascadeIIR::CascadeIIR(int nSOS, const double sosMat[][6], StateType type){
    m_num = nSOS;
    m_stages = new Biquad*[nSOS];
    m_states = new State*[nSOS];
    for(int i=0;i<m_num;i++){
        m_stages[i] = new Biquad;
        m_stages[i]->setCoefficients(
                    sosMat[i][3],
                    sosMat[i][4],
                    sosMat[i][5],
                    sosMat[i][0],
                    sosMat[i][1],
                    sosMat[i][2]);
        m_states[i] = StateFactory::getStateInstance(type);
    }
}

CascadeIIR::~CascadeIIR(){
    for(int i=0;i<m_num;i++){
        if(m_stages[i]){
            delete m_stages[i];
        }
        if(m_states[i]){
            delete m_states[i];
        }
    }
    if(m_stages){
        delete [] m_stages;
    }
    if(m_states){
        delete [] m_states;
    }

}

void CascadeIIR::reset(){
    for(int i=0;i<m_num;i++){
        m_states[i]->reset();
    }
}

double CascadeIIR::filter(double in){
    double out = in;
    for(int i=0;i<m_num;i++){
        out = m_states[i]->filter(out, m_stages[i]);
    }
    return out;
}
