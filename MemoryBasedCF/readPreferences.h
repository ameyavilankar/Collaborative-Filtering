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
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wctype.h>

#include "utilityfunctions.h"
#include "distances.h"

// Used to Load the dataset
int loadDataSet(char *moviefile, char *ratingsfile, std::map<long, std::map<std::string, double> >& userToMovie);
int saveToFile(char* filename, const std::map<long, std::vector<std::pair<std::string, double> > >& recommendations);
std::map<std::string, std::map<long, double> > transformPrefs(const std::map<long, std::map<std::string, double> >& userToMovie);
std::vector<std::pair<long, double> > topMatches(std::map<long, std::map<std::string, double> >& userToMovie, long user, int n = 20);
std::vector<std::pair<long, double> > positiveCorrelationUsers(const std::map<long, std::map<std::string, double> >& userToMovie, long user, int n = 20);
std::vector<std::pair<std::string, double> > getRecommendations(const std::map<long, std::map<std::string, double> >& userToMovie, long user);


#endif
