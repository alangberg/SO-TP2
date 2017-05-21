#ifndef CONCURRENT_HASHMAP_H__
#define CONCURRENT_HASHMAP_H__

#include <atomic>
#include <vector>
#include <string>
#include <pair>
#include <iostream>
#include <mutex> 
#include "ListaAtomica.hpp"
using namespace std;

int SIZE_TABLE = 26;
//typedef pair<string, unsigned int> Entry;



class ConcurrentHashMap{
public:
	ConcurrentHashMap();
	~ConcurrentHashMap();
	void addAndInc(string key);

private:
	Lista < pair<string, unsigned int> >[SIZE_TABLE] hash_table;
	unsigned int fHash(const unsigned int n) const;

};

ConcurrentHashMap::ConcurrentHashMap() {
	hash_table = Lista < pair<string, unsigned int> >[SIZE_TABLE];
	for (int i = 0; i < SIZE_TABLE; ++i){
		Lista();
	}
}

unsigned int fHash(const char letra) const{
	return (int)letra - 97;
}

void ConcurrentHashMap::addAndInc(string key){
	
	
}





#endif /* LISTA_ATOMICA_H__ */
