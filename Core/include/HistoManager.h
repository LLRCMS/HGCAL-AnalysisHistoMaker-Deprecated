////////////////////////////////////////////////////
// Class HistoManager
//
// Author     : F. Cassol Brunner
// Created    : 9/10/2002
// Last updated : $Date: 2007/09/24 08:32:55 $
//           by : $Author: jsauvan $
// Comment: allows to manage histos, and their declaration  
//	    through a parameter file
////////////////////////////////////////////////////

#ifndef HISTOMANAGER_H
#define HISTOMANAGER_H



#include <sstream>
#include <cctype>
#include <cstdio>
#include <cstdlib> 
#include <iostream>
#include <fstream> 
#include <vector>
#include <string>
#include <list>

//------ ROOT includes --------- 
#include <TFile.h> 
#include <TH1.h> 
#include <TH2.h> 
#include <TH3.h> 
#include <TTree.h> 
#include <TNtupleD.h>
#include <TArrayF.h>
#include <TArrayI.h>
#include <TClonesArray.h>
#include <TFile.h>


namespace AnHiMa
{



	class HistoManager
    { 
		////////////////////////////////////////////////////////////// 
		//     basic class for histogramming 
		///////////////////////////////////////////////////////////// 
		public : 


			TClonesArray*  fHisto1D[2000];    // list of 1D histograms
			TClonesArray*  fHisto1D_d[2000];    // list of 1D histograms, double precision
			TClonesArray*  fHisto2D[2000];    // list of 2D histograms
			TClonesArray*  fHisto3D[2000];    // list of 3D histograms
			TClonesArray*  fNtuple[2000];     // list of Ntuples      

			TArrayI        fNtBufSlot;      // index to nt buffer slot for 1d histos (size: last id of 1d histos)
			TArrayD        fNtBuffer;       // nt buffer for ntuple filling (size: last id of ntuple + its nvar) 

			HistoManager();
			void initialize(TFile* histoFile, std::string Parameters_file,std::string Tag);

			virtual ~HistoManager();

			virtual void ReadParameters(std::string Parameters_file,std::string Tag,const Short_t SysNum=0);
			virtual void ReadParameters(std::string parameters_file,std::string Tag,std::string ClassName,
					std::list<std::string>& PartTags,const Short_t SysNum=0); 
			virtual bool ParseHistos(
					std::istringstream& inParams,std::string& type, Int_t hoffset,
					std::string Tag, std::list<std::string>& PartTags,const Short_t SysNum=0);

			//--- Histo cards parsers
			void ReadParamsh1f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParamsh1d(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParamsh2f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParamsh1fgeneric(std::istringstream &inParams,std::string type,Int_t hoffset,std::list<std::string>& PartTags,
					const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams2dbin(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams3dbin(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0);
			void ReadParams1binfh1f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams1binvh1f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams1binvh1d(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams1binvh2f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams1binvvh1f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams1binVarSizeh1f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams2binVarSizeh2f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams2binfh1f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams2binvh1f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);
			void ReadParams2binvh2f(std::istringstream &inParams,std::string type,Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);

			void ReadParamsNtuple(std::istringstream &inParams,std::string type, Int_t hoffset,const Short_t SysNum=0,const Bool_t BookforSys=kFALSE);

			//--- histogram filling routines
			void FillHisto( const int& num, const double& value, const double& wait,const Short_t SysNum=0);
			void FillHistoD( const int& num, const double& value, const double& wait,const Short_t SysNum=0);
			void FillHisto( const int& num, const double& value_x, const double& value_y, const double& wait,const Short_t SysNum=0);
			void FillHisto( const int& num, const double& value_x, const double& value_y, const double& value_z,const double& wait,const Short_t SysNum=0);
			void Fill1BinHisto(Int_t hnum,Double_t binvalue,Double_t value,Double_t wait,const Short_t SysNum=0);
			void Fill1BinHistoD(Int_t hnum,Double_t binvalue,Double_t value,Double_t wait,const Short_t SysNum=0);
			void Fill1BinHisto(Int_t hnum,Double_t binvalue,Double_t value1, Double_t value2,Double_t wait,const Short_t SysNum=0);
			void Fill2BinHisto(Int_t hnum,Double_t binvalue,Double_t binvalue2,Double_t value,Double_t wait,const Short_t SysNum=0);
			void Fill2BinHisto(Int_t hnum,Double_t binvalue,Double_t binvalue2,Double_t value1,Double_t value2,Double_t wait,const Short_t SysNum=0);

			//--- ntuple filling routine
			void FillNtuple( Int_t num, Double_t run, Double_t event, Float_t weight, const Short_t SysNum=0);

			void Scale1dHisto(Int_t hnum,Float_t ScaleParam,const Short_t SysNum=0);
			
			//--- utilities routines 
			bool areInTheSameBin(const int&  numhisto,const double& x1,const double& x2,const Short_t SysNum=0);
            const TH1F* getHisto1D(int numHisto, const Short_t SysNum=0);
            const TH2F* getHisto2D(int numHisto, const Short_t SysNum=0);

	}; 

}
#endif
