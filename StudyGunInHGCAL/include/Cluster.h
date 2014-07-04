/**
 *  @file  Cluster.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/02/2014
 *
 *  @internal
 *     Created :  07/02/2014
 * Last update :  07/02/2014 04:54:37 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#ifndef CLUSTER_H
#define CLUSTER_H

#include <vector>
#include <map>
#include "TLorentzVector.h"

namespace AnHiMa
{

    class Event;

    class Cluster: public TLorentzVector
    {
        public:
            Cluster();
            ~Cluster();

            void build(Event&, double, double);

            double sigmaEtaEta() {return m_sigmaEtaEta;};
            double sigmaPhiPhi() {return m_sigmaPhiPhi;};
            double sigmaIzIz() {return m_sigmaIzIz;};

            double sigmaEtaEta(int layer) {return m_layerSigmaEtaEta[layer];};
            double sigmaPhiPhi(int layer) {return m_layerSigmaPhiPhi[layer];};  

            int maxLayer() {return m_maxLayer;};
            double layerEnergy(int);
            double layerSigmaEtaEta(int);
            double layerSigmaPhiPhi(int);

        private:
            double m_sigmaEtaEta;
            double m_sigmaPhiPhi;
            double m_sigmaIzIz;
            int m_maxLayer;
            std::map<int,double> m_layerSigmaEtaEta;
            std::map<int,double> m_layerSigmaPhiPhi;
            std::map<int,double> m_layerEnergies;

    };
};

#endif
