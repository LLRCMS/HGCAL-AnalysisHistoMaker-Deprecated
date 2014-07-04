/**
 *  @file  Cluster.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/02/2014
 *
 *  @internal
 *     Created :  07/02/2014
 * Last update :  07/02/2014 05:04:59 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */

#include <math.h>
#include <iostream>

#include "TVector2.h"

#include "Cluster.h"
#include "Event.h"

using namespace std;
using namespace AnHiMa;


/*****************************************************************/
Cluster::Cluster():TLorentzVector(),
    m_sigmaEtaEta(0.),
    m_sigmaPhiPhi(0.),
    m_sigmaIzIz(0.),
    m_maxLayer(-1)
/*****************************************************************/
{
}

/*****************************************************************/
Cluster::~Cluster()
/*****************************************************************/
{
}


/*****************************************************************/
void Cluster::build(Event& event, double eta0, double phi0)
/*****************************************************************/
{
    double etaWidth = 0.05; // half widths
    double phiWidth = 0.2;
    double clusterEnergy = 0.;
    double clusterEta = 0.;
    double clusterPhi = 0.;
    double clusterLayer = 0.;

    vector<double> etas;
    vector<double> phis;
    vector<double> layers;
    vector<double> energies;
    map<int,double> layerEnergies;
    // compute energy and position of the cluster
    for(int i=0;i<event.nhits;i++)
    {
        double eta = event.hit_eta[i];
        double phi = event.hit_phi[i];
        double z   = event.hit_z[i];
        int layer = event.hit_layer[i];
        double energy = event.hit_edep[i]; 
        double deta = eta-eta0;
        double dphi = TVector2::Phi_mpi_pi(phi-phi0);
        if(fabs(deta)<0.05 && fabs(dphi)<0.2) // box 
        {
            clusterEnergy += energy;
            clusterEta += eta*energy;
            clusterPhi += phi*energy;
            clusterLayer += (double)layer*energy;
            etas.push_back(eta);
            phis.push_back(phi);
            layers.push_back(layer);
            energies.push_back(energy);
            if(layerEnergies.find(layer)==layerEnergies.end()) layerEnergies[layer] = 0.;
            layerEnergies[layer] += energy;
        }
    }
    if(clusterEnergy==0.) 
    {
        SetPtEtaPhiM(0.,0.,0.,0.);
        return;
    }
    clusterEta /= clusterEnergy;
    clusterPhi /= clusterEnergy;
    clusterLayer /= clusterEnergy;
    double clusterEt = clusterEnergy/cosh(clusterEta);
    SetPtEtaPhiM(clusterEt, clusterEta, clusterPhi, 0.);
    m_layerEnergies = layerEnergies;

    // compute shape variables
    double clusterSigmaEtaEta = 0.;
    double clusterSigmaPhiPhi = 0.;
    double clusterSigmaIzIz = 0.;
    map<int,double> layerSigmaEtaEta;
    map<int,double> layerSigmaPhiPhi;
    for(int i=0;i<energies.size();i++)
    {
        double eta = etas[i];
        double phi = phis[i];
        double layer = layers[i];
        double energy = energies[i];
        double deta = eta-clusterEta;
        double dphi = TVector2::Phi_mpi_pi(phi-clusterPhi);
        double dlayer = (double)layer-clusterLayer;
        clusterSigmaEtaEta += deta*deta*energy;
        clusterSigmaPhiPhi += dphi*dphi*energy;
        clusterSigmaIzIz += dlayer*dlayer*energy;
        if(layerSigmaEtaEta.find(layer)==layerSigmaEtaEta.end()) layerSigmaEtaEta[layer] = 0.;
        if(layerSigmaPhiPhi.find(layer)==layerSigmaPhiPhi.end()) layerSigmaPhiPhi[layer] = 0.;
        layerSigmaEtaEta[layer] += deta*deta*energy;
        layerSigmaPhiPhi[layer] += dphi*dphi*energy;
    }
    clusterSigmaEtaEta /= clusterEnergy;
    clusterSigmaPhiPhi /= clusterEnergy;
    clusterSigmaIzIz   /= clusterEnergy;
    m_sigmaEtaEta = sqrt(clusterSigmaEtaEta);
    m_sigmaPhiPhi = sqrt(clusterSigmaPhiPhi);
    m_sigmaIzIz = sqrt(clusterSigmaIzIz);
    for(auto itr=m_layerEnergies.cbegin();itr!=m_layerEnergies.end();itr++)
    {
        int layer = itr->first;
        double energy = itr->second;
        layerSigmaEtaEta[layer] /= energy;
        layerSigmaPhiPhi[layer] /= energy;
        layerSigmaEtaEta[layer] = sqrt(layerSigmaEtaEta[layer]);
        layerSigmaPhiPhi[layer] = sqrt(layerSigmaPhiPhi[layer]);
    }
    m_layerSigmaEtaEta = layerSigmaEtaEta;
    m_layerSigmaPhiPhi = layerSigmaPhiPhi;

    // find maximum layer
    int maxLayer = -1;
    double maxEnergy = 0.;
    for(auto itr=layerEnergies.cbegin();itr!=layerEnergies.end();itr++)
    {
        if(itr->second>maxEnergy)
        {
            maxLayer = itr->first;
            maxEnergy = itr->second;
        }
    }
    m_maxLayer = maxLayer;
    
}



/*****************************************************************/
double Cluster::layerEnergy(int layer)
/*****************************************************************/
{
    double energy = 0.;
    auto itr = m_layerEnergies.find(layer);
    if(itr!=m_layerEnergies.end()) energy = itr->second;

    return energy;
}

/*****************************************************************/
double Cluster::layerSigmaEtaEta(int layer)
/*****************************************************************/
{
    double sigmaEtaEta = 0.;
    auto itr = m_layerSigmaEtaEta.find(layer);
    if(itr!=m_layerSigmaEtaEta.end()) sigmaEtaEta = itr->second;

    return sigmaEtaEta;
}


/*****************************************************************/
double Cluster::layerSigmaPhiPhi(int layer)
/*****************************************************************/
{
    double sigmaPhiPhi = 0.;
    auto itr = m_layerSigmaPhiPhi.find(layer);
    if(itr!=m_layerSigmaPhiPhi.end()) sigmaPhiPhi = itr->second;

    return sigmaPhiPhi;
}
