//---------------------------------------------------------
//
// RISQTutorialAnalysis.cc
// linehan3@fnal.gov
// 5/23/2024
//
// This is an analysis macro that is built to analyze
// the output of the G4CMP simulations that are run in
// the RISQ G4CMP Tutorial code.
//
//---------------------------------------------------------

//C++ includes
#include <cstdlib>
#include <fstream>
#include <iostream>

//ROOT includes
#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TLegend.h"

//---------------------------------------------------------------------------------------
// Define a set of structs for use interpreting the output from G4CMP
struct Hit
{
  double EnergyDeposit;
  double PositionX;
  double PositionY;
  double PositionZ;
  double Time;
  double ParticleType;
};

// //Forward declarations
// int FindClosestQubitID(double hitX_mm, double hitY_mm);
  
void AnalyzeProtonEvent(std::string hitsFilename)
{
  std::string base_filename = "sim_output_";
  //Read in the text files
  TFile* fIn = TFile::Open(hitsFilename.c_str(), "READ");
  std::cout << "Loaded ROOT file." << std::endl;

  // TTreeReader used to parse TTree with for loop
  TTreeReader myReader("Hits", fIn);
  // The branch "EnergyDeposit" contains floats; access them as myEnergyDeposit.
  TTreeReaderValue<Double_t> myEnergyDeposit(myReader, "EnergyDeposit");
  TTreeReaderValue<Double_t> myPositionX(myReader, "PositionX");
  TTreeReaderValue<Double_t> myPositionY(myReader, "PositionY");
  TTreeReaderValue<Double_t> myPositionZ(myReader, "PositionZ");
  TTreeReaderValue<Double_t> myTime(myReader, "Time");
  TTreeReaderArray<std::string> myParticleType(myReader, "ParticleType");
  
  //Define a number of histograms for the hits
  // eDep
  int eDepMinLog = -4;
  int eDepMaxLog = 10;
  int nBinsEDep = 200;
  // X, Y, Z
  double sensorMinX = -0.5;
  double sensorMaxX = 0.5;
  double sensorMinY = -0.5;
  double sensorMaxY = 0.5;
  double sensorMinZ = -2.;
  double sensorMaxZ = 0.;
  int nBinsX = 200;
  int nBinsY = 200;
  int nBinsZ = 200;
  // Time
  double minTime = 0.;
  double maxTime = 200.;
  int nBinsTime = 200.;
  // Proton
  TH1F* proton_eDep = new TH1F("proton_eDep", "Hit EDeps; log10(eDep[eV]); nEvents",nBinsEDep,eDepMinLog,eDepMaxLog);
  TH2F* proton_hitXY = new TH2F("proton_hitXY","XY Locations of proton Hits; X [um]; Y [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsY,sensorMinY,sensorMaxY);
  TH2F* proton_hitYZ = new TH2F("proton_hitYZ","YZ Locations of proton Hits; Y [um]; Z [um]; NHits/bin",nBinsY,sensorMinY,sensorMaxY,nBinsZ,sensorMinZ,sensorMaxZ);
  TH2F* proton_hitXZ = new TH2F("proton_hitXZ","XZ Locations of proton Hits; X [um]; Z [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsZ,sensorMinZ,sensorMaxZ);
  TH1F* proton_hitTime = new TH1F("proton_hitTime", "Hit Global Time; Time [ns]; nEvents",nBinsTime,minTime,maxTime);
  proton_eDep->SetFillColor(kRed);
  proton_hitTime->SetFillColor(kRed);
  // Phonon (all types)
  TH1F* phonon_eDep = new TH1F("phonon_eDep", "Hit EDeps; log10(eDep[eV]); nEvents",nBinsEDep,eDepMinLog,eDepMaxLog);
  TH2F* phonon_hitXY = new TH2F("phonon_hitXY","XY Locations of phonon Hits; X [um]; Y [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsY,sensorMinY,sensorMaxY);
  TH2F* phonon_hitYZ = new TH2F("phonon_hitYZ","YZ Locations of phonon Hits; Y [um]; Z [um]; NHits/bin",nBinsY,sensorMinY,sensorMaxY,nBinsZ,sensorMinZ,sensorMaxZ);
  TH2F* phonon_hitXZ = new TH2F("phonon_hitXZ","XZ Locations of phonon Hits; X [um]; Z [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsZ,sensorMinZ,sensorMaxZ);
  TH1F* phonon_hitTime = new TH1F("phonon_hitTime", "Hit Global Time; Time [ns]; nEvents",nBinsTime,minTime,maxTime);
  phonon_eDep->SetFillColor(kBlue);
  phonon_hitTime->SetFillColor(kBlue);
  // PhononL
  TH1F* phononL_eDep = new TH1F("phononL_eDep", "Hit EDeps; log10(eDep[eV]); nEvents",nBinsEDep,eDepMinLog,eDepMaxLog);
  TH2F* phononL_hitXY = new TH2F("phononL_hitXY","XY Locations of phononL Hits; X [um]; Y [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsY,sensorMinY,sensorMaxY);
  TH2F* phononL_hitYZ = new TH2F("phononL_hitYZ","YZ Locations of phononL Hits; Y [um]; Z [um]; NHits/bin",nBinsY,sensorMinY,sensorMaxY,nBinsZ,sensorMinZ,sensorMaxZ);
  TH2F* phononL_hitXZ = new TH2F("phononL_hitXZ","XZ Locations of phononL Hits; X [um]; Z [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsZ,sensorMinZ,sensorMaxZ);
  TH1F* phononL_hitTime = new TH1F("phononL_hitTime", "Hit Global Time; Time [ns]; nEvents",nBinsTime,minTime,maxTime);
  phononL_eDep->SetFillColor(kMagenta);
  phononL_hitTime->SetFillColor(kMagenta);
  // PhononTS
  TH1F* phononTS_eDep = new TH1F("phononTS_eDep", "Hit EDeps; log10(eDep[eV]); nEvents",nBinsEDep,eDepMinLog,eDepMaxLog);
  TH2F* phononTS_hitXY = new TH2F("phononTS_hitXY","XY Locations of phononTS Hits; X [um]; Y [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsY,sensorMinY,sensorMaxY);
  TH2F* phononTS_hitYZ = new TH2F("phononTS_hitYZ","YZ Locations of phononTS Hits; Y [um]; Z [um]; NHits/bin",nBinsY,sensorMinY,sensorMaxY,nBinsZ,sensorMinZ,sensorMaxZ);
  TH2F* phononTS_hitXZ = new TH2F("phononTS_hitXZ","XZ Locations of phononTS Hits; X [um]; Z [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsZ,sensorMinZ,sensorMaxZ);
  TH1F* phononTS_hitTime = new TH1F("phononTS_hitTime", "Hit Global Time; Time [ns]; nEvents",nBinsTime,minTime,maxTime);
  phononTS_eDep->SetFillColor(kCyan);
  phononTS_hitTime->SetFillColor(kCyan);
  // PhononTF
  TH1F* phononTF_eDep = new TH1F("phononTF_eDep", "Hit EDeps; log10(eDep[eV]); nEvents",nBinsEDep,eDepMinLog,eDepMaxLog);
  TH2F* phononTF_hitXY = new TH2F("phononTF_hitXY","XY Locations of phononTF Hits; X [um]; Y [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsY,sensorMinY,sensorMaxY);
  TH2F* phononTF_hitYZ = new TH2F("phononTF_hitYZ","YZ Locations of phononTF Hits; Y [um]; Z [um]; NHits/bin",nBinsY,sensorMinY,sensorMaxY,nBinsZ,sensorMinZ,sensorMaxZ);
  TH2F* phononTF_hitXZ = new TH2F("phononTF_hitXZ","XZ Locations of phononTF Hits; X [um]; Z [um]; NHits/bin",nBinsX,sensorMinX,sensorMaxX,nBinsZ,sensorMinZ,sensorMaxZ);
  TH1F* phononTF_hitTime = new TH1F("phononTF_hitTime", "Hit Global Time; Time [ns]; nEvents",nBinsTime,minTime,maxTime);
  phononTF_eDep->SetFillColor(kYellow);
  phononTF_hitTime->SetFillColor(kYellow);

  // Loop over all entries of the TTree.
  while (myReader.Next()) {
    // Just access the data as if myEnergyDeposit, etc were iterators (note the '*'
    //   in front of them):
    // std::cout << myParticleType << endl;
    if ((myParticleType).find("phonon") != std::string::npos) {
      phonon_eDep->Fill(*myEnergyDeposit);
      phonon_hitXY->Fill(*myPositionX, *myPositionY);
      phonon_hitYZ->Fill(*myPositionY, *myPositionZ);
      phonon_hitXZ->Fill(*myPositionX, *myPositionZ);
      phonon_hitTime->Fill(*myTime);

      if ((myParticleType).find("phononL") != std::string::npos) {
        phononL_eDep->Fill(*myEnergyDeposit);
        phononL_hitXY->Fill(*myPositionX, *myPositionY);
        phononL_hitYZ->Fill(*myPositionY, *myPositionZ);
        phononL_hitXZ->Fill(*myPositionX, *myPositionZ);
        phononL_hitTime->Fill(*myTime);
      } else if ((myParticleType).find("phononTS") != std::string::npos) {
        phononTS_eDep->Fill(*myEnergyDeposit);
        phononTS_hitXY->Fill(*myPositionX, *myPositionY);
        phononTS_hitYZ->Fill(*myPositionY, *myPositionZ);
        phononTS_hitXZ->Fill(*myPositionX, *myPositionZ);
        phononTS_hitTime->Fill(*myTime);
      } else if ((myParticleType).find("phononTF") != std::string::npos) {
        phononTF_eDep->Fill(*myEnergyDeposit);
        phononTF_hitXY->Fill(*myPositionX, *myPositionY);
        phononTF_hitYZ->Fill(*myPositionY, *myPositionZ);
        phononTF_hitXZ->Fill(*myPositionX, *myPositionZ);
        phononTF_hitTime->Fill(*myTime);
      }
    } else {
      proton_eDep->Fill(*myEnergyDeposit);
      proton_hitXY->Fill(*myPositionX, *myPositionY);
      proton_hitYZ->Fill(*myPositionY, *myPositionZ);
      proton_hitXZ->Fill(*myPositionX, *myPositionZ);
      proton_hitTime->Fill(*myTime);
    }
    
  }
  fIn->Close();

  //Define an outfile
  // std::string analysisFilename = hitsFilename.subtr(hitsFilename.rfind("/")+1, hitsFilename.rfind(".") - (hitsFilename.rfind("/")+1)) + "_AnalysisOutput.root"
  // TFile * fOut = new TFile(analysisFilename,"RECREATE");
  // std::cout << "Saving output into file: " << analysisFilename << std::endl;

  // Make proton-phonon eDep plots
  TCanvas* eDeps1 = new TCanvas();
  proton_eDep->Draw();
  phonon_eDep->Draw("SAME");
  TLegend* eDeps1_legend = new TLegend(0.7,0.7,0.9,0.9);
  eDeps1_legend->AddEntry(proton_eDep,"Proton eDep per-hit","f");
  eDeps1_legend->AddEntry(phonon_eDep,"All phonon eDep per-hit","f");
  eDeps1_legend->Draw();
  eDeps1->SaveAs(("proton_phonon_eDeps_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  eDeps1->Close();

  // Make split phonon eDep plots
  TCanvas* eDeps2 = new TCanvas();
  phononL_eDep->Draw();
  phononTS_eDep->Draw("SAME");
  phononTF_eDep->Draw("SAME");
  TLegend* eDeps2_legend = new TLegend(0.7,0.7,0.9,0.9);
  eDeps2_legend->AddEntry(phononL_eDep,"PhononL eDep per-hit","f");
  eDeps2_legend->AddEntry(phononTS_eDep,"PhononTS eDep per-hit","f");
  eDeps2_legend->AddEntry(phononTF_eDep,"PhononTF eDep per-hit","f");
  eDeps2_legend->Draw();
  eDeps2->SaveAs(("splitPhonon_eDeps_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  eDeps2->Close();

  // Make proton-phonon Time plots
  TCanvas* hitTimes1 = new TCanvas();
  proton_eDep->Draw();
  phonon_eDep->Draw("SAME");
  TLegend* hitTimes1_legend = new TLegend(0.7,0.7,0.9,0.9);
  hitTimes1_legend->AddEntry(proton_eDep,"Proton","f");
  hitTimes1_legend->AddEntry(phonon_eDep,"All phonon","f");
  hitTimes1_legend->Draw();
  eDeps1->SaveAs(("proton_phonon_hitTimes_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  hitTimes1->Close();

  // Make split phonon Time plots
  TCanvas* hitTimes2 = new TCanvas();
  phononL_hitTime->Draw();
  phononTS_hitTime->Draw("SAME");
  phononTF_hitTime->Draw("SAME");
  TLegend* hitTimes2_legend = new TLegend(0.7,0.7,0.9,0.9);
  hitTimes2_legend->AddEntry(phononL_hitTime,"PhononL","f");
  hitTimes2_legend->AddEntry(phononTS_hitTime,"PhononTS","f");
  hitTimes2_legend->AddEntry(phononTF_hitTime,"PhononTF","f");
  hitTimes2_legend->Draw();
  eDeps2->SaveAs(("splitPhonon_hitTimes_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  hitTimes2->Close();
  
  // Make pos plots
  TCanvas* pos = new TCanvas();
  // Proton
  proton_hitXY->Draw();
  pos->SaveAs(("protonXY_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  proton_hitYZ->Draw();
  pos->SaveAs(("protonYZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  proton_hitXZ->Draw();
  pos->SaveAs(("protonXZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  // Phonon
  phonon_hitXY->Draw();
  pos->SaveAs(("phononXY_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  phonon_hitYZ->Draw();
  pos->SaveAs(("phononYZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  phonon_hitXZ->Draw();
  pos->SaveAs(("phononXZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  // PhononL
  phononL_hitXY->Draw();
  pos->SaveAs(("phononLXY_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  phononL_hitYZ->Draw();
  pos->SaveAs(("phononLYZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  phononL_hitXZ->Draw();
  pos->SaveAs(("phononLXZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  // PhononTS
  phononTS_hitXY->Draw();
  pos->SaveAs(("phononTSXY_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  phononTS_hitYZ->Draw();
  pos->SaveAs(("phononTSYZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  phononTS_hitXZ->Draw();
  pos->SaveAs(("phononTSXZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  // PhononTF
  phononTF_hitXY->Draw();
  pos->SaveAs(("phononTFXY_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  phononTF_hitYZ->Draw();
  pos->SaveAs(("phononTFYZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();
  phononTF_hitXZ->Draw();
  pos->SaveAs(("phononTFXZ_"+hitsFilename.substr(hitsFilename.find(base_filename)+base_filename.length(), hitsFilename.rfind(".")-hitsFilename.find(base_filename)+base_filename.length())+".pdf").c_str());
  pos->Clear();

  
}

// //---------------------------------------------------------------------------------------
// // This is a bit of a kludge. What we really should do is pass the hit VOLUME out of
// // G4CMP in the hit info. However, this involves some broader modifications to G4CMP
// // beyond this example so we're kludging it together with this function.
// int FindClosestQubitID(double hitX_mm, double hitY_mm)
// {
//   const int nQubits = 6;
//   double qubitXY[nQubits][2] = {{-1.85,1.8},
// 				{0,1.8},
// 				{1.85,1.8},
// 				{-1.85,-1.8},
// 				{0,-1.8},
// 				{1.85,-1.8}};
//   double lowestDeltaR2_mm = 1000000;
//   int theQubitID = -1;
//   for( int iQ = 0; iQ < nQubits; ++iQ ){
//     double deltaR2_mm = TMath::Power((qubitXY[iQ][0] - hitX_mm),2) + TMath::Power((qubitXY[iQ][1] - hitY_mm),2);
//     if( deltaR2_mm < lowestDeltaR2_mm ){
//       lowestDeltaR2_mm = deltaR2_mm;
//       theQubitID = iQ;
//     }
//   }
//   return theQubitID;	 
// }


// //---------------------------------------------------------------------------------------
// // Analysis Script: Phonon Collection Efficiency
// void PCEStudy(std::string primariesFilename, std::string hitsFilename)
// {
//   //Read in the text files
//   std::vector<Event> eventList = ReadInG4CMPPrimaryAndHitFiles(hitsFilename,primariesFilename);
//   std::cout << "Done reading in events." << std::endl;


//   //Define an outfile
//   TFile * fOut = new TFile("AnalysisOutput.root","RECREATE");
  
//   //Define a number of histograms for the hits
//   TH1F * h_nHits = new TH1F("h_nHits","Number of Hits Per Event; nHits; nEvents",100,0,100);
//   TH1F * h_eDep = new TH1F("h_eDep","Hit EDeps; log10(eDep[eV]); nEvents",200,-6,1);  
//   TH2F * h_hitXY = new TH2F("h_nHitsXY","XY Locations of Hits; X [mm]; Y [mm]; NHits/bin",200,-5,5,200,-5,5);
//   TH2F * h_hitYZ = new TH2F("h_nHitsYZ","YZ Locations of Hits; Y [mm]; Z [mm]; NHits/bin",200,-5,5,200,4.6,5.02);
//   TH2F * h_hitXZ = new TH2F("h_nHitsXZ","XZ Locations of Hits; X [mm]; Z [mm]; NHits/bin",200,-5,5,200,4.6,5.02);
  
//   //Define a number of histograms for the primaries
//   TH2F * h_primXY = new TH2F("h_primXY","XY Location of Primary; X [mm]; Y [mm]; Primaries/bin",200,-5,5,200,-5,5);
//   TH2F * h_primYZ = new TH2F("h_primYZ","YZ Location of Primary; Y [mm]; Z [mm]; Primaries/bin",200,-5,5,200,4.6,5.02);
//   TH2F * h_primXZ = new TH2F("h_primXZ","XZ Location of Primary; X [mm]; Z [mm]; Primaries/bin",200,-5,5,200,4.6,5.02);

//   //Define histograms useful for calculating PCE
//   int nPCEBinsX = 50;
//   int nPCEBinsY = 50;
//   TH2F * h_totalHitEnergyAtPrimaryXY = new TH2F("h_totalHitEnergyAtPrimaryXY","Total Energy of Hits Generated by Primaries at this XY; X [mm]; Y [mm]; Energy/bin [eV]",nPCEBinsX,-5,5,nPCEBinsY,-5,5);
//   TH2F * h_totalPrimaryEnergyAtPrimaryXY = new TH2F("h_totalPrimaryEnergyAtPrimaryXY","Total Energy of Primaries at this XY; X [mm]; Y [mm]; Energy/bin [eV]",nPCEBinsX,-5,5,nPCEBinsY,-5,5);
//   TH2F * h_pceVsXY = new TH2F("h_pceVsXY","Phonon Collection Efficiency vs. Primary XY; X [mm]; Y [mm]; PCE",nPCEBinsX,-5,5,nPCEBinsY,-5,5);
  
  
//   //Loop over events
//   for( int iE = 0; iE < eventList.size(); ++iE ){
//     if( iE % 1000 == 0 ) std::cout << "Done with " << iE << " event histogram fills." << std::endl;

//     //Get the event
//     Event tE = eventList[iE];
    
//     //Add to the primary vector
//     PrimaryInfo thePrim = tE.thePrim;
//     h_primXY->Fill(thePrim.X_mm,thePrim.Y_mm);
//     h_primXZ->Fill(thePrim.X_mm,thePrim.Z_mm);
//     h_primYZ->Fill(thePrim.Y_mm,thePrim.Z_mm);
//     h_totalPrimaryEnergyAtPrimaryXY->Fill(thePrim.X_mm,thePrim.Y_mm,thePrim.energy_eV);      
    
//     //Plot a number of hit-related things: hit multiplicity, hit locations in XYZ, hits in XYZ weighted by energy, etc.
//     h_nHits->Fill(tE.hitVect.size());
//     for ( int iH = 0; iH < tE.hitVect.size(); ++iH ){

//       //Gather hit information
//       double hitX = tE.hitVect[iH].endX_mm;
//       double hitY = tE.hitVect[iH].endY_mm;
//       double hitZ = tE.hitVect[iH].endZ_mm;
//       double startX = tE.hitVect[iH].startX_mm;
//       double startY = tE.hitVect[iH].startY_mm;
//       double startZ = tE.hitVect[iH].startZ_mm;
//       double logEnergy_eV = TMath::Log10(tE.hitVect[iH].eDep_eV);


      
//       //Plot hit information
//       h_hitXY->Fill(hitX,hitY);
//       h_hitYZ->Fill(hitY,hitZ);
//       h_hitXZ->Fill(hitX,hitZ);
//       h_eDep->Fill(logEnergy_eV);
//       h_totalHitEnergyAtPrimaryXY->Fill(thePrim.X_mm,thePrim.Y_mm,tE.hitVect[iH].eDep_eV);
//     }
//   }

//   //Post-processing division
//   for( int iBX = 1; iBX <= h_totalHitEnergyAtPrimaryXY->GetNbinsX(); ++iBX ){
//     for( int iBY = 1; iBY <= h_totalHitEnergyAtPrimaryXY->GetNbinsY(); ++iBY ){
//       double num = h_totalHitEnergyAtPrimaryXY->GetBinContent(iBX,iBY);
//       double denom = h_totalPrimaryEnergyAtPrimaryXY->GetBinContent(iBX,iBY);
//       double pce = 0;
//       if( denom != 0 ) pce = num/denom;
//       h_pceVsXY->SetBinContent(iBX,iBY,pce);
//     }
//   }

  
//   //Write stuff currently established
//   fOut->Write();

// }

  
  




  









// //---------------------------------------------------------------------------------------
// // Parsing function
// std::vector<Event> ReadInG4CMPPrimaryAndHitFiles(std::string hitTextFilename, std::string primaryTextFilename)
// {
//   //Output: a vector of event objects nicely organizing our data.
//   std::vector<Event> output;

//   //First, let's open up our primary file and parse it. We'll get a map of int (event ID)
//   //to primary info and a map of int (eventID) to a list of hits
//   std::map<int,PrimaryInfo> primaryInfo = ParsePrimaryTextFileForPrimaries(primaryTextFilename);
//   std::map<int,std::vector<Hit> > hitInfo = ParseHitTextFileForHits(hitTextFilename);

//   //Now we do a final loop over event ID to merge these into actual events.
//   for( std::map<int,PrimaryInfo>::iterator it = primaryInfo.begin(); it != primaryInfo.end(); ++it ){
//     Event theEvent;
//     theEvent.runID = it->second.runID;
//     theEvent.eventID = it->first;
//     theEvent.hitVect = hitInfo[it->first];
//     theEvent.thePrim = it->second;
//     output.push_back(theEvent);
//   }
//   return output;

// }
  

// //---------------------------------------------------------------------------------------
// // Parsing function
// std::map<int,std::vector<Hit> > ParseHitTextFileForHits(std::string filename)
// {
//   std::map<int,std::vector<Hit> > output;
//   std::vector<Hit> dummy;
  
//   std::ifstream infile;
//   infile.open(filename.c_str());
//   std::string wholeLine;

//   //Begin loop through file
//   int eventID = -1;
//   int runID = -1;
//   int counter = 0;
//   while(1){
//     if(!infile.good()) break;
//     if(infile.is_open()){
//       std::getline(infile,wholeLine);
      
//       //Tokenize the string (split between spaces)
//       stringstream check1(wholeLine);
//       string token;
//       std::vector<std::string> tokens;
//       while(getline(check1,token,' ')){
// 	tokens.push_back(token);
//       }
//       if( tokens.size() == 0 ) break;
	  
//       //If we're on the first line of the file
//       if( tokens[0].find("Run") != std::string::npos ){
// 	continue;
//       }

//       //Check the runID and eventID, and if different than existing one,
//       //push back a new event into the map
//       if( std::atoi(tokens[0].c_str()) != runID || std::atoi(tokens[1].c_str()) != eventID ){
// 	output.emplace(std::atoi(tokens[1].c_str()),dummy);
// 	runID = std::atoi(tokens[0].c_str());
// 	eventID = std::atoi(tokens[1].c_str());
// 	counter++; 
// 	if( counter % 1000 == 0 ) std::cout << "Done reading " << counter << " events for hits." << std::endl;
//       }

//       //Log the hit information and push back into the most recently-created event in the vector
//       Hit theHit;      
//       theHit.runID = std::atoi(tokens[0].c_str());
//       theHit.eventID = std::atoi(tokens[1].c_str());
//       theHit.trackID = std::atoi(tokens[2].c_str());
//       theHit.particleName = tokens[3];
//       theHit.startEnergy_eV = std::atof(tokens[4].c_str());
//       theHit.startX_mm = std::atof(tokens[5].c_str());
//       theHit.startY_mm = std::atof(tokens[6].c_str());
//       theHit.startZ_mm = std::atof(tokens[7].c_str());
//       theHit.startT_ns = std::atof(tokens[8].c_str());
//       theHit.eDep_eV = std::atof(tokens[9].c_str());
//       theHit.trackWeight = std::atof(tokens[10].c_str());
//       theHit.endX_mm = std::atof(tokens[11].c_str());
//       theHit.endY_mm = std::atof(tokens[12].c_str());
//       theHit.endZ_mm = std::atof(tokens[13].c_str());
//       theHit.endT_ns = std::atof(tokens[14].c_str());
//       output[eventID].push_back(theHit);
//     }
//   }
//   return output;
// }


// //---------------------------------------------------------------------------------------
// // Parsing function
// std::map<int,PrimaryInfo> ParsePrimaryTextFileForPrimaries(std::string filename)
// {
//   std::map<int,PrimaryInfo> output;
//   std::ifstream infile;
//   infile.open(filename.c_str());
//   std::string wholeLine;

//   //Begin loop through file
//   while(1){
//     if(!infile.good()) break;
//     if(infile.is_open()){
//       std::getline(infile,wholeLine);
      
//       //Tokenize the string (split between spaces)
//       stringstream check1(wholeLine);
//       string token;
//       std::vector<std::string> tokens;
//       while(getline(check1,token,' ')){
// 	tokens.push_back(token);
//       }
//       if( tokens.size() == 0 ) break;
	  
//       //If we're on the first line of the file
//       if( tokens[0].find("Run") != std::string::npos ){
// 	continue;
//       }

//       //Here, it's simpler since there's one line per event (assuming only a single run)
//       //So we can use the event id as an index.
//       PrimaryInfo thePrim;
//       thePrim.runID = std::atoi(tokens[0].c_str());
//       thePrim.eventID = std::atoi(tokens[1].c_str());
//       thePrim.particleName = tokens[2];
//       thePrim.energy_eV = std::atof(tokens[3].c_str());
//       thePrim.X_mm = std::atof(tokens[4].c_str());
//       thePrim.Y_mm = std::atof(tokens[5].c_str());
//       thePrim.Z_mm = std::atof(tokens[6].c_str());
//       thePrim.T_ns = std::atof(tokens[7].c_str());
//       output.emplace(thePrim.eventID,thePrim);
//     }
//   }
//   return output;
// }



