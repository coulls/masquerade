*************************************
This repository includes the C++ source code for a program that uses
semi-global sequence alignment to align commands as provided in the
Schonlau et al. dataset.

Disclaimer:

This is research code, and should be used only to investigate the
usage of sequence alignment to detect masquerade attacks.  Users of
the code should know that it has not undergone significant
software testing, and should not be used beyond demonstration
purposes.

To Compile:

g++ -o seqalign main.cpp

Note that this program is cross-platform compatible, though there may be
warnings on Unix systems.

To Run:

Ensure that mask.dat (the Schonlau command sequence file) and summary.dat
(the matrix of ground truth for masquerades) are in the current working
directory.  Run seqalign with no parameters.  The program will
run through sensitivity levels from 0 to 100 by increments of 5 and
print out the performance of each user, plus the overall performance
at the given sensitivity level.

Defaults:
By default, the program is set to use the heuristic speed up mechanism.  
To remove this, change line 110 in main.cpp so that the final parameter
is 'false'.  Also, this version of the sequence alignment program is
set to use signature updating with the binary scoring system.  It cannot
be set to do otherwise.


Contact:
Papers and related material available at https://www.scottcoull.com
***************************************
