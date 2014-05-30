/**
 *  @file  Event.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    05/29/2014
 *
 *  @internal
 *     Created :  05/29/2014
 * Last update :  05/29/2014 04:45:27 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */




#ifndef EVENT_H
#define EVENT_H

#include "IEvent.h"
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <string>


namespace AnHiMa
{


    class Event: public IEvent
    {
        public:
            Event();
            ~Event();

            bool passSelection(int selection=0);
            void connectVariables(TChain* inputChain);

            //-------------
            // Input variables
            //-------------
            int run;
            int lumi;
            int event;

            // gen particles
            int   ngen;
            int   gen_id[10];
            float gen_pt[10];
            float gen_eta[10];
            float gen_phi[10];
            float gen_en[10];

            // hits
            int   nhits;      
            int   hit_type[10000];
            int   hit_layer[10000];
            int   hit_sec[10000];
            float hit_x[10000];
            float hit_y[10000];
            float hit_z[10000];
            float hit_edep[10000];

    };
}

#endif
