/**
 *  @file  Event.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    05/29/2014
 *
 *  @internal
 *     Created :  05/29/2014
 * Last update :  05/29/2014 05:17:36 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#include <iostream>
#include "Event.h"
#include <TChain.h>


using namespace AnHiMa;
using namespace std;

/*****************************************************************/
Event::Event():IEvent()
/*****************************************************************/
{


}



/*****************************************************************/
Event::~Event()
/*****************************************************************/
{
}


/*****************************************************************/
void Event::connectVariables(TChain* inputChain)
/*****************************************************************/
{
    IEvent::connectVariables(inputChain);

    // Branches to read
    inputChain->SetBranchStatus("run"       , true);
    inputChain->SetBranchStatus("lumi"      , true);
    inputChain->SetBranchStatus("event"     , true);

    inputChain->SetBranchStatus("ngen"      , true);
    inputChain->SetBranchStatus("gen_id"    , true);
    inputChain->SetBranchStatus("gen_status", true);
    inputChain->SetBranchStatus("gen_pt"    , true);
    inputChain->SetBranchStatus("gen_eta"   , true);
    inputChain->SetBranchStatus("gen_phi"   , true);
    inputChain->SetBranchStatus("gen_en"    , true);


    inputChain->SetBranchStatus("nhits"     , true);
    inputChain->SetBranchStatus("hit_type"  , true);
    inputChain->SetBranchStatus("hit_layer" , true);
    inputChain->SetBranchStatus("hit_sec"   , true);
    inputChain->SetBranchStatus("hit_x"     , true);
    inputChain->SetBranchStatus("hit_y"     , true);
    inputChain->SetBranchStatus("hit_z"     , true);
    inputChain->SetBranchStatus("hit_edep"  , true);

    // Connect branches
    inputChain->SetBranchAddress("run"       , &run);
    inputChain->SetBranchAddress("lumi"      , &lumi);
    inputChain->SetBranchAddress("event"     , &event);

    inputChain->SetBranchAddress("ngen"      , &ngen);
    inputChain->SetBranchAddress("gen_id"    , gen_id);
    inputChain->SetBranchAddress("gen_status", gen_status);
    inputChain->SetBranchAddress("gen_pt"    , gen_pt);
    inputChain->SetBranchAddress("gen_eta"   , gen_eta);
    inputChain->SetBranchAddress("gen_phi"   , gen_phi);
    inputChain->SetBranchAddress("gen_en"    , gen_en);

    inputChain->SetBranchAddress("nhits"     , &nhits);
    inputChain->SetBranchAddress("hit_type"  , hit_type);
    inputChain->SetBranchAddress("hit_layer" , hit_layer);
    inputChain->SetBranchAddress("hit_sec"   , hit_sec);
    inputChain->SetBranchAddress("hit_x"     , hit_x);
    inputChain->SetBranchAddress("hit_y"     , hit_y);
    inputChain->SetBranchAddress("hit_z"     , hit_z);
    inputChain->SetBranchAddress("hit_edep"  , hit_edep);


}


/*****************************************************************/
bool Event::passSelection(int selection)
/*****************************************************************/
{
    return true;
}

