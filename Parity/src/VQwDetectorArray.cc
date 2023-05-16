/**********************************************************\
* File: VQwDetectorArray.cc                          *
*                                                          *
* Author: Kevin Ward (Original Code by P. M. King)                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#include "VQwDetectorArray.h"

// System headers
#include <sstream>

// Qweak headers
#include "QwSubsystemArray.h"
#include "QwLog.h"
#ifdef __USE_DATABASE__
#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif
#include "QwPromptSummary.h"

/**
 * Defines configuration options for QwEventBuffer class using QwOptions
 * functionality.
 *
 * @param options Options object
 */

void VQwDetectorArray::DefineOptions(QwOptions &options){
  // Define the execution options
  options.AddOptions()
    ("QwDetectorArray.normalize",
     po::value<bool>()->default_bool_value(true),
     "Normalize the detectors by beam current");

  options.AddOptions()
    ("QwDetectorArray.norm_threshold",
     po::value<double>()->default_value(2.5),
     "Normalize the detectors for currents above this value");
}

/*!
 * Loads the configuration options into this instance of
 * VQwDetectorArray from the QwOptions object.
 *
 * @param options Options object
 */

void VQwDetectorArray::ProcessOptions(QwOptions &options) {

    bNormalization = options.GetValue<bool>("QwDetectorArray.normalize");

    if (! bNormalization) {

        QwWarning << "QwDetectorArray::ProcessOptions:  "
	     << "Detector yields WILL NOT be normalized."
         << QwLog::endl;

    }

  fNormThreshold = options.GetValue<double>("QwDetectorArray.norm_threshold");

}


//*****************************************************************//
/**
 * Publish internal values
 * @return
 */

Bool_t VQwDetectorArray::PublishInternalValues() const {

  // Publish variables

  Bool_t status = kTRUE;

/*
  status = status && PublishInternalValue("qwk_md1neg", "qwk_md1neg", GetIntegrationPMT("qwk_md1neg")->GetChannel("qwk_md1neg"));
  status = status && PublishInternalValue("qwk_md1pos", "qwk_md1pos", GetIntegrationPMT("qwk_md1pos")->GetChannel("qwk_md1pos"));
  status = status && PublishInternalValue("qwk_md2neg", "qwk_md2neg", GetIntegrationPMT("qwk_md2neg")->GetChannel("qwk_md2neg"));
  status = status && PublishInternalValue("qwk_md2pos", "qwk_md2pos", GetIntegrationPMT("qwk_md2pos")->GetChannel("qwk_md2pos"));
  status = status && PublishInternalValue("qwk_md3neg", "qwk_md3neg", GetIntegrationPMT("qwk_md3neg")->GetChannel("qwk_md3neg"));
  status = status && PublishInternalValue("qwk_md3pos", "qwk_md3pos", GetIntegrationPMT("qwk_md3pos")->GetChannel("qwk_md3pos"));
  status = status && PublishInternalValue("qwk_md4neg", "qwk_md4neg", GetIntegrationPMT("qwk_md4neg")->GetChannel("qwk_md4neg"));
  status = status && PublishInternalValue("qwk_md4pos", "qwk_md4pos", GetIntegrationPMT("qwk_md4pos")->GetChannel("qwk_md4pos"));
  status = status && PublishInternalValue("qwk_md5neg", "qwk_md5neg", GetIntegrationPMT("qwk_md5neg")->GetChannel("qwk_md5neg"));
  status = status && PublishInternalValue("qwk_md5pos", "qwk_md5pos", GetIntegrationPMT("qwk_md5pos")->GetChannel("qwk_md5pos"));
  status = status && PublishInternalValue("qwk_md6neg", "qwk_md6neg", GetIntegrationPMT("qwk_md6neg")->GetChannel("qwk_md6neg"));
  status = status && PublishInternalValue("qwk_md6pos", "qwk_md6pos", GetIntegrationPMT("qwk_md6pos")->GetChannel("qwk_md6pos"));
  status = status && PublishInternalValue("qwk_md7neg", "qwk_md7neg", GetIntegrationPMT("qwk_md7neg")->GetChannel("qwk_md7neg"));
  status = status && PublishInternalValue("qwk_md7pos", "qwk_md7pos", GetIntegrationPMT("qwk_md7pos")->GetChannel("qwk_md7pos"));
  status = status && PublishInternalValue("qwk_md8neg", "qwk_md8neg", GetIntegrationPMT("qwk_md8neg")->GetChannel("qwk_md8neg"));
  status = status && PublishInternalValue("qwk_md8pos", "qwk_md8pos", GetIntegrationPMT("qwk_md8pos")->GetChannel("qwk_md8pos"));
  status = status && PublishInternalValue("qwk_md9neg", "qwk_md9neg", GetIntegrationPMT("qwk_md9neg")->GetChannel("qwk_md9neg"));
  status = status && PublishInternalValue("qwk_md9pos", "qwk_md9pos", GetIntegrationPMT("qwk_md9pos")->GetChannel("qwk_md9pos"));
*/

/*
  status = status && PublishInternalValue("qwk_md1barsum","qwk_md1barsum", GetCombinedPMT("qwk_md1barsum")->GetChannel("qwk_md1barsum"));
  status = status && PublishInternalValue("qwk_md2barsum","qwk_md2barsum", GetCombinedPMT("qwk_md2barsum")->GetChannel("qwk_md2barsum"));
  status = status && PublishInternalValue("qwk_md3barsum","qwk_md3barsum", GetCombinedPMT("qwk_md3barsum")->GetChannel("qwk_md3barsum"));
  status = status && PublishInternalValue("qwk_md4barsum","qwk_md4barsum", GetCombinedPMT("qwk_md4barsum")->GetChannel("qwk_md4barsum"));
  status = status && PublishInternalValue("qwk_md5barsum","qwk_md5barsum", GetCombinedPMT("qwk_md5barsum")->GetChannel("qwk_md5barsum"));
  status = status && PublishInternalValue("qwk_md6barsum","qwk_md6barsum", GetCombinedPMT("qwk_md6barsum")->GetChannel("qwk_md6barsum"));
  status = status && PublishInternalValue("qwk_md7barsum","qwk_md7barsum", GetCombinedPMT("qwk_md7barsum")->GetChannel("qwk_md7barsum"));
  status = status && PublishInternalValue("qwk_md8barsum","qwk_md8barsum", GetCombinedPMT("qwk_md8barsum")->GetChannel("qwk_md8barsum"));
 
  status = status && PublishInternalValue("qwk_mdallbars","qwk_mdallbars", GetCombinedPMT("qwk_mdallbars")->GetChannel("qwk_mdallbars"));
*/

  //return status;


  // TODO:
  // The variables should be published based on the parameter file.
  // See QwBeamLine class for an implementation.

  // Publish variables through map file
  // This should work with bcm, bpmstripline, bpmcavity, combo bpm and combo bcm

    for (size_t pp = 0; pp < fPublishList.size(); pp++) {

        TString publish_name = fPublishList.at(pp).at(0);
        TString device_type = fPublishList.at(pp).at(1);
        TString device_name = fPublishList.at(pp).at(2);
        TString device_prop = fPublishList.at(pp).at(3);
        device_type.ToLower();
        device_prop.ToLower();

        const VQwHardwareChannel* tmp_channel;

        if (device_type == "integrationpmt") {

            tmp_channel = GetIntegrationPMT(device_name)->GetChannel(device_name);

        } else if (device_type == "combinedpmt") {

            tmp_channel = GetCombinedPMT(device_name)->GetChannel(device_name);

        } else {

            QwError << "QwBeamLine::PublishInternalValues() error "<< QwLog::endl;

        }

        if (tmp_channel == NULL) {

            QwError << "QwBeamLine::PublishInternalValues(): " << publish_name << " not found" << QwLog::endl;
             status |= kFALSE;

        } else {

            QwDebug << "QwBeamLine::PublishInternalValues(): " << publish_name << " found" << QwLog::endl;

        }

        status = status && PublishInternalValue(publish_name, publish_name, tmp_channel);

    }

    return status;

}


Bool_t VQwDetectorArray::PublishByRequest(TString device_name) {

    Bool_t status = kFALSE;
    //std::cerr << "#####   device_name==\"" << device_name << "\"" << std::endl;
  
    for(size_t i=0;i<fMainDetID.size();i++) {

        //std::cerr << "fMainDetID[i].fdetectorname==\"" << fMainDetID[i].fdetectorname << "\"" << std::endl;

        if(device_name.CompareTo(fMainDetID[i].fdetectorname)!=0) continue;
    
        if (fMainDetID[i].fTypeID == kQwCombinedPMT){

            status = PublishInternalValue(device_name, "published-by-request",
			 fCombinedPMT[fMainDetID[i].fIndex].GetChannel(device_name));

        } else if (fMainDetID[i].fTypeID == kQwIntegrationPMT) {

            status = PublishInternalValue(device_name, "published-by-request",
			 fIntegrationPMT[fMainDetID[i].fIndex].GetChannel(device_name));

        } else {

            QwError << "Unknown channel name:  " << device_name << QwLog::endl;

        }

        break;

    }

    if (!status)  
     QwDebug << "VQwDetectorArray::PublishByRequest:  Failed to publish channel name:  " << device_name << QwLog::endl;

    return status;

}


//*****************************************************************//
Int_t VQwDetectorArray::LoadChannelMap(TString mapfile) {
     
    Bool_t ldebug=kFALSE;

    std::vector<TString> combinedchannelnames;
    std::vector<Double_t> weight;
    Int_t wordsofar=0;
    Int_t currentsubbankindex=-1;
    Int_t sample_size=0;
    Double_t abs_saturation_limit = 8.5; // default saturation limit(volt)
    Bool_t bAssignedLimit = kFALSE;

    // Open the file
    QwParameterFile mapstr(mapfile.Data());
    TString varname, varvalue;

    fDetectorMaps.insert(mapstr.GetParamFileNameContents());
    mapstr.EnableGreediness();
    mapstr.SetCommentChars("!");

    UInt_t value;
    size_t vqwk_buffer_offset = 0;

    while (mapstr.ReadNextLine()) {

        RegisterRocBankMarker(mapstr);
        if (mapstr.PopValue("abs_saturation_limit",value)) {
	        abs_saturation_limit=value;
	        bAssignedLimit = kTRUE;
        }

        if (mapstr.PopValue("sample_size",value)) {
	        sample_size=value;   
        }
    
        if (mapstr.PopValue("vqwk_buffer_offset",value)) {
            vqwk_buffer_offset=value;    
        }
      
        mapstr.TrimComment('!');   // Remove everything after a '!' character.
        mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    
        if (mapstr.LineIsEmpty())  continue;

        Bool_t  lineok   = kTRUE;
	    TString keyword  = "";
	    TString keyword2 = "";
        TString modtype  = "";
        TString dettype  = "";
	    TString namech   = "";
        Int_t modnum     = 0;
	    Int_t channum    = 0;

	    modtype = mapstr.GetTypedNextToken<TString>();	// module type

        modtype.ToUpper();

        if (modtype == "VPMT") {

            channum       = mapstr.GetTypedNextToken<Int_t>();	//channel number
            Int_t combinedchans = mapstr.GetTypedNextToken<Int_t>();	//number of combined channels
            dettype     = mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
            dettype.ToLower();
            namech      = mapstr.GetTypedNextToken<TString>();  //name of the detector
            namech.ToLower();
            combinedchannelnames.clear();
        
            for (int i=0; i<combinedchans; i++){
        
                TString nameofcombinedchan = mapstr.GetTypedNextToken<TString>();
                nameofcombinedchan.ToLower();
                combinedchannelnames.push_back(nameofcombinedchan);        
            }
        
            weight.clear();
        
            for (int i=0; i<combinedchans; i++) {
        
                weight.push_back( mapstr.GetTypedNextToken<Double_t>());        
            }
	    
            keyword  = mapstr.GetTypedNextToken<TString>();
	        keyword.ToLower();
	        keyword2 = mapstr.GetTypedNextToken<TString>();
	        keyword2.ToLower();

        } else {
    
            modnum    = mapstr.GetTypedNextToken<Int_t>();	//slot number
            channum   = mapstr.GetTypedNextToken<Int_t>();	//channel number
            dettype = mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
            dettype.ToLower();
            namech  = mapstr.GetTypedNextToken<TString>();  //name of the detector
            namech.ToLower();

	        keyword   = mapstr.GetTypedNextToken<TString>();
	        keyword.ToLower();
	        keyword2  = mapstr.GetTypedNextToken<TString>();
	        keyword2.ToLower();    
        }


        if (currentsubbankindex!=GetSubbankIndex(fCurrentROC_ID,fCurrentBank_ID)) {

            currentsubbankindex=GetSubbankIndex(fCurrentROC_ID,fCurrentBank_ID);
            wordsofar=0;
        }

        QwDetectorArrayID localMainDetID;
        localMainDetID.fdetectorname=namech;
        localMainDetID.fmoduletype=modtype;
        localMainDetID.fSubbankIndex=currentsubbankindex;
        localMainDetID.fdetectortype=dettype;

	    //localMainDetID.fWordInSubbank=wordsofar;

        if (modtype=="MOLLERADC") {

	        Int_t offset = QwMollerADC_Channel::GetBufferOffset(modnum, channum)+vqwk_buffer_offset;

	        if (offset>=0){

	            localMainDetID.fWordInSubbank = wordsofar + offset;
	        }

	    } else if (modtype=="VPMT") {

            localMainDetID.fCombinedChannelNames = combinedchannelnames;
            localMainDetID.fWeight = weight;
        
            //std::cout<<"Add in a combined channel"<<std::endl;
        } else {

            QwError << "VQwDetectorArray::LoadChannelMap:  Unknown module type: "
		     << modtype <<", the detector "<<namech<<" will not be decoded "
		     << QwLog::endl;
            lineok=kFALSE;
            continue;
        }

        localMainDetID.fTypeID=GetDetectorTypeID(dettype);

	    if (localMainDetID.fTypeID==kQwUnknownPMT) {

	        QwError << "VQwDetectorArray::LoadChannelMap:  Unknown detector type: "
		     << dettype <<", the detector "<<namech<<" will not be decoded "
		     << QwLog::endl;
	        lineok=kFALSE;
	        continue;
	    }

        localMainDetID.fIndex= GetDetectorIndex(localMainDetID.fTypeID,
        localMainDetID.fdetectorname);

        if (localMainDetID.fIndex==-1){

            if (localMainDetID.fTypeID==kQwIntegrationPMT){

                QwIntegrationPMT localIntegrationPMT(GetName(),localMainDetID.fdetectorname);

		        if (keyword=="not_blindable" || keyword2=="not_blindable")
		         localIntegrationPMT.SetBlindability(kFALSE);

		        else 
		         localIntegrationPMT.SetBlindability(kTRUE);

		        if (keyword=="not_normalizable" || keyword2=="not_normalizable")
		  	     localIntegrationPMT.SetNormalizability(kFALSE);

		        else
		  	     localIntegrationPMT.SetNormalizability(kTRUE);

		        fIntegrationPMT.push_back(localIntegrationPMT);
                fIntegrationPMT[fIntegrationPMT.size()-1].SetDefaultSampleSize(sample_size);

		        if(bAssignedLimit)
		         fIntegrationPMT[fIntegrationPMT.size()-1].SetSaturationLimit(abs_saturation_limit);

		        localMainDetID.fIndex=fIntegrationPMT.size()-1;

            } else if (localMainDetID.fTypeID==kQwCombinedPMT) {

		        QwCombinedPMT localcombinedPMT(GetName(),localMainDetID.fdetectorname);

		        if (keyword=="not_normalizable" || keyword2=="not_normalizable")
		         localcombinedPMT.SetNormalizability(kFALSE);

		        else
		         localcombinedPMT.SetNormalizability(kTRUE);

		        if (keyword=="not_blindable" || keyword2 =="not_blindable") 
		         localcombinedPMT.SetBlindability(kFALSE);

		        else 
		         localcombinedPMT.SetBlindability(kTRUE);

                fCombinedPMT.push_back(localcombinedPMT);
                fCombinedPMT[fCombinedPMT.size()-1].SetDefaultSampleSize(sample_size);
                localMainDetID.fIndex=fCombinedPMT.size()-1;
            }
        }

        if (ldebug) {

            localMainDetID.Print();
            std::cout<<"line ok=";

            if (lineok) 
             std::cout<<"TRUE"<<std::endl;

            else
             std::cout<<"FALSE"<<std::endl;
        }

        if (lineok)
         fMainDetID.push_back(localMainDetID);

    } // End of "while (mapstr.ReadNextLine())"

    for (size_t i=0; i<fMainDetID.size(); i++) {

        if (fMainDetID[i].fTypeID==kQwCombinedPMT) {

            Int_t ind = fMainDetID[i].fIndex;

            //check to see if all required channels are available
            if (ldebug) {

                std::cout<<"fMainDetID[i].fCombinedChannelNames.size()="
                 <<fMainDetID[i].fCombinedChannelNames.size()<<std::endl<<"name list: ";

                for (size_t n=0; n<fMainDetID[i].fCombinedChannelNames.size(); n++)
                 std::cout<<"  "<<fMainDetID[i].fCombinedChannelNames[n];

                std::cout<<std::endl;
            }

            Int_t chanmatched=0;

            for (size_t j=0; j<fMainDetID[i].fCombinedChannelNames.size(); j++) {

                for (size_t k=0; k<fMainDetID.size(); k++) {

                    if (fMainDetID[i].fCombinedChannelNames[j]==fMainDetID[k].fdetectorname) {

                        if (ldebug)
                         std::cout<<"found a to-be-combined channel candidate"<<std::endl;

                        chanmatched ++;
                        break;
                    }
                }
            }

            if ((Int_t) fMainDetID[i].fCombinedChannelNames.size()==chanmatched) {

                for (size_t l=0; l<fMainDetID[i].fCombinedChannelNames.size(); l++) {

                    Int_t ind_pmt = GetDetectorIndex(GetDetectorTypeID("integrationpmt"),
                    fMainDetID[i].fCombinedChannelNames[l]);

                    fCombinedPMT[ind].Add(&fIntegrationPMT[ind_pmt],fMainDetID[i].fWeight[l]);
                }

                fCombinedPMT[ind].LinkChannel(fMainDetID[i].fdetectorname);

                if (ldebug)
                 std::cout<<"linked a combined channel"<<std::endl;
            } else {

                std::cerr<<"cannot combine void channels for "<<fMainDetID[i].fdetectorname<<std::endl;
                fMainDetID[i].fIndex = -1;
                continue;
            }
        }
    }


     // Now load the variables to publish
    mapstr.RewindToFileStart();
    QwParameterFile *section;
    std::vector<TString> publishinfo;
    while ((section = mapstr.ReadNextSection(varvalue))) {

        if (varvalue == "PUBLISH") {

            fPublishList.clear();

            while (section->ReadNextLine()) {

                section->TrimComment(); // Remove everything after a comment character
                section->TrimWhitespace(); // Get rid of leading and trailing spaces

                for (int ii = 0; ii < 4; ii++) {

                    varvalue = section->GetNextToken().c_str();

                    if (varvalue.Length()) {
                    
                        publishinfo.push_back(varvalue);                    
                    }
                }

                if (publishinfo.size() == 4)
                 fPublishList.push_back(publishinfo);

                publishinfo.clear();
            }
        }
    }

    // Print list of variables to publish
    if (fPublishList.size()>0){

        QwMessage << "Variables to publish:" << QwLog::endl;

        for (size_t jj = 0; jj < fPublishList.size(); jj++)
         QwMessage << fPublishList.at(jj).at(0) << " " << fPublishList.at(jj).at(1) << " "
		  << fPublishList.at(jj).at(2) << " " << fPublishList.at(jj).at(3) << QwLog::endl;
    }

    if (ldebug) {

        std::cout<<"Done with Load channel map\n";

        for (size_t i=0;i<fMainDetID.size();i++)
         if (fMainDetID[i].fIndex>=0)
          fMainDetID[i].Print();
    }

    ldebug=kFALSE;
    mapstr.Close(); // Close the file (ifstream)
    return 0;
}

void VQwDetectorArray::LoadEventCuts_Line(QwParameterFile &mapstr, TString &varvalue, Int_t &eventcut_flag) {
    TString device_type = mapstr.GetTypedNextToken<TString>();
    device_type.ToLower();
    TString device_name = mapstr.GetTypedNextToken<TString>();
     device_name.ToLower();

    Int_t det_index = GetDetectorIndex(GetDetectorTypeID(device_type),device_name);
	if (det_index == -1) {
	    QwWarning << " Device not found " << device_name << " of type " << device_type << QwLog::endl;
        //continue;
	}

    Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for IntegrationPMT value
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for IntegrationPMT value
	varvalue = mapstr.GetTypedNextToken<TString>();//global/local
    varvalue.ToLower();

    Double_t burplevel = mapstr.GetTypedNextToken<Double_t>();
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();

    if (device_type == GetQwPMTInstrumentTypeName(kQwIntegrationPMT)){
	    QwMessage << "VQwDetectorArray Error Code passing to QwIntegrationPMT " << GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut) << QwLog::endl;
	    fIntegrationPMT[det_index].SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut),LLX,ULX,stabilitycut,burplevel);
	} else if (device_type == GetQwPMTInstrumentTypeName(kQwCombinedPMT)){
	    QwMessage << "VQwDetectorArray Error Code passing to QwCombinedPMT " << GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut) << QwLog::endl;
	    fCombinedPMT[det_index].SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut),LLX,ULX,stabilitycut,burplevel);
	}
}

void VQwDetectorArray::LoadEventCuts_Fin(Int_t &eventcut_flag) {
    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
     fIntegrationPMT[i].SetEventCutMode(eventcut_flag);
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
     fCombinedPMT[i].SetEventCutMode(eventcut_flag); 

    fMainDetErrorCount = 0;//set the error counter to zero
}



Int_t VQwDetectorArray::LoadInputParameters(TString pedestalfile) {

    Bool_t ldebug=kFALSE;
    TString varname;
    Double_t varped;
    Double_t varcal;

    //  Double_t varbaserate;
    Double_t varnormrate;
    Double_t varvoltperhz;
    Double_t varasym;
    Double_t varcx;
    Double_t varcy;
    Double_t varcxp;
    Double_t varcyp;
    Double_t varce;

    TString localname;

    Int_t lineread=0;

    QwParameterFile mapstr(pedestalfile.Data());  //Open the file
    fDetectorMaps.insert(mapstr.GetParamFileNameContents());

    while (mapstr.ReadNextLine()) {

        lineread+=1;
        if (ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
         mapstr.TrimComment('!');   // Remove everything after a '!' character.
      
        mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      
        if (mapstr.LineIsEmpty())  continue;

        else {
            varname = mapstr.GetTypedNextToken<TString>();	//name of the channel
            varname.ToLower();
            varname.Remove(TString::kBoth,' ');
            varped  = mapstr.GetTypedNextToken<Double_t>(); // value of the pedestal
            varcal  = mapstr.GetTypedNextToken<Double_t>(); // value of the calibration factor

            varnormrate  = mapstr.GetTypedNextToken<Double_t>(); // value of the NormRate
            varvoltperhz = mapstr.GetTypedNextToken<Double_t>(); // value of the VoltPerHz
            varasym      = mapstr.GetTypedNextToken<Double_t>(); // value of the asymmetry
            varcx        = mapstr.GetTypedNextToken<Double_t>(); // value of the coefficient C_x
            varcy        = mapstr.GetTypedNextToken<Double_t>(); // value of the coefficient C_y 
            varcxp       = mapstr.GetTypedNextToken<Double_t>(); // value of the coefficient C_xp
            varcyp       = mapstr.GetTypedNextToken<Double_t>(); // value of the coefficient C_yp
            varce        = mapstr.GetTypedNextToken<Double_t>(); // value of the coefficient C_e


            if (ldebug) 
             std::cout << "Inputs for channel " << varname      << ": ped="  << varped  << ": cal=" << varcal << "\n"
                       << ": varnormrate="      << varnormrate  << "\n"
                       << ": varvoltperhz="     << varvoltperhz << "\n"
                       << ": asym="             << varasym      << "\n"
                       << ": C_x="              << varcx        << ": C_y="  << varcy   << "\n"
                       << ": C_xp="             << varcxp       << ": C_yp=" << varcyp  << "\n"
                       << ": C_e="              << varce        << "\n";

            Bool_t notfound=kTRUE;

            if (notfound)
             for (size_t i=0;i<fIntegrationPMT.size();i++)
              if (fIntegrationPMT[i].GetElementName()==varname) {

                fIntegrationPMT[i].SetPedestal(varped);
                fIntegrationPMT[i].SetCalibrationFactor(varcal);

                fIntegrationPMT[i].SetNormRate(varnormrate);
                fIntegrationPMT[i].SetVoltPerHz(varvoltperhz);
                fIntegrationPMT[i].SetAsymmetry(varasym);
                fIntegrationPMT[i].SetCoefficientCx(varcx);
                fIntegrationPMT[i].SetCoefficientCy(varcy);
                fIntegrationPMT[i].SetCoefficientCxp(varcxp);
                fIntegrationPMT[i].SetCoefficientCyp(varcyp);
                fIntegrationPMT[i].SetCoefficientCe(varce);

                i=fIntegrationPMT.size()+1;
                notfound=kFALSE;
                i=fIntegrationPMT.size()+1;
                
              }

        }

    }

    if (ldebug) 
     std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

    ldebug=kFALSE;
    mapstr.Close(); // Close the file (ifstream)
    return 0;

}


Bool_t VQwDetectorArray::IsGoodEvent() {

  Bool_t test=kTRUE;
  return test;

}

void VQwDetectorArray::ClearEventData() {

    for (size_t i=0;i<fIntegrationPMT.size();i++){
    
        fIntegrationPMT[i].ClearEventData();
    
    }

    for (size_t i=0;i<fCombinedPMT.size();i++)
     fCombinedPMT[i].ClearEventData();

    return;

}


/********************************************************/

void VQwDetectorArray::SetRandomEventParameters(Double_t mean, Double_t sigma) {

    for (size_t i = 0; i < fMainDetID.size(); i++) {

        // This is a QwIntegrationPMT
        if (fMainDetID.at(i).fTypeID == kQwIntegrationPMT)
         fIntegrationPMT[fMainDetID.at(i).fIndex].SetRandomEventParameters(mean, sigma);
    
    }

}

void VQwDetectorArray::SetRandomEventAsymmetry(Double_t asymmetry) {

    for (size_t i = 0; i < fMainDetID.size(); i++) {

        // This is a QwIntegrationPMT
        if (fMainDetID.at(i).fTypeID == kQwIntegrationPMT)
         fIntegrationPMT[fMainDetID.at(i).fIndex].SetRandomEventAsymmetry(asymmetry);
    
    }

}

void VQwDetectorArray::RandomizeEventData(int helicity, double time) {

    for (size_t i = 0; i < fMainDetID.size(); i++) {

        // This is a QwIntegrationPMT
        if (fMainDetID.at(i).fTypeID == kQwIntegrationPMT)
         fIntegrationPMT[fMainDetID.at(i).fIndex].RandomizeEventData(helicity, time);
    
    }

}

void VQwDetectorArray::EncodeEventData(std::vector<UInt_t> &buffer) {

    std::vector<UInt_t> elements;
    elements.clear();

    // Get all buffers in the order they are defined in the map file
    for (size_t i = 0; i < fMainDetID.size(); i++) {

        // This is a QwIntegrationPMT
        if (fMainDetID.at(i).fTypeID == kQwIntegrationPMT)
         fIntegrationPMT[fMainDetID.at(i).fIndex].EncodeEventData(elements);

    }


    // If there is element data, generate the subbank header
    std::vector<UInt_t> subbankheader;
    std::vector<UInt_t> rocheader;

    if (elements.size() > 0) {

        // Form CODA subbank header
        subbankheader.clear();
        subbankheader.push_back(elements.size() + 1);	// subbank size
        subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
        // subbank tag | subbank type | event number

        // Form CODA bank/roc header
        rocheader.clear();
        rocheader.push_back(subbankheader.size() + elements.size() + 1);	// bank/roc size
        rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
        // bank tag == ROC | bank type | event number

        // Add bank header, subbank header and element data to output buffer
        buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
        buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
        buffer.insert(buffer.end(), elements.begin(), elements.end());
    
    }

}

void  VQwDetectorArray::RandomizeMollerEvent(int helicity /*, const QwBeamCharge& charge, const QwBeamPosition& xpos, const QwBeamPosition& ypos, const QwBeamAngle& xprime, const QwBeamAngle& yprime, const QwBeamEnergy& energy*/) {

    /*  fTargetCharge.PrintInfo();
        fTargetX.PrintInfo();
        fTargetY.PrintInfo();
        fTargetXprime.PrintInfo();
        fTargetYprime.PrintInfo();
        fTargetEnergy.PrintInfo();*/

    if(RequestExternalValue("x_targ", &fTargetX)){
        
        if (bDEBUG){
            
            dynamic_cast<QwMollerADC_Channel*>(&fTargetX)->PrintInfo();
            QwWarning << "VQwDetectorArray::RandomizeMollerEvent Found "<<fTargetX.GetElementName()<< QwLog::endl;  
        }

    }else{

        bIsExchangedDataValid = kFALSE;
        QwError << GetName() << " could not get external value for "
	     << fTargetX.GetElementName() << QwLog::endl;

    }
  
    if(RequestExternalValue("y_targ", &fTargetY)){
        
        if (bDEBUG){
            dynamic_cast<QwMollerADC_Channel*>(&fTargetY)->PrintInfo();
            QwWarning << "VQwDetectorArray::RandomizeMollerEvent Found "<<fTargetY.GetElementName()<< QwLog::endl;
        }

    }else{

        bIsExchangedDataValid = kFALSE;
        QwError << GetName() << " could not get external value for "
	     << fTargetY.GetElementName() << QwLog::endl;
    }
  
    if(RequestExternalValue("xp_targ", &fTargetXprime)){
        
        if (bDEBUG){
            
            dynamic_cast<QwMollerADC_Channel*>(&fTargetXprime)->PrintInfo();
            QwWarning << "VQwDetectorArray::RandomizeMollerEvent Found "<<fTargetXprime.GetElementName()<< QwLog::endl;
        }

    }else{

        bIsExchangedDataValid = kFALSE;
        QwError << GetName() << " could not get external value for "
	     << fTargetXprime.GetElementName() << QwLog::endl;
    
    }

    if(RequestExternalValue("yp_targ", &fTargetYprime)){

        if (bDEBUG){

            dynamic_cast<QwMollerADC_Channel*>(&fTargetYprime)->PrintInfo();
            QwWarning << "VQwDetectorArray::RandomizeMollerEvent Found "<<fTargetYprime.GetElementName()<< QwLog::endl;
        
        }

    }else{

        bIsExchangedDataValid = kFALSE;
        QwError << GetName() << " could not get external value for "
	     << fTargetYprime.GetElementName() << QwLog::endl;

    }

    if(RequestExternalValue("e_targ", &fTargetEnergy)){

        if (bDEBUG){

            dynamic_cast<QwMollerADC_Channel*>(&fTargetEnergy)->PrintInfo();
            QwWarning << "VQwDetectorArray::RandomizeMollerEvent Found "<<fTargetEnergy.GetElementName()<< QwLog::endl;
        
        }

    }else{

        bIsExchangedDataValid = kFALSE;
        QwError << GetName() << " could not get external value for "
	     << fTargetEnergy.GetElementName() << QwLog::endl;

    }
    
    for (size_t i = 0; i < fMainDetID.size(); i++) {

        fIntegrationPMT[i].RandomizeMollerEvent(helicity, fTargetCharge, fTargetX, fTargetY, fTargetXprime, fTargetYprime, fTargetEnergy);
        //fIntegrationPMT[i].PrintInfo();
    
    }
 
}

Int_t VQwDetectorArray::ProcessConfigurationBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words) {

  /*  Int_t index = GetSubbankIndex(roc_id,bank_id);
    if (index>=0 && num_words>0){
      //  We want to process the configuration data for this ROC.
      UInt_t words_read = 0;
      for (size_t i = 0; i < fMainDetID.size(); i++) {
        words_read += fIntegrationPMT[i].ProcessConfigurationBuffer(&(buffer[words_read]),
  							   num_words-words_read);
      }
    }*/
  return 0;

}


Int_t VQwDetectorArray::ProcessEvBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words) {

    Bool_t lkDEBUG=kFALSE;

    Int_t index = GetSubbankIndex(roc_id,bank_id);

    if (index>=0 && num_words>0) {

        //  We want to process this ROC.  Begin looping through the data.
        if (lkDEBUG)
            std::cout << "VQwDetectorArray::ProcessEvBuffer:  "
             << "Begin processing ROC" << roc_id
             << " and subbank "<<bank_id
             << " number of words="<<num_words<<std::endl;

        for (size_t i=0;i<fMainDetID.size();i++) {

            if (fMainDetID[i].fSubbankIndex==index) {

                if (fMainDetID[i].fTypeID == kQwIntegrationPMT) {

                    if (lkDEBUG) {

                        std::cout<<"found IntegrationPMT data for "<<fMainDetID[i].fdetectorname<<std::endl;
                        std::cout<<"word left to read in this buffer:"<<num_words-fMainDetID[i].fWordInSubbank<<std::endl;
                    
                    }

                    fIntegrationPMT[fMainDetID[i].fIndex].ProcessEvBuffer(&(buffer[fMainDetID[i].fWordInSubbank]),
                     num_words-fMainDetID[i].fWordInSubbank);

                }

            }

        }

    }

  return 0;

}



Bool_t VQwDetectorArray::ApplySingleEventCuts() {

    Bool_t status=kTRUE;
    for(size_t i=0;i<fIntegrationPMT.size();i++){
        
        status &= fIntegrationPMT[i].ApplySingleEventCuts();
        if(!status && bDEBUG) 
         std::cout<<"******* VQwDetectorArray::SingleEventCuts()->IntegrationPMT[ "<<i<<" , "<<fIntegrationPMT[i].GetElementName()<<" ] ******\n"; 
    }
    
    for(size_t i=0;i<fCombinedPMT.size();i++){

        status &= fCombinedPMT[i].ApplySingleEventCuts();
        if(!status && bDEBUG) 
         std::cout<<"******* VQwDetectorArray::SingleEventCuts()->CombinedPMT[ "<<i<<" , "<<fCombinedPMT[i].GetElementName()<<" ] ******\n"; 
    }

    if (!status) 
     fMainDetErrorCount++; //failed  event counter for VQwDetectorArray

    return status;
}


UInt_t VQwDetectorArray::GetEventcutErrorFlag() { //return the error flag 

    UInt_t ErrorFlag;

    ErrorFlag=0;

    for(size_t i=0;i<fIntegrationPMT.size();i++){

        ErrorFlag |= fIntegrationPMT[i].GetEventcutErrorFlag();

    }

    for(size_t i=0;i<fCombinedPMT.size();i++){

        ErrorFlag |= fCombinedPMT[i].GetEventcutErrorFlag();
    
    }

    return ErrorFlag;

}

void VQwDetectorArray::IncrementErrorCounters() {

    for(size_t i=0;i<fIntegrationPMT.size();i++){

        fIntegrationPMT[i].IncrementErrorCounters();

    }

    for(size_t i=0;i<fCombinedPMT.size();i++){

        fCombinedPMT[i].IncrementErrorCounters();

    }

}

//inherited from the VQwSubsystemParity; this will display the error summary
void VQwDetectorArray::PrintErrorCounters() const {

    QwMessage<<"*********VQwDetectorArray Error Summary****************"<<QwLog::endl;
    QwMollerADC_Channel::PrintErrorCounterHead();
    
    for(size_t i=0;i<fIntegrationPMT.size();i++){

        //std::cout<<"  IntegrationPMT ["<<i<<"] "<<std::endl;
        fIntegrationPMT[i].PrintErrorCounters();

    }

    for(size_t i=0;i<fCombinedPMT.size();i++){

        //std::cout<<"  CombinedPMT ["<<i<<"] "<<std::endl;
        fCombinedPMT[i].PrintErrorCounters();

    }
    
    QwMollerADC_Channel::PrintErrorCounterTail();

}

Bool_t VQwDetectorArray::CheckForBurpFail(const VQwSubsystem *subsys) {

    Bool_t burpstatus = kFALSE;
    VQwSubsystem* tmp = const_cast<VQwSubsystem *>(subsys);

    if(Compare(tmp)) {

        const VQwDetectorArray* input = dynamic_cast<const VQwDetectorArray*>(subsys);

        for(size_t i=0;i<input->fIntegrationPMT.size();i++)
         burpstatus |= (this->fIntegrationPMT[i]).CheckForBurpFail(&(input->fIntegrationPMT[i]));

        for(size_t i=0;i<input->fCombinedPMT.size();i++)
         burpstatus |= (this->fCombinedPMT[i]).CheckForBurpFail(&(input->fCombinedPMT[i]));

    }

    return burpstatus;

}


void VQwDetectorArray::UpdateErrorFlag(const VQwSubsystem *ev_error){

    VQwSubsystem* tmp = const_cast<VQwSubsystem*>(ev_error);

    if(Compare(tmp)){

        const VQwDetectorArray* input = dynamic_cast<const VQwDetectorArray*> (ev_error);

        for (size_t i=0;i<input->fIntegrationPMT.size();i++)
         this->fIntegrationPMT[i].UpdateErrorFlag(&(input->fIntegrationPMT[i]));
    
        for (size_t i=0;i<input->fCombinedPMT.size();i++)
         this->fCombinedPMT[i].UpdateErrorFlag(&(input->fCombinedPMT[i]));

    }

}


void  VQwDetectorArray::ProcessEvent() {

    for (size_t i=0;i<fIntegrationPMT.size();i++)
     fIntegrationPMT[i].ProcessEvent();

    for (size_t i=0;i<fCombinedPMT.size();i++) {

        //std::cout<<"Process combination "<<i<<std::endl;
        fCombinedPMT[i].ProcessEvent();

    }

    return;

}

/**
 * Exchange data between subsystems
 */

void  VQwDetectorArray::ExchangeProcessedData() {

    //QwWarning << "VQwDetectorArray::ExchangeProcessedData "<< QwLog::endl;
    bIsExchangedDataValid = kTRUE;

    if (1==1 || bNormalization) {

        if(RequestExternalValue("q_targ", &fTargetCharge)) {

            if (bDEBUG) {

	            QwWarning << "VQwDetectorArray::ExchangeProcessedData Found "<<fTargetCharge.GetElementName()<< QwLog::endl;
	            //QwWarning <<"****VQwDetectorArray****"<< QwLog::endl;
	            (dynamic_cast<QwMollerADC_Channel*>(&fTargetCharge))->PrintInfo();
      
            }

        } else {

            bIsExchangedDataValid = kFALSE;
            QwError << GetName() << " could not get external value for "
	         << fTargetCharge.GetElementName() << QwLog::endl;

        }
    
    }

}


void  VQwDetectorArray::ProcessEvent_2() {

    if (bIsExchangedDataValid) {
      
        //data is valid, process it
      
        if (bDEBUG) {

            Double_t  pedestal = fTargetCharge.GetPedestal();
            Double_t  calfactor = fTargetCharge.GetCalibrationFactor();
            Double_t  volts = fTargetCharge.GetAverageVolts();
        
            std::cout<<"VQwDetectorArray::ProcessEvent_2(): processing with exchanged data"<<std::endl;
            std::cout<<"pedestal, calfactor, average volts = "<<pedestal<<", "<<calfactor<<", "<<volts<<std::endl;
        
        }
      
        if (bNormalization && fTargetCharge.GetValue()>fNormThreshold)
	     this->DoNormalization();

    } else {
      
        QwWarning<<"VQwDetectorArray::ProcessEvent_2(): could not get all external values."<<QwLog::endl;
    
    }

}




void  VQwDetectorArray::ConstructHistograms(TDirectory *folder, TString &prefix) {

    for (size_t i=0;i<fIntegrationPMT.size();i++)
     fIntegrationPMT[i].ConstructHistograms(folder,prefix);

    for (size_t i=0;i<fCombinedPMT.size();i++)
     fCombinedPMT[i].ConstructHistograms(folder,prefix);
    
    return;

}


void  VQwDetectorArray::FillHistograms() {

    for (size_t i=0;i<fIntegrationPMT.size();i++)
     fIntegrationPMT[i].FillHistograms();

    for (size_t i=0;i<fCombinedPMT.size();i++)
     fCombinedPMT[i].FillHistograms();

    return;

}


void VQwDetectorArray::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values) {

    for (size_t i=0;i<fIntegrationPMT.size();i++)
     fIntegrationPMT[i].ConstructBranchAndVector(tree, prefix, values);

    for (size_t i=0;i<fCombinedPMT.size();i++)
     fCombinedPMT[i].ConstructBranchAndVector(tree, prefix, values);

    return;

}

void VQwDetectorArray::ConstructBranch(TTree *tree, TString & prefix) {

    for (size_t i=0;i<fIntegrationPMT.size();i++)
     fIntegrationPMT[i].ConstructBranch(tree, prefix);

    for (size_t i=0;i<fCombinedPMT.size();i++)
     fCombinedPMT[i].ConstructBranch(tree, prefix);

    return;

}

void VQwDetectorArray::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file) {

    TString tmp;
    QwParameterFile* nextmodule;
    trim_file.RewindToFileStart();
    tmp="QwIntegrationPMT";
    trim_file.RewindToFileStart();

    if (trim_file.FileHasModuleHeader(tmp)){

        nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
        
        for (size_t i=0;i<fIntegrationPMT.size();i++)
         fIntegrationPMT[i].ConstructBranch(tree, prefix, *nextmodule);
    
    }

    tmp="QwCombinedPMT";
    trim_file.RewindToFileStart();

    if (trim_file.FileHasModuleHeader(tmp)){
        
        nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
        
        for (size_t i=0;i<fCombinedPMT.size();i++)
         fCombinedPMT[i].ConstructBranch(tree, prefix, *nextmodule );
    }

    return;
}

void VQwDetectorArray::FillTreeVector(std::vector<Double_t> &values) const {

    for (size_t i=0;i<fIntegrationPMT.size();i++)
     fIntegrationPMT[i].FillTreeVector(values);

    for (size_t i=0;i<fCombinedPMT.size();i++)
     fCombinedPMT[i].FillTreeVector(values);

    return;

}


const QwIntegrationPMT* VQwDetectorArray::GetChannel(const TString name) const {

    return GetIntegrationPMT(name);

}


Bool_t VQwDetectorArray::Compare(VQwSubsystem *value) {

    //  std::cout<<" Here in VQwDetectorArray::Compare \n";

    Bool_t res=kTRUE;

    if (typeid(*value)!=typeid(*this)) {

        res=kFALSE;
        //std::cout<<" types are not ok \n";
        //std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    
    } else {
      
        VQwDetectorArray* input = dynamic_cast<VQwDetectorArray*>(value);

        if (input->fIntegrationPMT.size()!=fIntegrationPMT.size()
         || input->fCombinedPMT.size()!=fCombinedPMT.size() ) {

            res=kFALSE;
            //std::cout<<" not the same number of channels \n";

        }

    }

  return res;

}


VQwSubsystem&  VQwDetectorArray::operator=  (VQwSubsystem *value) {

    //  std::cout<<" here in VQwDetectorArray::operator= \n";

    if (Compare(value)) {

        //VQwSubsystem::operator=(value);
        VQwDetectorArray* input = dynamic_cast<VQwDetectorArray*> (value);

        for (size_t i=0;i<input->fIntegrationPMT.size();i++)
         this->fIntegrationPMT[i]=input->fIntegrationPMT[i];

        for (size_t i=0;i<input->fCombinedPMT.size();i++)
         (this->fCombinedPMT[i])=(input->fCombinedPMT[i]);

    }

    return *this;

}


VQwSubsystem&  VQwDetectorArray::operator+=  (VQwSubsystem *value) {

    if (Compare(value)) {

        VQwDetectorArray* input= dynamic_cast<VQwDetectorArray*>(value) ;

        for (size_t i=0;i<input->fIntegrationPMT.size();i++)
         this->fIntegrationPMT[i]+=input->fIntegrationPMT[i];

        for (size_t i=0;i<input->fCombinedPMT.size();i++)
         this->fCombinedPMT[i]+=input->fCombinedPMT[i];

    }

  return *this;

}


VQwSubsystem&  VQwDetectorArray::operator-=  (VQwSubsystem *value) {

    if (Compare(value)) {

        VQwDetectorArray* input= dynamic_cast<VQwDetectorArray*>(value);

        for (size_t i=0;i<input->fIntegrationPMT.size();i++)
         this->fIntegrationPMT[i]-=input->fIntegrationPMT[i];

        for (size_t i=0;i<input->fCombinedPMT.size();i++)
         this->fCombinedPMT[i]-=input->fCombinedPMT[i];

    }

    return *this;

}


void VQwDetectorArray::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom) {

    if (Compare(numer)&&Compare(denom)) {

        VQwDetectorArray* innumer= dynamic_cast<VQwDetectorArray*>(numer) ;
        VQwDetectorArray* indenom= dynamic_cast<VQwDetectorArray*>(denom) ;

        for (size_t i=0;i<innumer->fIntegrationPMT.size();i++)
         this->fIntegrationPMT[i].Ratio(innumer->fIntegrationPMT[i],indenom->fIntegrationPMT[i]);

        for (size_t i=0;i<innumer->fCombinedPMT.size();i++)
         this->fCombinedPMT[i].Ratio(innumer->fCombinedPMT[i],indenom->fCombinedPMT[i]);

    }

    return;

}


void VQwDetectorArray::Scale(Double_t factor) {

    for (size_t i=0;i<fIntegrationPMT.size();i++)
     fIntegrationPMT[i].Scale(factor);

    for (size_t i=0;i<fCombinedPMT.size();i++)
     fCombinedPMT[i].Scale(factor);

    return;

}

//*****************************************************************//

void VQwDetectorArray::Normalize(VQwDataElement* denom) {

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
     fIntegrationPMT[i].Normalize(denom);
    
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
     fCombinedPMT[i].Normalize(denom);

}



void VQwDetectorArray::CalculateRunningAverage() {

    for (size_t i=0;i<fIntegrationPMT.size();i++)
     fIntegrationPMT[i].CalculateRunningAverage();

    for (size_t i=0;i<fCombinedPMT.size();i++)
     fCombinedPMT[i].CalculateRunningAverage();

    return;

}


void VQwDetectorArray::AccumulateRunningSum(VQwSubsystem* value1, Int_t count, Int_t ErrorMask) {

    if (Compare(value1)) {
        
        VQwDetectorArray* value = dynamic_cast<VQwDetectorArray*>(value1);

        for (size_t i = 0; i < fIntegrationPMT.size(); i++)
         fIntegrationPMT[i].AccumulateRunningSum(value->fIntegrationPMT[i], count, ErrorMask);
    
        for (size_t i = 0; i < fCombinedPMT.size(); i++)
         fCombinedPMT[i].AccumulateRunningSum(value->fCombinedPMT[i], count, ErrorMask);
    
    }

}

void VQwDetectorArray::DeaccumulateRunningSum(VQwSubsystem* value1, Int_t ErrorMask){
    
    if (Compare(value1)) {
        
        VQwDetectorArray* value = dynamic_cast<VQwDetectorArray*>(value1);

        for (size_t i = 0; i < fIntegrationPMT.size(); i++)
         fIntegrationPMT[i].DeaccumulateRunningSum(value->fIntegrationPMT[i], ErrorMask);
        
        for (size_t i = 0; i < fCombinedPMT.size(); i++)
         fCombinedPMT[i].DeaccumulateRunningSum(value->fCombinedPMT[i], ErrorMask);

    }  

}


EQwPMTInstrumentType VQwDetectorArray::GetDetectorTypeID(TString name) {
    
    return GetQwPMTInstrumentType(name);

}

//*****************************************************************//

Int_t VQwDetectorArray::GetDetectorIndex(EQwPMTInstrumentType type_id, TString name) {

    Bool_t ldebug=kFALSE;
    
    if (ldebug) {

        std::cout<<"VQwDetectorArray::GetDetectorIndex\n";
        std::cout<<"type_id=="<<type_id<<" name="<<name<<"\n";
        std::cout<<fMainDetID.size()<<" already registered detector\n";
    }

    Int_t result=-1;
    for (size_t i=0;i<fMainDetID.size();i++) {

        if (fMainDetID[i].fTypeID==type_id)
         if (fMainDetID[i].fdetectorname==name) {
            
            result=fMainDetID[i].fIndex;

            if (ldebug)
             std::cout<<"testing against ("<<fMainDetID[i].fTypeID
             <<","<<fMainDetID[i].fdetectorname<<")=>"<<result<<"\n";

         }
    }

    return result;

}

const QwIntegrationPMT* VQwDetectorArray::GetIntegrationPMT(const TString name) const {

    TString tmpname = name;
    tmpname.ToLower();
    if (! fIntegrationPMT.empty()) {

        for (size_t i=0;i<fIntegrationPMT.size();i++) {

            if (fIntegrationPMT.at(i).GetElementName() == tmpname) {

                //std::cout<<"Get IntegrationPMT "<<tmpname<<std::endl;
                return &(fIntegrationPMT.at(i));
            
            }
        
        }
    
    }
    
    QwMessage << "VQwDetectorArray::GetIntegrationPMT: cannot find channel " << tmpname << QwLog::endl;
    
    return NULL;

}


const QwCombinedPMT* VQwDetectorArray::GetCombinedPMT(const TString name) const {
  TString tmpname = name;
  tmpname.ToLower();
  if (! fCombinedPMT.empty())
    {
      for (size_t i=0;i<fCombinedPMT.size();i++)
        {
          if (fCombinedPMT.at(i).GetElementName() == tmpname)
            {
              //std::cout<<"Get CombinedPMT "<<tmpname<<std::endl;
              return &(fCombinedPMT.at(i));
            }
        }
    }
  QwMessage << "VQwDetectorArray::GetCombinedPMT: cannot find channel " << tmpname << QwLog::endl;
  return NULL;
}

void VQwDetectorArray::DoNormalization(Double_t factor) {

    if (bIsExchangedDataValid) {

        try {

	        this->Normalize(&fTargetCharge);
        
        }
        
        catch (std::exception& e) {

            std::cerr << e.what() << std::endl;
        
        }
    
    }

}

#ifdef __USE_DATABASE__

void  VQwDetectorArray::FillDB(QwParityDB *db, TString datatype) {

    Bool_t local_print_flag = false;

    if(local_print_flag) {
        
        QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
        QwMessage << "            VQwDetectorArray::FillDB                       " << QwLog::endl;
        QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    
    }

    std::vector<QwDBInterface> interface;
    std::vector<QwParitySSQLS::md_data> entrylist;

    UInt_t analysis_id = db->GetAnalysisID();

    TString measurement_type;
    measurement_type = QwDBInterface::DetermineMeasurementTypeID(datatype);

    UInt_t i,j;
    i = j = 0;
    
    if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "IntegrationPMT" <<QwLog::endl;

    for(i=0; i<fIntegrationPMT.size(); i++) {
    
        interface.clear();
        interface = fIntegrationPMT[i].GetDBEntry();
        
        for(j=0; j<interface.size(); j++) {
            
            interface.at(j).SetAnalysisID( analysis_id );
            interface.at(j).SetMainDetectorID( db );
            interface.at(j).SetMeasurementTypeID( measurement_type );
            interface.at(j).PrintStatus( local_print_flag );
            interface.at(j).AddThisEntryToList( entrylist );
        }
    }

    if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Combined PMT" <<QwLog::endl;

    for(i=0; i< fCombinedPMT.size(); i++) {

        interface.clear();
        interface = fCombinedPMT[i].GetDBEntry();
        
        for(j=0; j<interface.size(); j++) {
	        
            interface.at(j).SetAnalysisID( analysis_id );
	        interface.at(j).SetMainDetectorID( db );
	        interface.at(j).SetMeasurementTypeID( measurement_type );
	        interface.at(j).PrintStatus( local_print_flag );
	        interface.at(j).AddThisEntryToList( entrylist );
        }

    }

    if(local_print_flag) {

        QwMessage << QwColor(Qw::kGreen) << "Entrylist Size : "
	     << QwColor(Qw::kBoldRed) << entrylist.size()
         << QwColor(Qw::kNormal) << QwLog::endl;

    }

    db->Connect();
    
    // Check the entrylist size, if it isn't zero, start to query..
    
    if( entrylist.size() ) {
    
        mysqlpp::Query query= db->Query();
        query.insert(entrylist.begin(), entrylist.end());
        query.execute();
    } else {
        
        QwMessage << "VQwDetectorArray::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
    
    }
    
    db->Disconnect();
    
    return;

}

#endif

void  VQwDetectorArray::PrintValue() const {

    QwMessage << "=== VQwDetectorArray: " << GetName() << " ===" << QwLog::endl;
    
    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
     fIntegrationPMT[i].PrintValue();
    
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
     fCombinedPMT[i].PrintValue();

}

void  VQwDetectorArray::PrintInfo() const {

    std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";

    std::cout<<"there are "<<fIntegrationPMT.size()<<" IntegrationPMT \n";
    std::cout<<"          "<<fCombinedPMT.size()<<" CombinedPMT \n";

    std::cout<<" Printing Running AVG and other channel info"<<std::endl;

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
     fIntegrationPMT[i].PrintInfo();
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
     fCombinedPMT[i].PrintInfo();

}

void  VQwDetectorArray::PrintDetectorID() const {

    for (size_t i=0;i<fMainDetID.size();i++) {

        std::cout<<"============================="<<std::endl;
        std::cout<<" Detector ID="<<i<<std::endl;
        fMainDetID[i].Print();
    
    }

    return;

}

#ifdef __USE_DATABASE__

void VQwDetectorArray::FillErrDB(QwParityDB *db, TString datatype) {

    Bool_t local_print_flag = false;
    if(local_print_flag){
    
        QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
        QwMessage << "            QwDetectorArrayID::FillErrDB                  " << QwLog::endl;
        QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    
    }


    std::vector<QwErrDBInterface> interface;
    std::vector<QwParitySSQLS::md_errors> entrylist;

    UInt_t analysis_id = db->GetAnalysisID();

    UInt_t i,j;
    i = j = 0;
    if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "IntegrationPMT" <<QwLog::endl;

    for(i=0; i<fIntegrationPMT.size(); i++) {
        
        interface.clear();
        interface = fIntegrationPMT[i].GetErrDBEntry();

        for(j=0; j<interface.size(); j++) {

            interface.at(j).SetAnalysisID     ( analysis_id );
            interface.at(j).SetMainDetectorID ( db );
            interface.at(j).PrintStatus       ( local_print_flag );
            interface.at(j).AddThisEntryToList( entrylist );
        
        }
    
    }

    if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Combined PMT" <<QwLog::endl;

    for(i=0; i< fCombinedPMT.size(); i++) {

        interface.clear();
        interface = fCombinedPMT[i].GetErrDBEntry();
        
        for(j=0; j<interface.size(); j++) {
	
            interface.at(j).SetAnalysisID     ( analysis_id );
	        interface.at(j).SetMainDetectorID ( db );
	        interface.at(j).PrintStatus       ( local_print_flag );
	        interface.at(j).AddThisEntryToList( entrylist );

        }

    }

    if(local_print_flag) {

        QwMessage << QwColor(Qw::kGreen)   << "Entrylist Size : "
	              << QwColor(Qw::kBoldRed) << entrylist.size()
                  << QwColor(Qw::kNormal)  << QwLog::endl;

    }

    db->Connect();

    // Check the entrylist size, if it isn't zero, start to query..

    if( entrylist.size() ) {

        mysqlpp::Query query= db->Query();
        query.insert(entrylist.begin(), entrylist.end());
        query.execute();
    } else {

        QwMessage << "VQwDetectorArray::FillErrDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
    
    }

    db->Disconnect();

    return;

}

#endif


void VQwDetectorArray::WritePromptSummary(QwPromptSummary *ps, TString type) {

    Bool_t local_print_flag = false;
    Bool_t local_add_element= type.Contains("yield");
  
    if(local_print_flag){

        QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
        QwMessage << "        QwDetectorArrayID::WritePromptSummary()          " << QwLog::endl;
        QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    }

    const VQwHardwareChannel* tmp_channel = 0;
    TString  element_name        = "";
    Double_t element_value       = 0.0;
    Double_t element_value_err   = 0.0;
    Double_t element_value_width = 0.0;

    PromptSummaryElement *local_ps_element = NULL;
    Bool_t local_add_these_elements= false;

    for (size_t i = 0; i < fMainDetID.size();  i++) {

        element_name        = fMainDetID[i].fdetectorname;
        tmp_channel=GetIntegrationPMT(element_name)->GetChannel(element_name);	
        element_value       = 0.0;
        element_value_err   = 0.0;
        element_value_width = 0.0;
    

        local_add_these_elements=element_name.Contains("sam"); // Need to change this to add other detectorss in summary

        if(local_add_these_elements&&local_add_element){
      	    
            ps->AddElement(new PromptSummaryElement(element_name));     
        
        }


        local_ps_element=ps->GetElementByName(element_name);

      
        if(local_ps_element) {

	        element_value       = tmp_channel->GetValue();
	        element_value_err   = tmp_channel->GetValueError();
	        element_value_width = tmp_channel->GetValueWidth();
	
	        local_ps_element->Set(type, element_value, element_value_err, element_value_width);
        
        }
      
        if( local_print_flag && local_ps_element) {

	        printf("Type %12s, Element %32s, value %12.4e error %8.4e  width %12.4e\n", 
	         type.Data(), element_name.Data(), element_value, element_value_err, element_value_width);
        
        }
    
    }

  return;

}



void  QwDetectorArrayID::Print() const {

    std::cout<<std::endl<<"Detector name= "<<fdetectorname<<std::endl;
    std::cout<<"SubbankkIndex= "<<fSubbankIndex<<std::endl;
    std::cout<<"word index in subbank= "<<fWordInSubbank<<std::endl;
    std::cout<<"module type= "<<fmoduletype<<std::endl;
    std::cout<<"detector type= "<<fdetectortype<<"    index= "<<fTypeID<<std::endl;
    std::cout<<"Index of this detector in the vector of similar detector= "<<fIndex<<std::endl;
    std::cout<<"Subelement index= "<<fSubelement<<std::endl;
    std::cout<<"==========================================\n";

    return;
    
}




