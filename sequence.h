#ifndef SEQUENCE_H
#define SEQUENCE_H

class sequence{
private:
	deque<string> window;	//Holds our sequences
	int count;	//Counter for the matches (heuristic only)
public:
	//Basic constructor does nothing
	sequence(){}

	//Constructor with an initial sequence
	sequence(deque<string> win){
		window=win;
	}
	
	//Copy constructor
	sequence(const sequence & rhs){
		window=rhs.window;
	}

	//Clear the window for reuse
	void clear(){
		window.clear();
	}

	//Set and get the window
	void set_window(deque<string> & w){
		window=w;
	}

	deque<string> * get_window(){
		return &window;
	}

	//Keep track of the counts for the number of possible matches
	//Only used with heuristic
	void inc_count(int a){
		count+=a;
	}

	int get_count(){
		return count;
	}

	void clear_count(){
		count=0;
	}
	
	//Get a subsequence of the sequence
	deque<string> get_window_range(int start, int end){
		deque<string> temp;
		int i;
		if(end>(int)window.size()){
			end=window.size();
		}
		for(i=start;i<end;++i){
			temp.push_back(window[i]);
		}
		return temp;
	}

};

//Comparison operator
bool operator < (sequence rhs, sequence lhs){
	return (rhs.get_count() < lhs.get_count());
}
#endif


		
