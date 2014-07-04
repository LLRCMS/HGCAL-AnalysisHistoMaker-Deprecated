/**
 *  @file  Analysis.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    05/29/2014
 *
 *  @internal
 *     Created :  05/29/2014
 * Last update :  05/29/2014 05:21:35 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "IAnalysis.h"
#include "Event.h"
#include "EventAware.h"
#include "Cluster.h"

#include <TLorentzVector.h>
#include <TRandom3.h>

#include <string>
#include <map>


namespace AnHiMa
{

    class GraphCorrection;

    class Analysis: public IAnalysis, EventAware<Event>
    {
        public:
            Analysis();
            ~Analysis();

            bool initialize(const std::string& parameterFile);

            void execute();

        private:
            void fillHistos();
            void fillGenHistos();
            void fillHitHistos();
            void fillClusterHistos();
            
            void computeHitsEtaPhi();
            void findGenParticle();
            void matchCellToGen();
            void buildCluster();

            std::string m_sampleType;
            bool m_maxHitRef;
            int m_eventCount;
            TLorentzVector m_genParticle;
            int m_matchedHit;
            TRandom3 m_random;
            Cluster m_cluster;

    };
}


#endif
