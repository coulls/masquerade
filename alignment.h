#ifndef UPDATE_H
#define UPDATE_H

#pragma warning(disable: 4786)
#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <math.h>
#include <time.h>
#include <queue>
#include <algorithm>
#include "sequence.h"
#include "result.h"


using namespace std;

class alignment{
private:
	vector<sequence> signature;	//Stores our sequences
	int avg;			//Holds the average score as we proceed
	int count;			//Holds the total number of alignments that we have performed
	result result_vec;		//Holds the results of our detection
	
	//Map that tells us whether or not a command is in the signature at all
	map<string, bool> cmap;

	map<string, map<int,int> > command_map;

	//A quick utility function that figures out the maximum of three numbers
	//Useful when doing dynamic programming
	int max_num(const int & one, const int & two, const int & three){
		if(one>two){
			if(one>three){
				return one;
			}
			return three;
		}
		else if(two>three){
			return two;
		}
		else{
			return three;
		}
	}

	//Finds the minimum
	int min(const int & one, const int & two){
		if(one<two){
			return one;
		}
		return two;
	}

public:
	//Basic constructor just initializes our average and counter
	alignment(){
		avg=0;count=0;
	}

	//Clears out our object so we can reuse it
	void clear(){
		avg=0;count=0;
		signature.clear();
		result_vec.clear();
		command_map.clear();
		cmap.clear();
	}

	//Updates the counters
	void get_results(ifstream &input, int &user, int &fp, int &fn, int &tp, int &tn){
		result_vec.get_results(input,user, fp, fn, tp, tn);
	}

	void train(deque<string> & window){
		int i;	//Iterator
		
		//Add the sequence containing the window
		signature.push_back(sequence(window));


		//Go through each of the commands and keep a count of how often they occur
		for(i=0;i<(int)window.size();++i){
			command_map[window[i]][signature.size()-1]++;
			//Figure out if the command has been used by the user at all
			cmap[window[i]]=true;
		}
	}

	void create_base_threshold(){
		int i;		//Iteratore
		int rand_start1,rand_start2;	//Holds the random values for picking the sequences

		deque<string> temp;

		//Initialize the random function
		srand(time(NULL));
		rand();

		//Do 20 alignments to get an average alignment score
		for(i=0;i<20;++i){
			rand_start1=(int)((48.0*rand())/(RAND_MAX+1.0));
			while((*signature[rand_start1].get_window()).empty()){
				rand_start1=(int)((48.0)*rand()/(RAND_MAX+1.0));
			}
			
			
			rand_start2=(int)((48.0*rand())/(RAND_MAX+1.0));
			
			//Make sure our second random value does not intersect with the first sequence
			while(rand_start2==rand_start1 || rand_start2==(rand_start1-1) || 
				rand_start2==(rand_start1+1)|| (*signature[rand_start2].get_window()).empty()){
				
				rand_start2=(int)((48.0*rand())/(RAND_MAX+1.0));
			}

			temp=signature[rand_start2].get_window_range(0,100);

			//Record the alignment score between our first window and the first 100 commands of the second
			avg+=single_align(signature[rand_start1].get_window(),&temp,true,0,0);
			++count;
		}
		
	}


	void add_window(deque<string> & window, float percent, bool heuristic){

		int i;		//Iterator
		int match;	//Keeps track of the current alignment score
		int max;	//Keeps track of the best alignment score

		//Iterators for our command maps
		map<int,int>::iterator it;
		map<string,int>::iterator it1;

		//Vector 
		vector<int> temp;
		map<string,int> temp_map;
		

		//Initialize our scores, etc.
		match=max=0;

		if(heuristic){

			//Go through and figure out the counts each of the commands in the test sequence
			for(i=0;i<(int)window.size();++i){
				++temp_map[window[i]];
			}
			
			//Go through each of the test commands...
			for(it1=temp_map.begin();it1!=temp_map.end();++it1){
				//And go through each of the signature sequences that contain that command
				for(it=command_map[(*it1).first].begin();it!=command_map[(*it1).first].end();++it){
					//Update the 'match' count for each of the sequences
					signature[(*it).first].inc_count(min((*it).second,(*it1).second));
				}
			}

			max=0;
			//Go through the sig. sequences and find the ones with the best counts
			for(i=0;i<(int)signature.size();++i){
				if(signature[i].get_count()>max){
					max=signature[i].get_count();
					temp.clear();
					temp.push_back(i);
				}
				else if(signature[i].get_count()==max){
					temp.push_back(i);
				}
			}

			for(i=0;i<(int)signature.size();++i){
				signature[i].clear_count();
			}
			max=0;
		}
		else{
			//If we don't use the heuristic just use the entire sig.
			for(i=0;i<(int)signature.size();++i){
				temp.push_back(i);
			}
		}
		
		//Go through each of our signature sequences
		for(i=0;i<(int)temp.size();++i){

			//Align the test and signature sequences
			match=single_align(signature[temp[i]].get_window(),&window,false,temp[i],percent);
			
			//Record only the best match location	
			if(match>max){
				max=match;
			}
		}

		//If the score is below our threshold record the sequence as a masquerade
		if(max<=(((float)avg/count)*percent)){
			result_vec.set_result(true);

		}
		else{
			result_vec.set_result(false);
		}
		
		//Make sure to keep updating our average over time
		++count;
		avg+=max;
	}

	
	int single_align(deque<string> * window1,deque<string> * window2,bool sig, int index, float percent){
		int j,k;	//Iterators
		
		int win1_size, win2_size;	//Size of our two sequences
		int top, left, diag;	//Keeps the dynamic programming values
		int max;	//Holds the maximum value of our three choices for movement

		//Vectors for our columns and overall matrix
		vector<int> column;
		vector<vector<int> > matrix;

		//Vectors for our columns and matrix for tracing back the alignment
		vector<int> trace_column;
		vector<vector<int> > trace_matrix;

		//Get the sequence sizes
		win1_size=(*window1).size();
		win2_size=(*window2).size();
		
		for(j=0;j<=win1_size;++j){
			for(k=0;k<=win2_size;++k){
				//If we are in the upper left corner, start with a zero
				if(j==0){
					column.push_back(0);
					trace_column.push_back(0);
				}
				else if(k==0){
					column.push_back(0);
					trace_column.push_back(1);
				}
				else{
					//If we are in the last row or last column, no gap penalties
					if(j==win1_size || k==win2_size){
						top=column[k-1];
						left=matrix[j-1][k];
					}
					else{
						//Otherwise, -2 gap penalty for gaps in signature
						top=column[k-1]-2;
						if(top<0){
							top=0;
						}
						//-2 for gaps in test sequences
						left=matrix[j-1][k]-2;
						if(left<0){
							left=0;
						}
					}
					//Get the diagonal score in our matrix
					diag=matrix[j-1][k-1];

					//If the current test command is in the current signature spot
					//give a match reward
					if((*window1)[j-1].find((*window2)[k-1])!=string::npos){
						diag+=2;
					}
					//If the command occurred anywhere in the user's sig, give a slight
					//mismatch reward
					else if(cmap[(*window2)[k-1]]){
						diag+=1;
					}
					//Otherwise, give a mismatch penalty
					else{
						diag-=1;
					}

					//Add the maximum score to our matrix and record the movement in our tracing matrix
					max=max_num(top,left,diag);
					if(max==top){
						trace_column.push_back(0);
						column.push_back(top);
					}
					else if(max==left){
						trace_column.push_back(1);
						column.push_back(left);
					}
					else{
						trace_column.push_back(2);
						column.push_back(diag);
					}
						
				}
			}

			//Push the latest column onto our matrix
			matrix.push_back(column);
			column.clear();
			trace_matrix.push_back(trace_column);
			trace_column.clear();

		}

		
		//Update sensitivity setting -- depends on what percentage of your average you want to use
		//Set accordingly -- currently set to the masquerade threshold
		float sensitivity=(float)((1-percent)*0)+percent;
		
		//Now we are going to start walking back through our alignment
		//Only do this if we are doing a real alignment, not training (i.e., sig==False)
		//and only if it is better than our threshold
		if(matrix[win1_size][win2_size]>(((float)avg/count)*sensitivity) && !sig){
			//Start at the lower right corner of the matrix
			j=win1_size;
			k=win2_size;

			
			while(!(j==0 && k==0)){
				//Move up because we have a gap in our signature
				if(trace_matrix[j][k]==0){
					--k;
				}
				//Move left because we have a gap in the test sequence
				else if(trace_matrix[j][k]==1){
					--j;
				}
				//We aligned here so move diagonally
				else{
					//If this was a mismatch where the command previously occurred 
					//in the user's lexicon, then we want to append the new 
					//command at the signature position
					if(matrix[j][k]==matrix[j-1][k-1]+1){
						(*window1)[j-1]=(*window1)[j-1]+(*window2)[k-1];
						++command_map[(*window2)[k-1]][index];
					}
						
					//If this was a mismatch, and our score is sufficiently large, then
					//add this command to the user's lexicon
					else if(matrix[j][k]==matrix[j-1][k-1]-1){
						cmap[(*window2)[k-1]]=true;
					}
					--k; --j;
				}
			}
		}

		//Return our score
		return matrix[win1_size][win2_size];
	}


};

#endif

