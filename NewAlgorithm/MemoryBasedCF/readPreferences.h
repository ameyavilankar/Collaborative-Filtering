#ifndef READ_PREFERENCES
#define READ_PREFERENCES

// readPreferences.h -v1.0
// AUTHOR AMEYA VILANKAR

#include <map>
#include <ctime>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wctype.h>

#include "utilityfunctions.h"

// Used to Load the dataset
int loadDataSet(char *moviefile, char *ratingsfile, std::map<long, std::map<std::string, double> >& userToMovie);

#endif
