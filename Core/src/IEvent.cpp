/**
 *  @file  IEvent.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    12/21/2012
 *
 *  @internal
 *     Created :  12/21/2012
 * Last update :  12/21/2012 02:56:20 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */




#include <vector>
#include <TChain.h>
#include <TBranch.h>

#include "IEvent.h"

using namespace AnHiMa;
using namespace std;

/*****************************************************************/
IEvent::IEvent():
    m_inputChain(NULL)
/*****************************************************************/
{
}



/*****************************************************************/
IEvent::~IEvent()
/*****************************************************************/
{
}

/*****************************************************************/
void IEvent::connectVariables(TChain* inputChain)
/*****************************************************************/
{
    m_inputChain = inputChain;
    inputChain->SetBranchStatus("*", false);
    
}
