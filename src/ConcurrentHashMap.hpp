#ifndef CONCURRENT_HASHMAP_H__
#define CONCURRENT_HASHMAP_H__

#include <atomic>
#include <vector>
#include <string>
#include <utility>
#include <list>
#include <iostream>
#include <fstream>
#include <mutex> 
#include <pthread.h>
#include <stdio.h>

#include "ListaAtomica.hpp"
using namespace std;

class ConcurrentHashMap{
public:
	ConcurrentHashMap();
	~ConcurrentHashMap();
	void addAndInc(string key);
	bool member(string key);
	pair<string, unsigned int> maximum(unsigned int nt);
	static ConcurrentHashMap count_words(string arch);
	vector< Lista < pair<string, unsigned int> >* > tabla;

private:
	unsigned int fHash(const char letra) {
  	return (int)letra - 97;
	}

};

#endif /* LISTA_ATOMICA_H__ */