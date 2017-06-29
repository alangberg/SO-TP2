#include <iostream>
#include <cstdlib>
#include "ConcurrentHashMap.hpp"

using namespace std;

int main(int argc, char **argv) {
  ConcurrentHashMap h;
  list<string> l = {"input"};
  int i;

  if (argc != 2) {
    cerr << "uso: " << argv[0] << " #threads" << endl;
    return 1;
  }

  h = ConcurrentHashMap::count_words(l);
  pair<string, unsigned int> max = h.maximum(atoi(argv[1]));

  printf("Maximo: (%s,%d)\n", max.first.c_str(), max.second);

  return 0;
}

