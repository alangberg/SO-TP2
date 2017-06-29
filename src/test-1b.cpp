#include <iostream>
#include <cstdlib>
#include "ConcurrentHashMap.hpp"

using namespace std;

int main(int argc, char **argv) {
  ConcurrentHashMap h;
  pair<string, unsigned int> p;
  list<string> l = { "corpus-0", "corpus-1", "corpus-2", "corpus-3", "corpus-4" };

  if (argc != 3) {
    cerr << "uso: " << argv[0] << " #threads " << endl;
    return 1;
  }

  h = ConcurrentHashMap::count_words(l);
  p = h.maximum(atoi(argv[1]));

  cout << p.first << " " << p.second << endl;

  return 0;
}