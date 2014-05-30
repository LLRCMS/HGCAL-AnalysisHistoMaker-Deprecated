/**
 *  @file  IEvent.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    12/17/2012
 *
 *  @internal
 *     Created :  12/17/2012
 * Last update :  12/17/2012 05:27:28 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */

#ifndef IEVENT_H
#define IEVENT_H

#include <map>
#include <string>
#include <iostream>


class TChain;
class TBranch;

namespace AnHiMa
{
    class IEvent
    {
        public:
            IEvent();
            ~IEvent();

            virtual bool passSelection(int selection=0) = 0;
            void connectVariables(TChain* inputChain);

        //protected:
        //    TChain* inputChain() {return m_inputChain;}

        private:
            TChain* m_inputChain;


    };
}

#endif
