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
    static const int MAXGEN=10000;
    static const int MAXHIT=10000000;

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
            int   gen_id    [MAXGEN];
            int   gen_status[MAXGEN];
            float gen_pt    [MAXGEN];
            float gen_eta   [MAXGEN];
            float gen_phi   [MAXGEN];
            float gen_en    [MAXGEN];

            // hits
            int   nhits;      
            int   hit_type [MAXHIT];
            int   hit_layer[MAXHIT];
            int   hit_sec  [MAXHIT];
            float hit_x    [MAXHIT];
            float hit_y    [MAXHIT];
            float hit_z    [MAXHIT];
            float hit_eta  [MAXHIT];
            float hit_phi  [MAXHIT];
            float hit_edep [MAXHIT];

    };
}

#endif
