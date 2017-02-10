/**********************************************************
 * Main file for example sequence alignment 
 * masquerade detection code
 *
 * Disclaimer:  This is research code and is meant to illustrate 
 * the interesting usage of sequence alignment for detecting masquerading.  
 * Please do not distribute this code or use in commercial applications.
 * For related information:  http://www.cs.jhu.edu/~coulls/
 *
 * Contact:  Scott Coull -- coulls at cs dot jhu dot edu
 *
 * *********************************************************/

//Includes of sequence alignment files
#include "input.h"
#include "alignment.h"
#include "result.h"
#include "sequence.h"

//Common library includes
#include <fstream>
#include <time.h>
#include <deque>
#include <iostream>
using namespace std;

int main(){
	int fp, fn, tp, tn;	//Maintain the false positive, false negative, 
				//true positive, and true negative counts per user
	
	float totalfp,totalfn,totaltp,totaltn;	//Maintain the totals over all of the users
		
	int i,j,k,l;		//Iterators

	ifstream input, input2;	//Input file streams

	//Use Deques to represent the sequence of commands
	deque<deque<string> > signature;
	deque<deque<string> >test;
	deque<string> temp;

	//Create an alignment object which deals with all of our alignments, training, etc.
	alignment alignment_obj;
	
	//Zero out our counts
	fp=fn=tp=tn=0;
	totalfp=totalfn=totaltp=totaltn=0.0;

	//Go through a variety of sensitivity levels by 5% increments
	for(l=0;l<=100;l+=5){

		//Open the masquerading file and the ground truth file, respectively
		input.open("mask.dat");
		input2.open("summary.dat");
	
		//Go through each user in the file
		for(i=0;i<50;++i){
			
			//Get the user's signature
			for(j=0;j<49;++j){
				
				//We are going to break the user's signature into 50 overlapping
				//windows of 200 commands each
				while(temp.size()<200){

					//We use the "/" to delimit commands since when we do updating it is 
					//possible to have multiple commands per spot in the deque
					temp.push_back("/"+read_input(input)+"/");
				}
				//Push back our current window into the signature deque
				signature.push_back(temp);

				//Get rid of the first 100 commands in the deque to 'slide' the window
				for(k=0;k<100;++k){
					temp.pop_front();
				}
			}

			//Clear out our temp deque to make room for the test sequences
			temp.clear();

			//Get the 100 test sequences of size 100 each
			for(j=0;j<100;++j){
				for(k=0;k<100;++k){
					temp.push_back("/"+read_input(input)+"/");
				}
				test.push_back(temp);

				//This time we are not overlapping the windows, so just clear the whole thing
				temp.clear();
			}


			//"Train" our alignment object by adding the sequence, and performing some calculations
			//on the commands it contains
			for(j=0;j<49;++j){
				alignment_obj.train(signature[j]);	
			}

			//Use randomly selected portions of the signature to get an idea of the average alignment score
			//among 'good' sequences
			alignment_obj.create_base_threshold();
		
		
			
			//Add our test sequences to the alignment object, along with the sensitivity
			//This will actually perform the alignment for each object
			//Last argument specifies whether we are using a heuristic or not.
			for(j=0;j<100;++j){
				alignment_obj.add_window(test[j],(float)l/100, true);
			}

			//Have the alignment object calculate how well we did
			alignment_obj.get_results(input2,i, fp, fn, tp, tn);
			cout << "User " << i << " complete!  FP:  " << fp << " TN: " << tn  << " FN:  " << fn << " TP: " << tp << endl;
		
			//Clear the alignment object and the sequences
			alignment_obj.clear();
			signature.clear();
			test.clear();

			//Keep track of our total false positive rate as the fraction of known 
			//'good' sequences that were flagged as masquerades
			totalfp+=(float)fp/tn;
			++totaltn;

			//If we have any actual 'bad' sequences, then get our false negative rate as 
			//the fraction of 'bad' sequences that we missed
			if(tp>0){
				totalfn+=(float)fn/tp;
				++totaltp;
			}

			//Reset our per user counters
			fn=fp=tp=tn=0;

		}

		//Close out our two input files for this round -- basically we just want to 
		//reset the pointer to the front of the file...		
		input.close();	
		input2.close();
		
		//Print out our results over all of the users here
		cout << "Sensitivity: " << l << endl;
		cout << "False Positive:  " << (float)totalfp/totaltn << endl;
		cout << "False Negative:  " << (float)totalfn/totaltp << endl;

		//Reset our overall counts
		totalfp=totalfn=totaltp=totaltn=0.0;
	}
	return 0;
}



