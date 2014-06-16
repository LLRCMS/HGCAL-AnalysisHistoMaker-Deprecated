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
    m_sampleType("electron"),
    m_maxHitRef(true),
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

    m_sampleType = m_reader.params().GetValue("SampleType", "electron");
    m_maxHitRef = m_reader.params().GetValue("MaxHitRef", true);
    cout<<"INFO: SampleType = "<<m_sampleType<<"\n";
    if(m_maxHitRef) cout<<"INFO: Max hit in event taken as reference point\n";
    else cout<<"INFO: Generated particle (or random direction for pileup) taken as reference point\n";
    
    return true;
}


/*****************************************************************/
void Analysis::execute()
/*****************************************************************/
{
    if(!event().passSelection()) return;

    m_eventCount++;
    computeHitsEtaPhi();
    int nTries = 1;
    if(m_sampleType=="pileup") nTries = 10;
    for(int i=0;i<nTries;i++)
    {
        cout<<" Randomize "<<i<<"\n";
        findGenParticle();
        if(m_sampleType!="pileup")
        {
            matchCellToGen();
        }
        fillHistos();
    }

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
    if(m_sampleType=="pion")
    {
        vector<TLorentzVector> photons;
        double eta_pion = 0.;
        for(int i=0;i<event().ngen;i++)
        {
            if(event().gen_id[i]==22 && event().gen_status[i]==1)
            {
                photons.push_back(TLorentzVector());
                photons.back().SetPtEtaPhiE(event().gen_pt[i],event().gen_eta[i],event().gen_phi[i],event().gen_en[i]);
            }
            if(event().gen_id[i]==111) eta_pion = event().gen_eta[i];
        }
        if(photons.size()>2) cout<<"WARN: Found more than 2 photons in the event\n";
        if(photons.size()<2) cout<<"WARN: Found less than 2 photons in the event\n";
        else
        {
            TLorentzVector photon1 = photons[0];
            TLorentzVector photon2 = photons[1];
            double deta = photon2.Eta()-photon1.Eta();
            double dphi = photon2.DeltaPhi(photon1);
            m_histos.Fill1BinHisto(20+hoffset, eta_pion, fabs(deta), weight, sysNum);
            m_histos.Fill1BinHisto(25+hoffset, eta_pion, fabs(dphi), weight, sysNum);
        }
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
    double xmax = 0.;
    double ymax = 0.;
    double zmax = 0.;
    double Emax = 0;
    double etamax = 0.;
    double phimax = 0.;
    for(int i=0;i<event().nhits;i++)
    {
        if(event().hit_edep[i]>Emax)
        {
            imax = i;
            xmax = event().hit_x[i];
            ymax = event().hit_y[i];
            zmax = event().hit_z[i];
            etamax = event().hit_eta[i];
            phimax = event().hit_phi[i];
            Emax = event().hit_edep[i];
        }
    }
    // gen particle
    double eta_gen = m_genParticle.Eta();

    vector<double> energyPerLayer;
    energyPerLayer.resize(32);
    for(int i=0;i<event().nhits;i++)
    {
        double x = event().hit_x[i];
        double y = event().hit_y[i];
        double z = event().hit_z[i];
        double eta = event().hit_eta[i];
        double phi = event().hit_phi[i];
        int layer = event().hit_layer[i];
        double energy = event().hit_edep[i];
        if(x==0 && y==0 && z==0) continue;
        if(layer<0) continue;
        if(z<0) continue;
        //cout<<"Hit energy = "<<event().hit_edep[i]<<"\n";
        m_histos.FillHisto(51+hoffset, energy, weight, sysNum); 
        m_histos.FillHisto(52+hoffset, x, y, weight, sysNum);
        
        int i0 = m_matchedHits[layer];
        //float x0 = event().hit_x[i0];
        //float y0 = event().hit_y[i0];
        double eta0 = 0.;
        double phi0 = 0.;
        if(m_maxHitRef)
        {
            eta0 = etamax;
            phi0 = phimax;
        }
        else
        {
            eta0 = m_genParticle.Eta();//event().hit_eta[i0];
            phi0 = m_genParticle.Phi();//event().hit_phi[i0];
        }
        double theta0 = 2.*atan(exp(-eta0));
        double r0 = z*tan(theta0);
        double x0 = r0*cos(phi0); 
        double y0 = r0*sin(phi0);

        double dx = x-x0;
        double dy = y-y0;
        double deta = eta-eta0;
        double dphi = TVector2::Phi_mpi_pi(phi-phi0);
        double dr = sqrt(dx*dx+dy*dy);
        double drep = sqrt(deta*deta+dphi*dphi);
        m_histos.FillHisto(53+hoffset, dr, weight, sysNum);
        m_histos.FillHisto(54+hoffset, dx, dy, weight, sysNum);
        if(drep<0.4)
        {
            energyPerLayer[layer] += energy;
            if(fabs(eta_gen)<1.8)
            {
                m_histos.Fill1BinHisto(100+hoffset, layer, dx, dy, weight*energy, sysNum);
                m_histos.Fill1BinHisto(1100+hoffset, layer, deta, dphi, weight*energy, sysNum);
                m_histos.Fill1BinHisto(2100+hoffset, layer, deta, weight*energy, sysNum);
                m_histos.Fill1BinHisto(3100+hoffset, layer, dphi, weight*energy, sysNum);
            }
            else if(fabs(eta_gen)<2.2)
            {
                m_histos.Fill1BinHisto(150+hoffset, layer, dx, dy, weight*energy, sysNum);
                m_histos.Fill1BinHisto(1150+hoffset, layer, deta, dphi, weight*energy, sysNum);
                m_histos.Fill1BinHisto(2150+hoffset, layer, deta, weight*energy, sysNum);
                m_histos.Fill1BinHisto(3150+hoffset, layer, dphi, weight*energy, sysNum);
            }
            else if(fabs(eta_gen)<2.8)
            {
                m_histos.Fill1BinHisto(200+hoffset, layer, dx, dy, weight*energy, sysNum);
                m_histos.Fill1BinHisto(1200+hoffset, layer, deta, dphi, weight*energy, sysNum);
                m_histos.Fill1BinHisto(2200+hoffset, layer, deta, weight*energy, sysNum);
                m_histos.Fill1BinHisto(3200+hoffset, layer, dphi, weight*energy, sysNum);
            }
            else if(fabs(eta_gen)<3.2)
            {
                m_histos.Fill1BinHisto(250+hoffset, layer, dx, dy, weight*energy, sysNum);
                m_histos.Fill1BinHisto(1250+hoffset, layer, deta, dphi, weight*energy, sysNum);
                m_histos.Fill1BinHisto(2250+hoffset, layer, deta, weight*energy, sysNum);
                m_histos.Fill1BinHisto(3250+hoffset, layer, dphi, weight*energy, sysNum);
            }
        }
        // Single events
        if(m_eventCount<=10)
        {
            m_histos.Fill2BinHisto(10000+hoffset, m_eventCount, layer, dx, dy, weight*energy, sysNum);
            m_histos.Fill2BinHisto(11000+hoffset, m_eventCount, layer, x, y, weight*energy, sysNum);
            m_histos.Fill2BinHisto(12000+hoffset, m_eventCount, layer, deta, dphi, weight*energy, sysNum);
        }
    }


    double energySum = 0.;
    for(int i=0;i<32;i++)
    {
        double energyLayer = energyPerLayer[i];
        energySum += energyLayer;
    }
    if(energySum>0)
    {
        for(int layer=1;layer<32;layer++)
        {
            double energyFraction = energyPerLayer[layer]/energySum;
            if(fabs(eta_gen)<1.8)
            {
                m_histos.Fill1BinHisto(4100+hoffset, layer, energyFraction, weight, sysNum);
            }
            else if(fabs(eta_gen)<2.2)
            {
                m_histos.Fill1BinHisto(4150+hoffset, layer, energyFraction, weight, sysNum);
            }
            else if(fabs(eta_gen)<2.8)
            {
                m_histos.Fill1BinHisto(4200+hoffset, layer, energyFraction, weight, sysNum);
            }
            else if(fabs(eta_gen)<3.2)
            {
                m_histos.Fill1BinHisto(4250+hoffset, layer, energyFraction, weight, sysNum);
            }
        }
    }


}

/*****************************************************************/
void Analysis::findGenParticle()
/*****************************************************************/
{
    if(m_sampleType=="electron" || m_sampleType=="photon" )
    {
        m_genParticle.SetPtEtaPhiE(event().gen_pt[0], event().gen_eta[0], event().gen_phi[0], event().gen_en[0]);
    }
    else if(m_sampleType=="pion")
    {
        int ngen = event().ngen;
        int npions = 0;
        for(int i=0;i<event().ngen;i++)
        {
            // looking for pi0
            if(event().gen_id[i]==111)
            {
                npions++;
                m_genParticle.SetPtEtaPhiE(event().gen_pt[i], event().gen_eta[i], event().gen_phi[i], event().gen_en[i]);
            }
        }
        if(npions>1)
        {
            cout<<"WARN: More than 2 generated pi0 in the event. The last one in the list is taken.\n";
        }
    }
    else if(m_sampleType=="pileup")
    {
        vector<double> etas = {1.5,2.,2.5,2.9};
        unsigned int ieta = m_random.Integer(4);
        double phi = m_random.Uniform(-M_PI, M_PI);
        m_genParticle.SetPtEtaPhiM(1., etas[ieta], phi, 0.);
    }
}

/*****************************************************************/
void Analysis::matchCellToGen()
/*****************************************************************/
{
    map<int,float> matchedDistances;
    for(int i=1;i<32;i++)
    {
        m_matchedHits[i] = -1;
        matchedDistances[i] = 999.;
    }
    float etag = m_genParticle.Eta();
    float phig = m_genParticle.Phi();
    for(int i=0;i<event().nhits;i++)
    {
        int layer = event().hit_layer[i];
        if(layer<0) continue;
        float eta = event().hit_eta[i];
        float phi = event().hit_phi[i];
        float deta = etag - eta;
        float dphi = TVector2::Phi_mpi_pi(phig-phi);
        float dr = sqrt(deta*deta+dphi*dphi);
        if(dr<0.7 && (m_matchedHits[layer]==-1 || dr<matchedDistances[layer]))
        {
            m_matchedHits[layer] = i;
            matchedDistances[layer] = dr;
        }
    }
}


/*****************************************************************/
void Analysis::computeHitsEtaPhi()
/*****************************************************************/
{
    for(int i=0;i<event().nhits;i++)
    {
        double x = event().hit_x[i];
        double y = event().hit_y[i];
        double z = event().hit_z[i];
        double rho = sqrt(x*x+y*y+z*z);
        double phi = TMath::ATan2(y,x);
        double eta = 0;
        if (rho>=z) eta=0.5*TMath::Log( (rho+z)/(rho-z) );
        event().hit_eta[i] = (float)eta;
        event().hit_phi[i] = (float)phi;
    }
}
