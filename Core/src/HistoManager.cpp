#include "HistoManager.h" 
#include <TROOT.h>

using namespace std;
using namespace AnHiMa;

void HistoManager::ReadParameters(string parameters_file,string Tag,const Short_t SysNum) 
{  

 string ClassName="";
 
 list<string> PartTags;
 ReadParameters(parameters_file,Tag,ClassName,PartTags,SysNum);

}


void HistoManager::ReadParameters(string parameters_file,string Tag,string ClassName,
					list<string>& PartTags,const Short_t SysNum) 
{  
  string item;
//... open the Parameters file
  
  try {

   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 1000 "<< endl;              
    string mess=error.str();      
     
    throw mess; 
    return;
   }



   ifstream inParams(parameters_file.c_str());
   
   if(!inParams)
     throw string("HistoManager::ReadParameters = Error in opening histogram parameter file! - stop");
   
   string end="end_";
   string start=Tag+":";
   
   bool stop=false;
   string token,type,readtag,dummy;
   
//... stream buffer with histogram definitions
   istringstream* parstrstr=0;
   string parstr;
   
    while (getline(inParams, item)) {
      if (item.find(start) == string::npos) { continue; }
      cout << endl << "Start reading loop " << Tag << 
           " (SysNum " << SysNum << "): " << endl;
      string bufx(item.substr(item.find(":") + 1));
      string::size_type more = 0;
      parstr = "";
      do {
        //... eat whitespace
        while ( bufx[0] == ' ' ) {
          bufx = bufx.substr(1,bufx.size());
        }
        //cout << bufx << endl;

        // check for multiple entries separated by ,      
        more = bufx.find(",");
        //cout << more << endl;
	if (more == string::npos) {
	  token = bufx;
        } else {
          token = bufx.substr(0,more);          
          bufx = bufx.substr(more+1,bufx.size());
        }

        //cout << "token: " <<token << endl;
        //... optional histogram id offset in parentheses  
        Int_t hoffset = 0;
        string::size_type s1 = token.find("(")+1;
        string::size_type s2 = token.find(")")-s1;
        string oc = token.substr(s1,s2);  
        hoffset = atoi(oc.data());

        //cout << hoffset << endl;
        if (hoffset) {
          type = token.substr(0,s1 - 1).c_str();
        } else {
          type = token;
          while ( bufx[type.size()-1] == ' ' ) {//... eat whitespace
            type = type.substr(0,type.size()-1);
          }
         }       
        if(ClassName!="") type=type+ClassName;

	cout << endl 
          << "________________________________________________________" << endl
          << "type: '" << type <<"' histogram id offset: " << hoffset   << endl
          << "________________________________________________________" << endl
          << endl;

    	//... on first iteration read histogram definitions into a buffer
        if ( parstr == "" ) {        
          parstr = "";
  	  while (!stop && getline(inParams,item)) {
            parstr += item;            
            parstr += '\n';
            if (item == end + Tag) { stop = true; }		          
          }
        }

        //cout << parstr << endl;
        parstrstr = new istringstream(parstr.data());
        ParseHistos(*parstrstr,type,hoffset,Tag,PartTags,SysNum);
        delete parstrstr;

        //cout << "token: " <<token << endl;
        //if (more!=string::npos) cout  << "_______________________" << endl;

      } while (more!=string::npos);

      stop = false;      
    } //... while

   
   // make nt buffer slot index large enough (always!)
   fNtBufSlot.Set(max(fHisto1D[0]->GetLast(),fHisto1D_d[0]->GetLast())+1);

   cout << endl 
        << "--> number of h1f histos "<< fHisto1D[SysNum]->GetEntries()<<endl;
   cout << "--> number of h1d histos "<< fHisto1D_d[SysNum]->GetEntries()<<endl;
   cout << "--> number of h2f histos "<< fHisto2D[SysNum]->GetEntries()<<endl;
   cout << "--> number of ntuples    "<< fNtuple[SysNum]->GetEntries()<<endl;

//... close the Parameters file
   inParams.close(); 

  } //... try 
  catch(string error){
     cout << error <<endl;;
     exit(0); 
  } catch(...){
     throw string("HistoManager::Parameters = Undefined error in reading parameters_file! - stop");
     exit(0); 
  }     
     
}



bool HistoManager::ParseHistos(
  istringstream& inParams, string&type, Int_t hoffset,
  string Tag, list<string>& PartTags,const Short_t SysNum)
{   
//     Read and parse histo definitions
    
    bool   stop=false;
    string item;
    string end="end_";

    while (!stop && inParams >> item) {
        //...read and create h1f histos	;
        if(item=="h1f") {
            ReadParamsh1f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="h1d") {
            ReadParamsh1d(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="h1f_sys"){
            ReadParamsh1f(inParams,type,hoffset,SysNum,kTRUE);
        }
        else if(item=="h2f"){
            ReadParamsh2f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="h2f_sys"){
            ReadParamsh2f(inParams,type,hoffset,SysNum,kTRUE);
        }
        //...read and create h1f generic histos	;
        else if(item=="h1fgeneric") {
            ReadParamsh1fgeneric(inParams,type,hoffset,PartTags,SysNum,kFALSE);
        }
        else if(item=="h1fgeneric_sys") {
            ReadParamsh1fgeneric(inParams,type,hoffset,PartTags,SysNum,kTRUE);
        }

        //... for 2D histograms with variable bin width (as defined in Root)
        else if(item=="2dbin"){
            ReadParams2dbin(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="2dbin_sys"){
            ReadParams2dbin(inParams,type,hoffset,SysNum,kTRUE);
        }
        //... for 3D histograms with variable bin width (as defined in Root)
        else if(item=="3dbin"){
            ReadParams3dbin(inParams,type,hoffset,SysNum);
        }

        //... for bin grid histograms with f2x bin width
        else if(item=="1binfh1f"){
            ReadParams1binfh1f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="1binfh1f_sys"){
            ReadParams1binfh1f(inParams,type,hoffset,SysNum,kTRUE);
        }
        //... for bin grid histogram with variable histogram bin width
        else if(item=="1binvbh1f"){
            ReadParams1binVarSizeh1f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="1binvbh1f_sys"){
            ReadParams1binVarSizeh1f(inParams,type,hoffset,SysNum,kTRUE);
        }
        //... for bin histograms with variable bin width
        else if(item=="1binvh1f"){
            ReadParams1binvh1f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="1binvh1f_sys"){
            ReadParams1binvh1f(inParams,type,hoffset,SysNum,kTRUE);
        }
        else if(item=="1binvh1d"){
            ReadParams1binvh1d(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="1binvh1d_sys"){
            ReadParams1binvh1d(inParams,type,hoffset,SysNum,kTRUE);
        }
        else if(item=="1binvh2f"){
            ReadParams1binvh2f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="1binvh2f_sys"){
            ReadParams1binvh2f(inParams,type,hoffset,SysNum,kTRUE);
        }
        //... for variable bin histograms with variable bin width
        else if(item=="1binvvh1f"){
            ReadParams1binvvh1f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="1binvvh1f_sys"){
            ReadParams1binvvh1f(inParams,type,hoffset,SysNum,kTRUE);
        }
        //... for 2bin histograms with fix bin width
        else if(item=="2binfh1f"){
            ReadParams2binfh1f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="2binfh1f_sys"){
            ReadParams2binfh1f(inParams,type,hoffset,SysNum,kTRUE);
        }
        //... for 2bin histograms with variable bin width
        else if(item=="2binvh1f"){
            ReadParams2binvh1f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="2binvh1f_sys"){
            ReadParams2binvh1f(inParams,type,hoffset,SysNum,kTRUE);
        }
        else if(item=="2binvh2f"){
            ReadParams2binvh2f(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="2binvh2f_sys"){
            ReadParams2binvh2f(inParams,type,hoffset,SysNum,kTRUE);
        }
        //... for ntuples
        else if(item=="ntuple"){
            ReadParamsNtuple(inParams,type,hoffset,SysNum,kFALSE);
        }
        else if(item=="ntuple_sys"){
            ReadParamsNtuple(inParams,type,hoffset,SysNum,kTRUE);

        } else if (item == end + Tag) {
            stop = true;
        }			//if
    }			//while 
  return stop;
}



 
HistoManager::HistoManager(){ 

    for(Int_t i=0;i<1000;i++) {
    	fHisto1D[i] = new TClonesArray("TH1F",1);
    	fHisto1D_d[i] = new TClonesArray("TH1D",1);
    	fHisto2D[i] = new TClonesArray("TH2F",1);
    	fHisto3D[i] = new TClonesArray("TH3F",1);
      	fNtuple[i]  = new TClonesArray("TNtupleD",1);
    }
    // need at least one element in nt buffer (used as /dev/null)
    fNtBuffer.Set(1);   
}
 
void HistoManager::initialize(TFile* histoFile, string Parameters_file,string Tag){ 
    //for(Int_t i=0;i<1000;i++) {
    //	fHisto1D[i] = new TClonesArray("TH1F",1);
    //	fHisto1D_d[i] = new TClonesArray("TH1D",1);
    //	fHisto2D[i] = new TClonesArray("TH2F",1);
    //	fHisto3D[i] = new TClonesArray("TH3F",1);
    //  	fNtuple[i]  = new TClonesArray("TNtupleD",1);
    //}
    //// need at least one element in nt buffer (used as /dev/null)
    //fNtBuffer.Set(1);
    ReadParameters(Parameters_file,Tag);
    //...read the parameters and book the histograms in the / dir
    histoFile->cd(); //-- back to the file with histos


    //...read the parameters and book the histograms in each sub-directory
    TList* dir=histoFile->GetListOfKeys();
    for(int i=0;i<dir->GetSize();i++){
        cout <<endl<< "===============================================================" << endl
            << "--> Create histo in  dir "<< dir->At(i)->GetName() << endl;
        histoFile->cd(dir->At(i)->GetName());

        ReadParameters(Parameters_file,Tag,i+1);
    }
    //run.histoFile->cd();
    gROOT->cd();
}  
 
HistoManager::~HistoManager(){ 
    

//---- don't know why, but seems to block the jobs at the end (for 2D histos only ?)
//---- therefore very brutal solution: remove it ! crack !!
//  for(Int_t i=0;i<1000;i++) {
// //...delete array if 1D histo   
//      if(fHisto1D[i]){
//        fHisto1D[i]->Delete();
//        //delete fHisto1D[i];
//      }
////    
// //...delete array if 2D histo 
//      if(fHisto2D[i]){
//        fHisto2D[i]->Delete();
////        delete fHisto2D[i];
//      }  
//   }

}

void HistoManager::FillHisto(const int& num, const double& value, const double& weight,const Short_t SysNum){ 
//...fill histogram h1f
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 1000 "<< endl;              
    string mess=error.str();      
    throw mess; 
    return;
   }
  
   
  if(num>fHisto1D[SysNum]->GetLast()) return;
   TH1F* histo= (TH1F*)fHisto1D[SysNum]->At(num);
   
  if ( histo == 0 ) {
    if(SysNum==0) {
      ostringstream error;
      error << "HistoManager::FillHisto = Error 1D histogram num " 
              << num << " doesn't exist "<< endl;              
	string mess=error.str();      
	throw mess; 
    }
  } else {
  
    histo->Fill(value,weight);
    fNtBuffer[fNtBufSlot[num]] = (Double_t)value;     
  }
} 

void HistoManager::FillHistoD(const int& num, const double& value, const double& weight,const Short_t SysNum){ 
//...fill histogram h1f
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHistoD = Error Systematic num " 
          << SysNum << " >= 1000 "<< endl;              
    string mess=error.str();      
    throw mess; 
    return;
   }
  
   
  if(num>fHisto1D_d[SysNum]->GetLast()) return;
   TH1D* histo= (TH1D*)fHisto1D_d[SysNum]->At(num);
   
  if ( histo == 0 ) {
    if(SysNum==0) {
      ostringstream error;
      error << "HistoManager::FillHistoD = Error 1D double histogram num " 
              << num << " doesn't exist "<< endl;              
	string mess=error.str();      
	throw mess; 
    }
  } else {
  
    histo->Fill(value,weight);
    //fNtBuffer[fNtBufSlot[num]] = (Double_t)value;     
  }
}

void HistoManager::FillNtuple( Int_t num, Double_t run, Double_t event, Float_t weight, const Short_t SysNum)
{
//... fill ntuple
// associated histograms have to be filled before to fill nt buffer!

  if ( num > fNtuple[SysNum]->GetLast() ) { return; }
  
  TNtupleD* nt = static_cast<TNtupleD*>(fNtuple[SysNum]->At(num));

  if ( nt==0 ) {
    if(SysNum==0) {
      cerr << "HistoManager::FillNtuple Error Ntuple num "<<num<<" doesn't exist"<<endl;        
      throw("bye!"); 
    } 
  } else {

    fNtBuffer[num] = run;
    fNtBuffer[num+1] = event;  
    fNtBuffer[num+2] = (Double_t)weight;  

    const Double_t * buf = fNtBuffer.GetArray();
    nt->Fill(buf+num);
  }
}

void HistoManager::FillHisto(const int& num, const double& value_x, const double& value_y, const double& wait,const Short_t SysNum){ 
//...fill histogram h2f
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 1000 "<< endl;              
    string mess=error.str();      
     
    throw mess; 
    return;
   }

   if(num>fHisto2D[SysNum]->GetLast()) return;
   TH2F* histo= (TH2F*)fHisto2D[SysNum]->At(num);
   
   if(histo != NULL)histo->Fill(value_x,value_y,wait);
   else {
    if(SysNum==0) {
	ostringstream error;
	error << "HistoManager::FillHisto = Error 2D histogram num " 
              << num << " doesn't exist "<< endl;              
	string mess=error.str();      
	throw mess; 
    }
  
   }
} 

void HistoManager::FillHisto(const int& num, const double& value_x, const double& value_y, const double& value_z, const double& wait,const Short_t SysNum)
{ 
//...fill histogram h3f
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 1000 "<< endl;              
    string mess=error.str();      
     
    throw mess; 
    return;
   }

   if(num>fHisto3D[SysNum]->GetLast()) return;
   TH3F* histo= (TH3F*)fHisto3D[SysNum]->At(num);
   
   if(histo != NULL)histo->Fill(value_x,value_y,value_z,wait);
   else {
    if(SysNum==0) {
	ostringstream error;
	error << "HistoManager::FillHisto = Error 3D histogram num " 
              << num << " doesn't exist "<< endl;              
	string mess=error.str();      
	throw mess; 
    }
  
   }
}


void HistoManager::Fill1BinHisto(Int_t hnum,Double_t binvalue,Double_t value,Double_t wait,const Short_t SysNum)
{ 
///////////////////////////////////////////////////////////// 
//     Fill 1dim. bin histograms 
///////////////////////////////////////////////////////////// 
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 1000 "<< endl;              
    string mess=error.str();      
    throw mess; 
    return;
   }

 if(hnum>fHisto1D[SysNum]->GetLast()) return;
 TH1F* basehisto= (TH1F*)fHisto1D[SysNum]->At(hnum);
   
 if(basehisto) {
	Double_t bmin,bmax;
	for(Int_t i=1;i<=basehisto->GetBinContent(1);i++) {
		bmin=basehisto->GetBinContent(1+i);
		bmax=basehisto->GetBinContent(2+i);
//		printf("Min = %f Max = %f \n",bmin,bmax);
        	if(binvalue>bmin&&binvalue<bmax)
            		((TH1F*)fHisto1D[SysNum]->At(hnum+i))->Fill(value,wait); 
	}
 } else {
    if(SysNum==0) {
       ostringstream error;
       error << "HistoManager::Fill1BinHisto = Error bin histograms hnum " 
             << hnum << " doesn't exist "<< endl;              
       string mess=error.str();      
       throw mess; 
    }
 }
} 

void HistoManager::Fill1BinHistoD(Int_t hnum,Double_t binvalue,Double_t value,Double_t wait,const Short_t SysNum)
{ 
///////////////////////////////////////////////////////////// 
//     Fill 1dim. bin histograms 
///////////////////////////////////////////////////////////// 
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 1000 "<< endl;              
    string mess=error.str();      
    throw mess; 
    return;
   }

 if(hnum>fHisto1D_d[SysNum]->GetLast()) return;
 TH1D* basehisto= (TH1D*)fHisto1D_d[SysNum]->At(hnum);
   
 if(basehisto) {
	Double_t bmin,bmax;
	for(Int_t i=1;i<=basehisto->GetBinContent(1);i++) {
		bmin=basehisto->GetBinContent(1+i);
		bmax=basehisto->GetBinContent(2+i);
//		printf("Min = %f Max = %f \n",bmin,bmax);
        	if(binvalue>bmin&&binvalue<bmax)
            		((TH1D*)fHisto1D_d[SysNum]->At(hnum+i))->Fill(value,wait); 
	}
 } else {
    if(SysNum==0) {
       ostringstream error;
       error << "HistoManager::Fill1BinHisto = Error bin histograms hnum " 
             << hnum << " doesn't exist "<< endl;              
       string mess=error.str();      
       throw mess; 
    }
 }
}

void HistoManager::Fill1BinHisto(Int_t hnum,Double_t binvalue,Double_t value1, Double_t value2,Double_t wait,const Short_t SysNum)
{ 
///////////////////////////////////////////////////////////// 
//     Fill 1dim. bin histograms 
///////////////////////////////////////////////////////////// 
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 1000 "<< endl;              
    string mess=error.str();      
    throw mess; 
    return;
   }

 if(hnum>fHisto1D[SysNum]->GetLast()) return;
 TH1F* basehisto= (TH1F*)fHisto1D[SysNum]->At(hnum);
   
 if(basehisto) {
	Double_t bmin,bmax;
	for(Int_t i=1;i<=basehisto->GetBinContent(1);i++) {
		bmin=basehisto->GetBinContent(1+i);
		bmax=basehisto->GetBinContent(2+i);
//		printf("Min = %f Max = %f \n",bmin,bmax);
        	if(binvalue>bmin&&binvalue<bmax)
            		((TH2F*)fHisto2D[SysNum]->At(hnum+i))->Fill(value1,value2,wait); 
	}
 } else {
    if(SysNum==0) {
       ostringstream error;
       error << "HistoManager::Fill1BinHisto = Error bin histograms hnum " 
             << hnum << " doesn't exist "<< endl;              
       string mess=error.str();      
       throw mess; 
    }
 }
}




void HistoManager::Fill2BinHisto(Int_t hnum,Double_t binvalue,Double_t binvalue2,Double_t value,Double_t wait,const Short_t SysNum)
{ 
///////////////////////////////////////////////////////////// 
//     Fill 2dim. bin histograms 
///////////////////////////////////////////////////////////// 
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 20 "<< endl;              
    string mess=error.str();      
     
    throw mess; 
    return;
   }

 if(hnum>fHisto1D[SysNum]->GetLast()) return;
 TH1F* basehisto= (TH1F*)fHisto1D[SysNum]->At(hnum);
   
 Int_t numhisto=hnum;  
 if(basehisto) {
	Double_t bmin,bmax,bmin2,bmax2;
//	cout<<basehisto->GetBinContent(1)<<"*"<<basehisto->GetBinContent(2)<<endl;
	for(Int_t i=1;i<=basehisto->GetBinContent(1);i++) {
	   for(Int_t j=1;j<=basehisto->GetBinContent(2);j++) {
		bmin=basehisto->GetBinContent(2+i);
		bmax=basehisto->GetBinContent(3+i);
		bmin2=basehisto->GetBinContent((Int_t)basehisto->GetBinContent(1)+1+2+j);
		bmax2=basehisto->GetBinContent((Int_t)basehisto->GetBinContent(1)+1+3+j);
		numhisto+=1;
//		printf("%d : Min = %f Max = %f, Min2 = %f Max2 = %f\n",numhisto,bmin,bmax,bmin2,bmax2);
        	if((binvalue>bmin&&binvalue<bmax) && (binvalue2>bmin2&&binvalue2<bmax2))
            		((TH1F*)fHisto1D[SysNum]->At(numhisto))->Fill(value,wait); 
	}
      }
 	
 } else {
    if(SysNum==0) {
       ostringstream error;
       error << "HistoManager::Fill1BinHisto = Error bin histograms hnum " 
             << hnum << " doesn't exist "<< endl;              
       string mess=error.str();      
       throw mess; 
    }
 }
} 

void HistoManager::Fill2BinHisto(Int_t hnum,Double_t binvalue,Double_t binvalue2,Double_t value1,Double_t value2,Double_t wait,const Short_t SysNum)
{ 
///////////////////////////////////////////////////////////// 
//     Fill 2dim. bin histograms 
///////////////////////////////////////////////////////////// 
   if(SysNum>=1000) {
     ostringstream error;
      error << "HistoManager::FillHisto = Error Systematic num " 
          << SysNum << " >= 20 "<< endl;              
    string mess=error.str();      
     
    throw mess; 
    return;
   }

 if(hnum>fHisto1D[SysNum]->GetLast()) return;
 TH1F* basehisto= (TH1F*)fHisto1D[SysNum]->At(hnum);
   
 Int_t numhisto=hnum;  
 if(basehisto) {
	Double_t bmin,bmax,bmin2,bmax2;
//	cout<<basehisto->GetBinContent(1)<<"*"<<basehisto->GetBinContent(2)<<endl;
	for(Int_t i=1;i<=basehisto->GetBinContent(1);i++) {
	   for(Int_t j=1;j<=basehisto->GetBinContent(2);j++) {
		bmin=basehisto->GetBinContent(2+i);
		bmax=basehisto->GetBinContent(3+i);
		bmin2=basehisto->GetBinContent((Int_t)basehisto->GetBinContent(1)+1+2+j);
		bmax2=basehisto->GetBinContent((Int_t)basehisto->GetBinContent(1)+1+3+j);
		numhisto+=1;
//		printf("%d : Min = %f Max = %f, Min2 = %f Max2 = %f\n",numhisto,bmin,bmax,bmin2,bmax2);
        	if((binvalue>bmin&&binvalue<bmax) && (binvalue2>bmin2&&binvalue2<bmax2))
            		((TH2F*)fHisto2D[SysNum]->At(numhisto))->Fill(value1,value2,wait); 
	}
      }
 	
 } else {
    if(SysNum==0) {
       ostringstream error;
       error << "HistoManager::Fill1BinHisto = Error bin histograms hnum " 
             << hnum << " doesn't exist "<< endl;              
       string mess=error.str();      
       throw mess; 
    }
 }
}




void HistoManager::ReadParamsh1f(istringstream &inParams,string type, Int_t hoffset,
			const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo h1f
///////////////////////////////////////////////////////////// 

   int num=0,bin=0;
   string name,word;
   string comment="no comment";
   string space=" ";
   double min=0,max=0;

   inParams >> num >> name  >> word;
   num += hoffset;
   
   if(word=="'"){
     word=space; 
     comment=space;
     do {
       comment=comment+space+word; 
       inParams >> word;
    
     }while(word!="'"); 
   }
   if(comment.find("'")!=string::npos){
     cout << "HistoManager::ReadParameters = error in h1f " << num<< " comment format"<<endl; 
     throw string("stop! \n");
   }		     
   inParams  >>bin>>min>>max;
  

   name="h"+type+name;
   comment=comment+space+"("+type+")";
   if(SysNum==0) cout << "TH1F    " << num <<" " <<name <<" : "<<comment<<endl;
   	   
   if(SysNum==0||BookforSys) {
   	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(name.c_str(),comment.c_str(),bin,min,max); 
   	((TH1F*)fHisto1D[SysNum]->At(num))->Sumw2();
   	((TH1F*)fHisto1D[SysNum]->At(num))->StatOverflows();
   }      


}
void HistoManager::ReadParamsh1d(istringstream &inParams,string type, Int_t hoffset,
			const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo h1f
///////////////////////////////////////////////////////////// 

   int num=0,bin=0;
   string name,word;
   string comment="no comment";
   string space=" ";
   double min=0,max=0;

   inParams >> num >> name  >> word;
   num += hoffset;
   
   if(word=="'"){
     word=space; 
     comment=space;
     do {
       comment=comment+space+word; 
       inParams >> word;
    
     }while(word!="'"); 
   }
   if(comment.find("'")!=string::npos){
     cout << "HistoManager::ReadParameters = error in h1d " << num<< " comment format"<<endl; 
     throw string("stop! \n");
   }		     
   inParams  >>bin>>min>>max;
  

   name="h"+type+name;
   comment=comment+space+"("+type+")";
   if(SysNum==0) cout << "TH1D    " << num <<" " <<name <<" : "<<comment<<endl;
   	   
   if(SysNum==0||BookforSys) {
   	new (fHisto1D_d[SysNum]->AddrAt(num)) TH1D(name.c_str(),comment.c_str(),bin,min,max); 
   	((TH1D*)fHisto1D_d[SysNum]->At(num))->Sumw2();
   	((TH1D*)fHisto1D_d[SysNum]->At(num))->StatOverflows();
   }      


}

void HistoManager::ReadParamsh2f(istringstream &inParams,string type, Int_t hoffset,
				const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo h2f
///////////////////////////////////////////////////////////// 

   int num=0,bin_x=0,bin_y=0;
   string name,word;
   string comment="no comment";
   string space=" ";
   double min_x=0,max_x=0, min_y=0,max_y=0;

   inParams >> num >> name  >> word;
   num += hoffset;
   
   if(word=="'"){
     word=space; 
     comment=space;
     do {
   	comment=comment+space+word; 
   	inParams >> word;
     }while(word!="'"); 
   } 
   if(comment.find("'")!=string::npos){
     cout << "HistoManager::ReadParameters = error in h2f " << num<< " comment format"<<endl; 
     throw string("stop! \n");
   }	     
   inParams   >>bin_x>>min_x>>max_x>>bin_y>>min_y>>max_y; 

   name="h"+type+name;
   comment=comment+space+"("+type+")";

   if(SysNum==0) cout <<num <<" " << name <<": "<<comment <<endl;	   
   if(SysNum==0||BookforSys) {
	new (fHisto2D[SysNum]->AddrAt(num)) 
      		TH2F(name.c_str(),comment.c_str(),bin_x,min_x,max_x,bin_y,min_y,max_y);	
   	((TH2F*)fHisto2D[SysNum]->At(num))->Sumw2();
   	((TH2F*)fHisto2D[SysNum]->At(num))->StatOverflows();
   }

}





void HistoManager::ReadParams1binfh1f(istringstream &inParams,string type, Int_t hoffset,
					const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo 1binfh1f
///////////////////////////////////////////////////////////// 

    cout << "Binned histo with fix bin width ..."<<endl;
    int num=0,bin=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min=0,max=0;
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
   inParams  >>bin>>min>>max;
    cout <<num<<" "<<name<<" "<<comment<<" "<<bin<<" "<<min<<" "<<max<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,hnum;
    Double_t binwidth,bmin,bmax,offset;
    string binvar;
    Char_t hname[100],label[200];
    inParams >> binvar >> nbbins >> binwidth >> offset; 
    cout<<" "<<binvar<<" "<< nbbins<<" "<<binwidth<<endl; 
    sprintf(hname,"h%s%s_BIN_%s",type.c_str(),name.c_str(),binvar.c_str());
    sprintf(label,"h%s%s bin values",type.c_str(),comment.c_str());
    if(SysNum==0||BookforSys) {
	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(hname,label,nbbins+2,0,nbbins+2); //histo to store min/max of bins
        ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(1,nbbins);
    }
    for(Int_t i=0;i<nbbins;i++) {
   	  bmin=offset+i*binwidth;
	  bmax=offset+(i+1)*binwidth;
	  hnum=num+i+1;
   	  sprintf(hname,"h%s%s%d",type.c_str(),name.c_str(),i);
   	  sprintf(label,"%s (%7.2f<%s<%7.2f) (%s)",comment.c_str(),bmin,binvar.c_str(),bmax,type.c_str());
	  cout << hname <<": "<< label <<endl;  	
    	  if(SysNum==0||BookforSys) {
	  	new (fHisto1D[SysNum]->AddrAt(hnum)) TH1F(hname,label,bin,min,max);
   	  	((TH1F*)fHisto1D[SysNum]->At(hnum))->Sumw2();      
   	  	((TH1F*)fHisto1D[SysNum]->At(hnum))->StatOverflows();
	  	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+2,bmin);
   	  	if(i==nbbins-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+3,bmax);
	  }
    }

}



void HistoManager::ReadParamsh1fgeneric(istringstream &inParams,string type, Int_t hoffset,
					 list<string>& PartTags,
					 const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo h1fgeneric
///////////////////////////////////////////////////////////// 

    cout << "Generic h1f histo ..."<<endl;
    int num=0,bin=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min=0,max=0;

    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
    	comment=comment+space+word; 
    	inParams >> word;
     
      }while(word!="'"); 
    }
    if(comment.find("'")!=string::npos){
      cout << "HistoManager::ReadParameters = error in h1fgeneric " << num<< " comment format"<<endl; 
      throw string("stop! \n");
    }		      
    inParams  >>bin>>min>>max;
  
//--- loop over particles to book
   Int_t nbpart=0;
   for(list<string>::iterator it=PartTags.begin();it!=PartTags.end(); it++) {
	string& parttag=(*it);
 
    	string thisname="h"+type+name+parttag;
    	string thiscomment=comment+parttag+space+"("+type+")";
    	Int_t thisnum=num+nbpart;
	if(SysNum==0) cout << thisnum <<" " <<thisname <<" : "<<thiscomment<<endl;
    	    
    	if(SysNum==0||BookforSys) {
    		new (fHisto1D[SysNum]->AddrAt(thisnum)) TH1F(thisname.c_str(),thiscomment.c_str(),bin,min,max); 
    		((TH1F*)fHisto1D[SysNum]->At(thisnum))->Sumw2();
    		((TH1F*)fHisto1D[SysNum]->At(thisnum))->StatOverflows();
	} 
	nbpart+=1;
   }     


}




void HistoManager::ReadParams1binvh1f(istringstream &inParams,string type, Int_t hoffset,
					const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo 1binvh1f (variable width bins)
///////////////////////////////////////////////////////////// 

    cout << "Binned histo with variable bin width ..."<<endl;
    int num=0,bin=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min=0,max=0;
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
   inParams  >>bin>>min>>max;
    cout <<num<<" "<<name<<" "<<comment<<" "<<bin<<" "<<min<<" "<<max<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,hnum;
    Double_t bmin,bmax;
    string binvar;
    Char_t hname[100],label[200];
    inParams >> binvar >> nbbins;
    inParams >> bmin;
//  cout<<" "<<binvar<<" "<< nbbins<<" "<<binwidth<<endl; 
    sprintf(hname,"h%s%s_BIN_%s",type.c_str(),name.c_str(),binvar.c_str());
    sprintf(label,"h%s%s bin values",type.c_str(),comment.c_str());
    if(SysNum==0||BookforSys) {
    	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(hname,label,nbbins+2,0,nbbins+2); //histo to store min/max of bins
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(1,nbbins);
    }
    for(Int_t i=0;i<nbbins;i++) {
   	  inParams >> bmax;
	  hnum=num+i+1;
   	  sprintf(hname,"h%s%s%d",type.c_str(),name.c_str(),i);
   	  sprintf(label,"%s (%7.2f<%s<%7.2f) (%s)",comment.c_str(),bmin,binvar.c_str(),bmax,type.c_str());
	  cout << hname <<": "<< label <<endl;  	
    	  if(SysNum==0||BookforSys) {
	  	new (fHisto1D[SysNum]->AddrAt(hnum)) TH1F(hname,label,bin,min,max);
	  	((TH1F*)fHisto1D[SysNum]->At(hnum))->Sumw2();
	  	((TH1F*)fHisto1D[SysNum]->At(hnum))->StatOverflows();
   	  	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+2,bmin);
   	  	if(i==nbbins-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+3,bmax);
	  }
	  bmin=bmax;
    }


}

void HistoManager::ReadParams1binvh1d(istringstream &inParams,string type, Int_t hoffset,
					const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo 1binvh1d (variable width bins)
///////////////////////////////////////////////////////////// 

    cout << "Binned histo with variable bin width ..."<<endl;
    int num=0,bin=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min=0,max=0;
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
   inParams  >>bin>>min>>max;
    cout <<num<<" "<<name<<" "<<comment<<" "<<bin<<" "<<min<<" "<<max<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,hnum;
    Double_t bmin,bmax;
    string binvar;
    Char_t hname[100],label[200];
    inParams >> binvar >> nbbins;
    inParams >> bmin;
//  cout<<" "<<binvar<<" "<< nbbins<<" "<<binwidth<<endl; 
    sprintf(hname,"h%s%s_BIN_%s",type.c_str(),name.c_str(),binvar.c_str());
    sprintf(label,"h%s%s bin values",type.c_str(),comment.c_str());
    if(SysNum==0||BookforSys) {
    	new (fHisto1D_d[SysNum]->AddrAt(num)) TH1D(hname,label,nbbins+2,0,nbbins+2); //histo to store min/max of bins
    	((TH1D*)fHisto1D_d[SysNum]->AddrAt(num))->SetBinContent(1,nbbins);
    }
    for(Int_t i=0;i<nbbins;i++) {
   	  inParams >> bmax;
	  hnum=num+i+1;
   	  sprintf(hname,"h%s%s%d",type.c_str(),name.c_str(),i);
   	  sprintf(label,"%s (%7.2f<%s<%7.2f) (%s)",comment.c_str(),bmin,binvar.c_str(),bmax,type.c_str());
	  cout << hname <<": "<< label <<endl;  	
    	  if(SysNum==0||BookforSys) {
	  	new (fHisto1D_d[SysNum]->AddrAt(hnum)) TH1D(hname,label,bin,min,max);
	  	((TH1D*)fHisto1D_d[SysNum]->At(hnum))->Sumw2();
	  	((TH1D*)fHisto1D_d[SysNum]->At(hnum))->StatOverflows();
   	  	((TH1D*)fHisto1D_d[SysNum]->AddrAt(num))->SetBinContent(i+2,bmin);
   	  	if(i==nbbins-1) ((TH1D*)fHisto1D_d[SysNum]->AddrAt(num))->SetBinContent(i+3,bmax);
	  }
	  bmin=bmax;
    }


}

void HistoManager::ReadParams1binvh2f(istringstream &inParams,string type, Int_t hoffset,
					const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo 1binvh1f (variable width bins)
///////////////////////////////////////////////////////////// 

    cout << "Binned histo with variable bin width ..."<<endl;
    int num=0,bin1=0,bin2=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min1=0,max1=0,min2=0,max2=0;
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
   inParams  >>bin1>>min1>>max1>>bin2>>min2>>max2;
    cout <<num<<" "<<name<<" "<<comment<<" "<<bin1<<" "<<min1<<" "<<max1<<" "<<bin2<<" "<<min2<<" "<<max2<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,hnum;
    Double_t bmin,bmax;
    string binvar;
    Char_t hname[100],label[200];
    inParams >> binvar >> nbbins;
    inParams >> bmin;
//  cout<<" "<<binvar<<" "<< nbbins<<" "<<binwidth<<endl; 
    sprintf(hname,"h%s%s_BIN_%s",type.c_str(),name.c_str(),binvar.c_str());
    sprintf(label,"h%s%s bin values",type.c_str(),comment.c_str());
    if(SysNum==0||BookforSys) {
    	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(hname,label,nbbins+2,0,nbbins+2); //histo to store min/max of bins
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(1,nbbins);
    }
    for(Int_t i=0;i<nbbins;i++) {
   	  inParams >> bmax;
	  hnum=num+i+1;
   	  sprintf(hname,"h%s%s%d",type.c_str(),name.c_str(),i);
   	  sprintf(label,"%s (%7.2f<%s<%7.2f) (%s)",comment.c_str(),bmin,binvar.c_str(),bmax,type.c_str());
	  cout << hname <<": "<< label <<endl;  	
    	  if(SysNum==0||BookforSys) {
	  	new (fHisto2D[SysNum]->AddrAt(hnum)) TH2F(hname,label,bin1,min1,max1,bin2,min2,max2);
	  	((TH2F*)fHisto2D[SysNum]->At(hnum))->Sumw2();
	  	((TH2F*)fHisto2D[SysNum]->At(hnum))->StatOverflows();
   	  	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+2,bmin);
   	  	if(i==nbbins-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+3,bmax);
	  }
	  bmin=bmax;
    }


}

void HistoManager::ReadParams1binvvh1f(istringstream &inParams,string type, Int_t hoffset,
					const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo 1binvvh1f (grid with variable width bins of histograms with variable width bins)
///////////////////////////////////////////////////////////// 

    cout << "Grid with variable bin width of histograms with variable bin widths..."<<endl;
    int num=0,bin=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min=0,max=0;
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }

    // Lecture du nombre de bins
     Int_t NInputBin;
     inParams  >>NInputBin;
 
     // Boucle sur le binning 
     Double_t *BinningVar = new Double_t[NInputBin];
     Double_t iBinMin = 0;
     Double_t iBinMax = 0;
     for(Int_t iBin = 0;iBin<NInputBin;iBin++){
       inParams >> BinningVar[iBin];
     }
     iBinMin =  BinningVar[0];
     iBinMax =  BinningVar[NInputBin-1];
 
     //for(Int_t iBin = 0;iBin<NInputBin;iBin++){
     //        printf("Bin%d = %g\n",iBin,BinningVar[iBin]);
     //}
 
     // Make labels
     //Char_t hname[100],label[200];
     //sprintf(hname,"h%s%s",type.c_str(),name.c_str());
     //sprintf(label,"%s",comment.c_str());
     //cout <<num<<" "<<hname<<" "<<comment<<" ("<<NInputBin<<" bins from "<<iBinMin<<" to "<<iBinMax<< ")\n";
 


    //--- read binned variable, number of bins, bin width
    Int_t nbbins,hnum;
    Double_t bmin,bmax;
    string binvar;
    Char_t hname[100],label[200];
    inParams >> binvar >> nbbins;
    inParams >> bmin;
  //cout<<" "<<binvar<<" "<< nbbins<<" "<<bmin<<endl; 
    sprintf(hname,"h%s%s_BIN_%s",type.c_str(),name.c_str(),binvar.c_str());
    sprintf(label,"h%s%s bin values",type.c_str(),comment.c_str());
    if(SysNum==0||BookforSys) {
    	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(hname,label,nbbins+2,0,nbbins+2); //histo to store min/max of bins
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(1,nbbins);
    }

    for(Int_t i=0;i<nbbins;i++) {
        inParams >> bmax;
        hnum=num+i+1;
        sprintf(hname,"h%s%s%d",type.c_str(),name.c_str(),i);
        sprintf(label,"%s (%7.2f<%s<%7.2f) (%s)",comment.c_str(),bmin,binvar.c_str(),bmax,type.c_str());
        cout << hname <<": "<< label <<endl;  	

        if(SysNum==0||BookforSys) {
            new (fHisto1D[SysNum]->AddrAt(hnum)) TH1F(hname,label,NInputBin-1,BinningVar);
            ((TH1F*)fHisto1D[SysNum]->At(hnum))->Sumw2();
            ((TH1F*)fHisto1D[SysNum]->At(hnum))->StatOverflows();
            ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+2,bmin);
            if(i==nbbins-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+3,bmax);
        }
        bmin=bmax;
    }
    delete[] BinningVar;

}


void HistoManager::ReadParams1binVarSizeh1f(istringstream &inParams,string type, Int_t hoffset,
					     const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo 1binvbh1f (variable width bins)
///////////////////////////////////////////////////////////// 


     cout << "Histo with variable bin width ..."<<endl;
 
     // Helper vars
     int num=0;
     string name,word;
     string comment="no comment";
     string space=" ";
     
     // Lecture du numero, du nom de l'histo et du titre
     inParams >> num >> name  >> word;
     num += hoffset;
     if(word=="'"){
       word=space; 
       comment=space;
       do {
    	 comment=comment+space+word; 
    	 inParams >> word;
       }while(word!="'"); 
     }
 
     // Lecture du nombre de bins
     Int_t NInputBin;
     inParams  >>NInputBin;
 
     // Boucle sur le binning 
     Double_t *BinningVar = new Double_t[NInputBin];
     Double_t iBinMin = 0;
     Double_t iBinMax = 0;
     for(Int_t iBin = 0;iBin<NInputBin;iBin++){
       inParams >> BinningVar[iBin];
     }
     iBinMin =  BinningVar[0];
     iBinMax =  BinningVar[NInputBin-1];
 
     for(Int_t iBin = 0;iBin<NInputBin;iBin++){
       //      printf("Bin%d = %g\n",iBin,BinningVar[iBin]);
     }
 
     // Make labels
     Char_t hname[100],label[200];
     sprintf(hname,"h%s%s",type.c_str(),name.c_str());
     sprintf(label,"%s",comment.c_str());
     cout <<num<<" "<<hname<<" "<<comment<<" ("<<NInputBin<<" bins from "<<iBinMin<<" to "<<iBinMax<< ")\n";
 
     // Store histo
     if(SysNum==0||BookforSys) {
     	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(hname,label,NInputBin-1,BinningVar);
     	((TH1F*)fHisto1D[SysNum]->At(num))->Sumw2();
     	((TH1F*)fHisto1D[SysNum]->At(num))->StatOverflows();
     }

     delete[] BinningVar;

}

void HistoManager::ReadParams2binVarSizeh2f(istringstream &inParams,string type, Int_t hoffset,
					     const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for bin histo 2binvbh2f (variable width bins)
///////////////////////////////////////////////////////////// 


     cout << "Histo with variable bin width ..."<<endl;
 
     // Helper vars
     int num=0;
     string name,word;
     string comment="no comment";
     string space=" ";
     
     // Lecture du numero, du nom de l'histo et du titre
     inParams >> num >> name  >> word;
     num += hoffset;
     if(word=="'"){
       word=space; 
       comment=space;
       do {
    	 comment=comment+space+word; 
    	 inParams >> word;
       }while(word!="'"); 
     }
 
     // Lecture du nombre de bins
     Int_t NInputBinX;
     inParams  >>NInputBinX;
 
     // Boucle sur le binning 
     Double_t *BinningVarX = new Double_t[NInputBinX];
     Double_t iBinMinX = 0;
     Double_t iBinMaxX = 0;
     for(Int_t iBin = 0;iBin<NInputBinX;iBin++){
       inParams >> BinningVarX[iBin];
     }
     iBinMinX =  BinningVarX[0];
     iBinMaxX =  BinningVarX[NInputBinX-1];

     Int_t NInputBinY;
     inParams  >>NInputBinY;
 
     // Boucle sur le binning 
     Double_t *BinningVarY = new Double_t[NInputBinX];
     Double_t iBinMinY = 0;
     Double_t iBinMaxY = 0;
     for(Int_t iBin = 0;iBin<NInputBinY;iBin++){
       inParams >> BinningVarY[iBin];
     }
     iBinMinY =  BinningVarY[0];
     iBinMaxY =  BinningVarY[NInputBinY-1];
 
 
     // Make labels
     Char_t hname[100],label[200];
     sprintf(hname,"h%s%s",type.c_str(),name.c_str());
     sprintf(label,"%s",comment.c_str());
     //cout <<num<<" "<<hname<<" "<<comment<<" ("<<NInputBinX<<" bins from "<<iBinMinX<<" to "<<iBinMaxX<< ")\n";
 
     // Store histo
     if(SysNum==0||BookforSys) {
     	new (fHisto2D[SysNum]->AddrAt(num)) TH2F(hname,label,NInputBinX-1,BinningVarX, NInputBinY-1,BinningVarY);
     	((TH2F*)fHisto2D[SysNum]->At(num))->Sumw2();
     	((TH2F*)fHisto2D[SysNum]->At(num))->StatOverflows();
     }

     delete[] BinningVarX;
     delete[] BinningVarY;

}


void HistoManager::ReadParams2binfh1f(istringstream &inParams,string type, Int_t hoffset,
					const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for 2bin histo 2binfh1f
///////////////////////////////////////////////////////////// 

    cout << "Double binned histo with fix bin width ..."<<endl;
    int num=0,bin=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min=0,max=0;
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
   inParams  >>bin>>min>>max;
    cout <<num<<" "<<name<<" "<<comment<<" "<<bin<<" "<<min<<" "<<max<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,nbbins2,hnum;
    Double_t binwidth,bmin,bmax,offset;
    Double_t binwidth2,bmin2,bmax2,offset2;
    string binvar,binvar2;
    Char_t hname[100],label[200];
    inParams >> binvar >> nbbins >> binwidth >> offset;
    inParams >> binvar2 >> nbbins2 >> binwidth2 >> offset2;
    cout<<" "<<binvar<<" "<< nbbins<<" "<<binwidth<<endl; 
    cout<<" "<<binvar2<<" "<< nbbins2<<" "<<binwidth2<<endl; 

    sprintf(hname,"h%s%s_BIN_%s_%s",type.c_str(),name.c_str(),binvar.c_str(),binvar2.c_str());
    sprintf(label,"h%s%s bin values",type.c_str(),comment.c_str());
    if(SysNum==0||BookforSys) {
	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(hname,label,(nbbins+nbbins2)+3,0,(nbbins+nbbins2)+3); //histo to store min/max of bins
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(1,nbbins);
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(2,nbbins2);
    }
    hnum=num;
    for(Int_t i=0;i<nbbins;i++) {
   	bmin=offset+i*binwidth;
	bmax=offset+(i+1)*binwidth;
    	for(Int_t j=0;j<nbbins2;j++) {
   	  bmin2=offset2+j*binwidth2;
	  bmax2=offset2+(j+1)*binwidth2;
	  hnum+=1;
   	  sprintf(hname,"h%s%s%d",type.c_str(),name.c_str(),hnum-num-1);
   	  sprintf(label,"%s (%7.2f<%s<%7.2f) (%7.2f<%s<%7.2f) (%s)"
	               ,comment.c_str(),bmin,binvar.c_str(),bmax,bmin2,binvar2.c_str(),bmax2,type.c_str());
	  cout << hname <<": "<< label <<endl;  	
	  if(SysNum==0||BookforSys) {
		new (fHisto1D[SysNum]->AddrAt(hnum)) TH1F(hname,label,bin,min,max);
	  	((TH1F*)fHisto1D[SysNum]->At(hnum))->Sumw2();
	  	((TH1F*)fHisto1D[SysNum]->At(hnum))->StatOverflows();
	  }
    	}
	if(SysNum==0||BookforSys) {
		((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+3,bmin);
   		if(i==nbbins-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+4,bmax);
	}
    }
    for(Int_t j=0;j<nbbins2;j++) {
   	bmin2=offset2+j*binwidth2;
	bmax2=offset2+(j+1)*binwidth2;
	if(SysNum==0||BookforSys) {
		((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(j+nbbins+1+3,bmin2);
   		if(j==nbbins2-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(j+nbbins+1+4,bmax2);
	}
    }
}




void HistoManager::ReadParams2binvh1f(istringstream &inParams,string type, Int_t hoffset,
					const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for 2bin histo 2binvh1f variable bin width
///////////////////////////////////////////////////////////// 

    cout << "Double binned histo with variable bin width ..."<<endl;
    int num=0,bin=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min=0,max=0;
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
    inParams  >>bin>>min>>max;
    cout <<num<<" "<<name<<" "<<comment<<" "<<bin<<" "<<min<<" "<<max<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,nbbins2,hnum;
    Double_t bmin,bmax;
    Double_t bmin2,bmax2;
    string binvar,binvar2;
    Char_t hname[100],label[200];
    inParams >> binvar >> nbbins;
    Float_t *bins1=new Float_t[nbbins+1];
    inParams >> bins1[0];
    for(Int_t i=0;i<nbbins;i++) inParams >> bins1[i+1];
    inParams >> binvar2 >> nbbins2 ;
    Float_t *bins2=new Float_t[nbbins2+1];
    inParams >> bins2[0];
    for(Int_t i=0;i<nbbins2;i++) inParams >> bins2[i+1];

    cout<<" "<<binvar<<" "<< nbbins<<endl; 
    cout<<" "<<binvar2<<" "<< nbbins2<<endl; 

    sprintf(hname,"h%s%s_BIN_%s_%s",type.c_str(),name.c_str(),binvar.c_str(),binvar2.c_str());
    sprintf(label,"h%s%s bin values",type.c_str(),comment.c_str());
    if(SysNum==0||BookforSys) {
	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(hname,label,(nbbins+nbbins2)+3,0,(nbbins+nbbins2)+3); //histo to store min/max of bins
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(1,nbbins);
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(2,nbbins2);
    }
    hnum=num;
    
    
    for(Int_t i=0;i<nbbins;i++) {
   	bmin=bins1[i];
	bmax=bins1[i+1];
    	for(Int_t j=0;j<nbbins2;j++) {
   	  bmin2=bins2[j];
	  bmax2=bins2[j+1];
	  hnum+=1;
   	  sprintf(hname,"h%s%s%d",type.c_str(),name.c_str(),hnum-num-1);
   	  sprintf(label,"%s (%7.2f<%s<%7.2f) (%7.2f<%s<%7.2f) (%s)"
	               ,comment.c_str(),bmin,binvar.c_str(),bmax,bmin2,binvar2.c_str(),bmax2,type.c_str());
	  cout << hname <<": "<< label <<endl;  	
    	  if(SysNum==0||BookforSys) {
	  	new (fHisto1D[SysNum]->AddrAt(hnum)) TH1F(hname,label,bin,min,max);
	  	((TH1F*)fHisto1D[SysNum]->At(hnum))->Sumw2();
	  	((TH1F*)fHisto1D[SysNum]->At(hnum))->StatOverflows();
	  }
    	}
    	if(SysNum==0||BookforSys) {
		((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+3,bmin);
   		if(i==nbbins-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+4,bmax);
	}
    }
    for(Int_t j=0;j<nbbins2;j++) {
   	bmin2=bins2[j];
	bmax2=bins2[j+1];
    	if(SysNum==0||BookforSys) {
		((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(j+nbbins+1+3,bmin2);
   		if(j==nbbins2-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(j+nbbins+1+4,bmax2);
	}
    }

delete [] bins1;
delete [] bins2;

}

void HistoManager::ReadParams2binvh2f(istringstream &inParams,string type, Int_t hoffset,
					const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for 2bin histo 2binvh2f variable bin width
///////////////////////////////////////////////////////////// 

    cout << "Double binned histo with variable bin width ..."<<endl;
    int num=0,bin1=0,bin2=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    double min1=0,max1=0,min2=0,max2=0;
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
    inParams  >>bin1>>min1>>max1>>bin2>>min2>>max2;
    cout <<num<<" "<<name<<" "<<comment<<" "<<bin1<<" "<<min1<<" "<<max1<<" "<<bin2<<" "<<min2<<" "<<max2<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,nbbins2,hnum;
    Double_t bmin,bmax;
    Double_t bmin2,bmax2;
    string binvar,binvar2;
    Char_t hname[100],label[200];
    inParams >> binvar >> nbbins;
    Float_t *bins1=new Float_t[nbbins+1];
    inParams >> bins1[0];
    for(Int_t i=0;i<nbbins;i++) inParams >> bins1[i+1];
    inParams >> binvar2 >> nbbins2 ;
    Float_t *bins2=new Float_t[nbbins2+1];
    inParams >> bins2[0];
    for(Int_t i=0;i<nbbins2;i++) inParams >> bins2[i+1];

    cout<<" "<<binvar<<" "<< nbbins<<endl; 
    cout<<" "<<binvar2<<" "<< nbbins2<<endl; 

    sprintf(hname,"h%s%s_BIN_%s_%s",type.c_str(),name.c_str(),binvar.c_str(),binvar2.c_str());
    sprintf(label,"h%s%s bin values",type.c_str(),comment.c_str());
    if(SysNum==0||BookforSys) {
	new (fHisto1D[SysNum]->AddrAt(num)) TH1F(hname,label,(nbbins+nbbins2)+3,0,(nbbins+nbbins2)+3); //histo to store min/max of bins
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(1,nbbins);
    	((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(2,nbbins2);
    }
    hnum=num;
    
    
    for(Int_t i=0;i<nbbins;i++) {
   	bmin=bins1[i];
	bmax=bins1[i+1];
    	for(Int_t j=0;j<nbbins2;j++) {
   	  bmin2=bins2[j];
	  bmax2=bins2[j+1];
	  hnum+=1;
   	  sprintf(hname,"h%s%s%d",type.c_str(),name.c_str(),hnum-num-1);
   	  sprintf(label,"%s (%7.2f<%s<%7.2f) (%7.2f<%s<%7.2f) (%s)"
	               ,comment.c_str(),bmin,binvar.c_str(),bmax,bmin2,binvar2.c_str(),bmax2,type.c_str());
	  cout << hname <<": "<< label <<endl;  	
    	  if(SysNum==0||BookforSys) {
	  	new (fHisto2D[SysNum]->AddrAt(hnum)) TH2F(hname,label,bin1,min1,max1,bin2,min2,max2);
	  	((TH2F*)fHisto2D[SysNum]->At(hnum))->Sumw2();
	  	((TH2F*)fHisto2D[SysNum]->At(hnum))->StatOverflows();
	  }
    	}
    	if(SysNum==0||BookforSys) {
		((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+3,bmin);
   		if(i==nbbins-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(i+4,bmax);
	}
    }
    for(Int_t j=0;j<nbbins2;j++) {
   	bmin2=bins2[j];
	bmax2=bins2[j+1];
    	if(SysNum==0||BookforSys) {
		((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(j+nbbins+1+3,bmin2);
   		if(j==nbbins2-1) ((TH1F*)fHisto1D[SysNum]->AddrAt(num))->SetBinContent(j+nbbins+1+4,bmax2);
	}
    }

delete [] bins1;
delete [] bins2;

}







void HistoManager::ReadParams2dbin(istringstream &inParams,string type, Int_t hoffset,const Short_t SysNum,const Bool_t BookforSys)
{
//////////////////////////////////////////////////////////// 
//     read params for 2d bin (by B.P. used in NCCC analysis) 
///////////////////////////////////////////////////////////// 
    cout << "2d histo with double variable bin width ..."<<endl;
  
    int num=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
    //   inParams  >>bin>>min>>max;
    name="h"+type+name;
    cout <<num<<" "<<name<<" "<<comment<<" "<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,nbbins2;
    //    Double_t bmin,bmax;
    //   Double_t bmin2,bmax2;
    string binvar,binvar2;
//   Char_t hname[100],label[200];
    inParams >> binvar >> nbbins;
    Float_t *bins1=new Float_t[nbbins+1];
    inParams >> bins1[0];
    for(Int_t i=0;i<nbbins;i++) inParams >> bins1[i+1];
    inParams >> binvar2 >> nbbins2 ;
    Float_t *bins2=new Float_t[nbbins2+1];
    inParams >> bins2[0];
    for(Int_t i=0;i<nbbins2;i++) inParams >> bins2[i+1];

    cout<<" "<<binvar<<" "<< nbbins<<endl; 
    for(Int_t i=0;i<nbbins+1;i++){
      cout << " " << bins1[i];}
    cout << endl;
    cout<<" "<<binvar2<<" "<< nbbins2<<endl; 
   for(Int_t i=0;i<nbbins2+1;i++){
      cout << " " << bins2[i];}
    cout << endl;
// now create the histogram ...
    if(SysNum==0||BookforSys) {
        new (fHisto2D[SysNum]->AddrAt(num)) 
            TH2F(name.c_str(),comment.c_str(),nbbins,bins1,nbbins2,bins2);   
        ((TH2F*)fHisto2D[SysNum]->At(num))->Sumw2();
        ((TH2F*)fHisto2D[SysNum]->At(num))->StatOverflows();
    }

delete [] bins1;
delete [] bins2;

}

void HistoManager::ReadParams3dbin(istringstream &inParams,string type, Int_t hoffset,const Short_t SysNum)
{
//////////////////////////////////////////////////////////// 
//     read params for 3d bin (by B.P. used in NCCC analysis 
///////////////////////////////////////////////////////////// 
    cout << "3d histo with double variable bin width ..."<<endl;
  
    int num=0;
    string name,word;
    string comment="no comment";
    string space=" ";
    
    inParams >> num >> name  >> word;
    num += hoffset;
    if(word=="'"){
      word=space; 
      comment=space;
      do {
   	 comment=comment+space+word; 
   	 inParams >> word;
      }while(word!="'"); 
    }
    //   inParams  >>bin>>min>>max;
    name="h"+type+name;
    cout <<num<<" "<<name<<" "<<comment<<" "<<endl;

    //--- read binned variable, number of bins, bin width
    Int_t nbbins,nbbins2,nbbins3;
    //    Double_t bmin,bmax;
    //   Double_t bmin2,bmax2;
    string binvar,binvar2,binvar3;
//   Char_t hname[100],label[200];
    inParams >> binvar >> nbbins;
    Float_t *bins1=new Float_t[nbbins+1];
    inParams >> bins1[0];
    for(Int_t i=0;i<nbbins;i++) inParams >> bins1[i+1];

    inParams >> binvar2 >> nbbins2 ;
    Float_t *bins2=new Float_t[nbbins2+1];
    inParams >> bins2[0];
    for(Int_t i=0;i<nbbins2;i++) inParams >> bins2[i+1];

    inParams >> binvar3 >> nbbins3 ;
    Float_t *bins3=new Float_t[nbbins3+1];
    inParams >> bins3[0];
    for(Int_t i=0;i<nbbins3;i++) inParams >> bins3[i+1];


    cout<<" "<<binvar<<" "<< nbbins<<endl; 
    for(Int_t i=0;i<nbbins+1;i++){
      cout << " " << bins1[i];}
    cout << endl;
    cout<<" "<<binvar2<<" "<< nbbins2<<endl; 
   for(Int_t i=0;i<nbbins2+1;i++){
      cout << " " << bins2[i];}
    cout << endl;
    cout<<" "<<binvar3<<" "<< nbbins3<<endl; 
   for(Int_t i=0;i<nbbins3+1;i++){
      cout << " " << bins3[i];}
// now create the histogram ...
    new (fHisto3D[SysNum]->AddrAt(num)) 
             TH3F(name.c_str(),comment.c_str(),nbbins,bins1,nbbins2,bins2,nbbins3,bins3);   
          ((TH3F*)fHisto3D[SysNum]->At(num))->Sumw2();
          ((TH3F*)fHisto3D[SysNum]->At(num))->StatOverflows();


delete [] bins1;
delete [] bins2;
delete [] bins3;

}


void HistoManager::ReadParamsNtuple(istringstream &inParams,string type, Int_t hoffset,
			const Short_t SysNum,const Bool_t BookforSys)
{
///////////////////////////////////////////////////////////// 
//     read params for ntuples (by GB)
///////////////////////////////////////////////////////////// 

   int num=0;
   string name;
   string word;
   string comment = "no comment";
   string space = " ";

   inParams >> num >> name >> word;
   num += hoffset;

   if ( SysNum == 0 ) {
     if (num<fNtBuffer.GetSize()) {
       cout << "<HistoManager::ReadParamsNtuple> Ntuple '"<<name<<"' id must be above " << fNtBuffer.GetSize() << endl;
       throw string("stop! \n");
     }
   }  
   
   if ( word == "'" ) {
     word = space; 
     comment = space;
     do {
       comment = comment+space+word; 
       inParams >> word;    
     } while ( word != "'" ); 
   }
   
   if ( comment.find("'") != string::npos ) {
     cout << "<HistoManager::ReadParamsNtuple> Error in ntuple " << num << " comment format!" << endl;
     throw string("stop! \n");
   }		 
   
   string varlist;
   inParams >> varlist;
  
   name = "nt"+type+name;
   comment = comment+space+"("+type+")";
   
   if ( SysNum == 0 ) cout << "TNtuple " << num << " " <<name << " : " << comment << endl; 
   
   // add standard branches       	   
   varlist = "Run:Event:Weight:"+varlist;

   if ( SysNum == 0 || BookforSys ) {
     new ( fNtuple[SysNum]->AddrAt(num) 
         ) TNtupleD( name.c_str(), comment.c_str(), varlist.c_str()); 
   }      
   
   //create buffers only for central value
   if ( SysNum ) return;

   TNtupleD* nt = static_cast<TNtupleD*>(fNtuple[SysNum]->At(num));
   
   if ( !nt ) {
     cout << "<HistoManager::ReadParamsNtuple> Ntuple " 
          << num << " could not be created on array!" << endl; 
     throw string("stop!\n");
   }
   
   // check varlist - each must correspond to a previously defined 1d histo
   // the list of histos are associated to the ntuple
           
   Int_t      nvar = nt->GetNvar();
   TObjArray *vars = nt->GetListOfBranches();

   if ( fNtBuffer.GetSize() < num+nvar+1 ) { fNtBuffer.Set(num+nvar+1); }

   for (Int_t i=3; i<nvar; i++) {
     TBranch *branch = static_cast<TBranch*>(vars->At(i));
     const char *name = branch->GetName();
     string hname = "h"+type+name;

     TH1F* histo= static_cast<TH1F*>(fHisto1D[SysNum]->FindObject(hname.c_str()));

     if ( !histo ) {
       cout << "<HistoManager::ReadParamsNtuple> Could not find histo for branch " << name << endl;
       throw string(" stop!\n");
     }   
     
     Int_t hnum = fHisto1D[SysNum]->IndexOf(histo);

     if ( fNtBufSlot.GetSize() < hnum+1 ) { fNtBufSlot.Set(hnum+1); }

     if(fNtBufSlot[hnum]){
       cout << "<HistoManager::ReadParamsNtuple> Histo "
            <<hnum<<" already fills nt buffer slot "<<fNtBufSlot[hnum]<<endl;
       throw string("fix your ntuple definitions!\n");
     }

     fNtBufSlot[hnum] = num+i;   

     cout << " -> Branch '" << name << "' <- \t" 
          << hnum << " " << histo->GetName() //<< ", buffer slot " << num+i
          << endl;          
   }  
   
   //cout << "histo -> nt buffer slots" << fNtBufSlot.GetSize() << endl;
   //cout << "nt buffer size          " << fNtBuffer.GetSize() << endl;
}


void HistoManager::Scale1dHisto(Int_t hnum,Float_t ScaleParam,const Short_t SysNum)
{
// Scale 1D histogram

  // Get histogram number hnum
  TH1F *Histo = (TH1F *)fHisto1D[SysNum]->At(hnum);
  Histo->Scale(ScaleParam);
}


bool HistoManager::areInTheSameBin(const int&  numhisto,const double& x1,const double& x2,const Short_t SysNum)
{
//--- return true if both events are in the same bin

 bool result=false;

//--- init the bin numbers
int numx1=0;
int numx2=0;

//--- get a pointer to the histo
TH1F* histo= (TH1F*)fHisto1D[SysNum]->At(numhisto); //--- Take care ! here put SysNum ? for systematics ...
 

if(histo != NULL){
    numx1=histo->GetXaxis()->FindBin(x1);
    numx2=histo->GetXaxis()->FindBin(x2);

} else {
    if(SysNum==0) {
       ostringstream error;
       error << "HistoManager::AreInTheSameBin = Error 1D histogram num " 
	     << numhisto << " doesn't exist "<< endl;              
       string mess=error.str();      
       throw mess; 
     }
}


 if ( numx1==numx2 && numx1 >0 ) {
     result=true;
 }else{
     result=false;
 }

 return result;

}

const TH1F* HistoManager::getHisto1D(int numhisto, const Short_t SysNum)
{
    TH1F* histo= (TH1F*)fHisto1D[SysNum]->At(numhisto);
    if(!histo)
    {
       ostringstream error;
       error << "HistoManager::getHisto1D = Error 1D histogram num " 
	     << numhisto << " doesn't exist "<< endl;              
       string mess=error.str();      
       throw mess; 
     }
    return histo;
}

const TH2F* HistoManager::getHisto2D(int numhisto, const Short_t SysNum)
{
    TH2F* histo= (TH2F*)fHisto2D[SysNum]->At(numhisto);
    if(!histo)
    {
       ostringstream error;
       error << "HistoManager::getHisto1D = Error 2D histogram num " 
	     << numhisto << " doesn't exist "<< endl;              
       string mess=error.str();      
       throw mess; 
     }
    return histo;
}
