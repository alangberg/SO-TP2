#include <iostream>
#include <cstdlib>
#include "ConcurrentHashMap.hpp"
#include <string> 
using namespace std;

int main(int argc, char **argv) {
	pair<string, unsigned int> p;
	list<string> aux;
	int i = 0;
	int cant_archs = atoi(argv[1]);
	int instancia = atoi(argv[2]);
	int cant_words = atoi(argv[3]);
	cout<<"instancia "<<instancia<<": "<<cant_archs<<" archivos de "<<cant_words<<" palabras cada uno"<<endl;
	while(i < cant_archs){
		string i_string = to_string(i);
		aux.push_back("test01_"+ i_string +".txt"); 

		ConcurrentHashMap h;
		h = ConcurrentHashMap::count_words(aux.back());
		p = h.maximum(5);
		cout << "maximum archivo " << i <<": "<<p.first <<endl;
		i++;
	}
	cout<<""<<endl;
	p = ConcurrentHashMap::maximum(5, 5, aux);
	// // cout<<"asdad"<<endl;
	cout<< "maximum global: " << p.first << endl;
	cout<< " "<<endl;
	// cout << "maximum archivo i "<<instancia<<": ("<<p.first << " " << p.second<<")" << endl;

	return 0;
}

