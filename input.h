#ifndef INPUT_H
#define INPUT_H

#include <fstream>
#include <string>
using namespace std;

//Read the input one token at a time
string read_input(ifstream & input){
	string command;

	input >> command;

	return command;
}

#endif


