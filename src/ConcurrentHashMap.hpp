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
	bool member(string key);


private:
	Lista < pair<string, unsigned int> >[SIZE_TABLE] tabla;
	unsigned int fHash(const unsigned int n) const;

};


ConcurrentHashMap::ConcurrentHashMap() {
	tabla = Lista < pair<string, unsigned int> >[SIZE_TABLE];
	for (int i = 0; i < SIZE_TABLE; ++i){
		Lista();
	}
}

unsigned int fHash(const char letra) const{
	return (int)letra - 97;
}

void ConcurrentHashMap::addAndInc(string key){
	unsigned int posicion = fHash(key[0]);
	bool pertenece = false;
	Iterador it = tabla[posicion].CrearIt();
	
	while(it.HaySiguiente()){
		if(it.Siguiente().first == key){
			it.Siguiente().second()++;
			pertenece = true;
		}
		it.Avanzar();
	}
	
	if (!pertenece){
		pair<string, unsigned int> dicc_entry (key,1);
		tabla[posicion].push_front(dicc_entry);
	}
}

bool ConcurrentHashMap::member(string key){
	unsigned int posicion = fHash(key[0]);
	bool pertenece = false;
	Iterador it = tabla[posicion].CrearIt();

	while(it.HaySiguiente()){
		if(it.Siguiente().first == key){
			pertenece = true;
		}
		it.Avanzar();
	}
	
	return pertenece;
}




#endif /* LISTA_ATOMICA_H__ */
