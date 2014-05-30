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

#include <TLorentzVector.h>

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

            int m_eventCount;

    };
}


#endif
