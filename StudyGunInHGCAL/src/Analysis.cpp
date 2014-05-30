/**
 *  @file  Analysis.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    05/29/2014
 *
 *  @internal
 *     Created :  05/29/2014
 * Last update :  05/29/2014 05:23:17 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */


#include <math.h>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <map>
#include <vector>
#include <algorithm>

#include <TLorentzVector.h>
#include <TVector2.h>

#include "Analysis.h"
#include "Utilities.h"



using namespace AnHiMa;
using namespace std;



/*****************************************************************/
Analysis::Analysis():IAnalysis(),
    m_eventCount(0)
/*****************************************************************/
{

}



/*****************************************************************/
Analysis::~Analysis()
/*****************************************************************/
{

}



/*****************************************************************/
bool Analysis::initialize(const string& parameterFile)
/*****************************************************************/
{
    bool status = IAnalysis::initialize(parameterFile);
    if(!status) return status;
    event().connectVariables(m_inputChain);
    
    return true;
}


/*****************************************************************/
void Analysis::execute()
/*****************************************************************/
{
    if(!event().passSelection()) return;

    m_eventCount++;
    fillHistos();

}

/*****************************************************************/
void Analysis::fillHistos()
/*****************************************************************/
{

    short sysNum = 0;
    float weight = 1.;
    int hoffset  = 0;


    m_histos.FillHisto(0+hoffset, 0.5, weight, sysNum); // Number of events

    fillGenHistos();
    fillHitHistos();

}

/*****************************************************************/
void Analysis::fillGenHistos()
/*****************************************************************/
{
    short sysNum = 0;
    float weight = 1.;
    int hoffset  = 0;

    m_histos.FillHisto(10+hoffset, event().ngen, weight, sysNum); 

    for(int i=0;i<event().ngen;i++)
    {
        m_histos.FillHisto(11+hoffset, event().gen_eta[i], weight, sysNum); 
        m_histos.FillHisto(12+hoffset, event().gen_phi[i], weight, sysNum); 
        m_histos.FillHisto(13+hoffset, event().gen_pt[i], weight, sysNum); 
    }
}

/*****************************************************************/
void Analysis::fillHitHistos()
/*****************************************************************/
{
    short sysNum = 0;
    float weight = 1.;
    int hoffset  = 0;

    m_histos.FillHisto(50+hoffset, event().nhits, weight, sysNum); 

    // find max hit
    int imax = -1;
    int xmax = 0.;
    int ymax = 0.;
    int zmax = 0.;
    float Emax = 0;
    for(int i=0;i<event().nhits;i++)
    {
        if(event().hit_edep[i]>Emax)
        {
            imax = i;
            xmax = event().hit_x[i];
            ymax = event().hit_y[i];
            zmax = event().hit_z[i];
            Emax = event().hit_edep[i];
        }
    }
    // gen particle
    float eta_gen = event().gen_eta[0];

    for(int i=0;i<event().nhits;i++)
    {
        float x = event().hit_x[i];
        float y = event().hit_y[i];
        float z = event().hit_z[i];
        int layer = event().hit_layer[i];
        float energy = event().hit_edep[i];
        if(x==0 && y==0 && z==0) continue;
        if(z<0) continue;
        //cout<<"Hit energy = "<<event().hit_edep[i]<<"\n";
        m_histos.FillHisto(51+hoffset, energy, weight, sysNum); 
        m_histos.FillHisto(52+hoffset, x, y, weight, sysNum);

        float dx = x-xmax;
        float dy = y-ymax;
        float dr = sqrt(dx*dx+dy*dy);
        m_histos.FillHisto(53+hoffset, dr, weight, sysNum);
        m_histos.FillHisto(54+hoffset, dx, dy, weight, sysNum);
        if(fabs(eta_gen)<1.8)
        {
            m_histos.Fill1BinHisto(55+hoffset, layer, dx, dy, weight*energy, sysNum);
        }
        else if(fabs(eta_gen)<2.2)
        {
            m_histos.Fill1BinHisto(100+hoffset, layer, dx, dy, weight*energy, sysNum);
        }
        else if(fabs(eta_gen)<2.8)
        {
            m_histos.Fill1BinHisto(150+hoffset, layer, dx, dy, weight*energy, sysNum);
        }
        else if(fabs(eta_gen)<3.2)
        {
            m_histos.Fill1BinHisto(200+hoffset, layer, dx, dy, weight*energy, sysNum);
        }
        // Single events
        if(m_eventCount<=10)
        {
            m_histos.Fill2BinHisto(1000+hoffset, m_eventCount, layer, dx, dy, weight*energy, sysNum);
            m_histos.Fill2BinHisto(2000+hoffset, m_eventCount, layer, x, y, weight*energy, sysNum);
        }
    }

}
