#include <stdlib.h>
#include "Riostream.h"
#include "TGraphErrors.h"
#include <stdio.h>
#include "TROOT.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
using namespace std;

//vector<vector<string>> CSVParse(TString fileName, vector<vector<string>> * array)
vector<vector<string>> CSVParse(TString fileName)
{
	//in.open(Form("%sbasic.dat",fileName.Data()));
	//ifstream in("Lab8Data.csv");
	ifstream in(fileName.Data());
	string line, field1, field2, field3, field4, field5;

	vector<vector<string> > array;   // the 2D array
	vector<string> v;                // array of values for one line only

	//getline(in,line); //skip header line

	while ( getline(in,line) )    // get next line in file
	{
		v.clear();
		stringstream ss(line);

		getline(ss,field1,',');  // break line into comma delimitted fields
		getline(ss,field2,',');
		getline(ss,field3,',');
		getline(ss,field4,',');
		ss << "\n";
		getline(ss,field5,'\n');
		//field5 = ss.str();

		//field1 = atof(field.c_str());
		v.push_back(field1);  // add each field to the 1D array
		v.push_back(field2);
		v.push_back(field3);
		v.push_back(field4);
		v.push_back(field5);

		array.push_back(v);  // add the 1D array to the 2D array
	}
	in.close();
	v.clear();
	//return &array;
	return array;
}
TCanvas * c1 = new TCanvas();
TMultiGraph *mg = new TMultiGraph();
int alreadyDrew = 0;
void plotCSVHelper(string file = "test.csv", int flag = 1) {

	// Format: thickness,mm,5,0.99041,0.0301189
	vector<vector<string>> data = CSVParse(Form("%s",file.c_str()));
	//vector<vector<string>> data;
	//CSVParse(Form("%s",file.c_str()),&data);
	string title = file.substr(0,file.find(".csv"));
	int len = data.size();
	// Plot data
	//   for each entry in the vector, open the sub-vector and print the modifier to the plot name (with the units)
	//     plot the rest of the entries and error vs the number parameter
	//   done

	//std::cout<<"length of vector = "<<len<<std::endl;
	double *runnumber = new double[len];
	double *value = new double[len];
	double *error = new double[len];
	for (int i=0; i<len; i++){
		std::stringstream iss3(data[i][0]);
		iss3 >> runnumber[i];
		std::stringstream iss4(data[i][1]);
		iss4 >> value[i];	
		std::stringstream iss5(data[i][2]);
		iss5 >> error[i];	
	}
	
	double intVal = 0.0;
	double intErr = 0.0;
	double *integrated_value = new double[len];
	double *integrated_error = new double[len];

	printf("%s per run",title.c_str());
	for (int j=0;j<len;j++){
		printf("%f: %f +- %f \n",runnumber[j],value[j],error[j]);
		intVal+=value[j];
		integrated_value[j]=intVal;
		intErr+=error[j];
		integrated_error[j]=intErr;
	}
	
	// Instantaneous
	TGraphErrors * graph = new TGraphErrors(len,runnumber,value,0,error);
	if (flag==1){
		graph->SetTitle(Form("%s",title.c_str()));
		graph->SetMarkerColor(alreadyDrew+1);
		graph->SetMarkerStyle(21);
		//graph->Fit("pol1");
		//graph->SetLineColor(alreadyDrew+1);
		//graph->GetFunction("pol1")->SetLineColor(alreadyDrew+1);
		//gStyle->SetOptFit(0111);
		//graph->SetMarkerSize(.4);
		mg->Add(graph);
		mg->Draw("APE");
		//c1->BuildLegend(0.125,0.8,0.5,0.9);
		//c1->Update();
		//TPaveStats * stats = (TPaveStats*)graph->GetListOfFunctions()->FindObject("stats");
		//stats->SetTextColor(alreadyDrew);
		//stats->SetX1NDC(0.333*(alreadyDrew-1)); 
		//stats->SetX2NDC(0.333*alreadyDrew);
		//stats->SetY1NDC(0.9);
		//stats->SetY2NDC(1.0);
		mg->GetXaxis()->SetTitle(Form("Run Number"));
		mg->GetYaxis()->SetTitle(Form("%s",title.c_str()));
		mg->SetTitle(Form("%s per Run",title.c_str()));
		alreadyDrew++;
		c1->Update();
		c1->SaveAs(Form("Plot_%s.pdf",title.c_str()));
	}
	TGraphErrors * graph_int = new TGraphErrors(len,runnumber,integrated_value,0,integrated_error);
	if (flag==2){
		// Integrated
		graph_int->SetTitle(Form("%s",title.c_str()));
		graph_int->SetMarkerColor(alreadyDrew+1);
		graph_int->SetMarkerStyle(21);
		mg->Add(graph_int);
		mg->Draw("APE");
		mg->GetXaxis()->SetTitle(Form("Run Number"));
		mg->GetYaxis()->SetTitle(Form("Integrated %s",title.c_str()));
		mg->SetTitle(Form("%s integrated over runs",title.c_str()));
		alreadyDrew++;
		c1->Update();
		c1->SaveAs(Form("Plot_integrated_%s.pdf",title.c_str()));
	}
	data.clear();
	delete[] runnumber;
	delete[] value;
}

void plotCSV(int flag=0, string file1 = "file1.csv", string file2 = "file2.csv", string file3 = "file3.csv", string file4 = "file4.csv", string file5 = "file5.csv", string file6 = "file6.csv", int numLines=1) {
    if (numLines==0 || flag=0){
        printf("USAGE: .x plotCSV(int flag, \"file1.csv\",\"file2.csv\"\n, etc. , int n_files");
    }
    else{
        plotCSVHelper(file1,flag);
        if (numLines>1){
            plotCSVHelper(file2,flag);
        }
        if (numLines>2){
            plotCSVHelper(file3,flag);
        }
        if (numLines>3){
            plotCSVHelper(file4,flag);
        }
        if (numLines>4){
            plotCSVHelper(file5,flag);
        }
    }
}
