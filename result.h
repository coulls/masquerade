#ifndef RESULT_H
#define RESULT_H

#include <vector>
#include <string>
using namespace std;

class result{
private:
	//Vector that holds all of our results for each alignment
	vector<bool> result_vector;
public:

	//Basic constructor does nothing
	result(){}

	//Clears out the results for reuse
	void clear(){
		result_vector.clear();
	}

	//Get the results vector directly
	void set_result(const bool &res){
		result_vector.push_back(res);
	}

	//Updates the counts for true positives, etc...
	void get_results(ifstream &infile, int & user, int & fp, int &fn, int &tp, int & tn){
		int i;	//Iterator
		string temp_string;	//Holds the ground truth from the summary file

		//Go through the summary/ground truth file by column
		for(i=0;i<100;++i){
			//Fancy stuff to make use read down a specific column of the matrix
			infile.seekg((i*101)+(user*2),ios::beg);

			infile >> temp_string;
	
			//The real answer was a negative
			if(temp_string=="0"){
				++tn;
				//We answered incorrectly, so record it
				if(result_vector[i]!=false){
					++fp;
				}
			}
			//Real answer was a positive
			else{
				++tp;
				//We answered incorrectly, so record it
				if(result_vector[i]!=true){
					++fn;
				}
			}
		}
	}
};

#endif

