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

#define SIZE_TABLE 26

using namespace std;

class ConcurrentHashMap {
	public:
		ConcurrentHashMap();
		~ConcurrentHashMap();
		void addAndInc(string key);
		bool member(string key);
		pair<string, unsigned int> maximum(unsigned int nt);
		static pair<string, unsigned int> maximum(unsigned int p_archivos, unsigned int p_maximos, list<string>archs);
		static void* maximum_thread(void *thread_args);
		static void* mergeHashMap(void *thread_args);
		static ConcurrentHashMap count_words(string arch);
	  static ConcurrentHashMap count_words(list<string> archs);
		static ConcurrentHashMap count_words(unsigned int num_threads, list<string> archs);
		static void* llenarHashMap(void *thread_args);
		static void* llenarHashMap2(void *thread_args);
		vector< Lista < pair<string, unsigned int> >* > tabla;

	private:
		unsigned int fHash(const char letra) {
	  	return (int)letra - 97;
		}

		struct thread_data { 
		  unsigned int thread_id;
		  ConcurrentHashMap* map;
		  string archivo;
		};

		struct thread_data2 { 
		  unsigned int thread_id;
		  ConcurrentHashMap* map;
		  list<string>::iterator* it_inicio;
		  list<string>::iterator it_fin;
		  pthread_mutex_t* mutex_it;
			};

		struct thread_data_max { 
		  unsigned int thread_id;
		  ConcurrentHashMap* map;
		 	vector< pair<string, unsigned int> >* maximos;
		  atomic<int>* fila;
		};

		struct thread_data_merge { 
		  unsigned int map_id;
		  unsigned int thread_id;
		  ConcurrentHashMap* map;
		  ConcurrentHashMap* merge_map;
		  atomic<int>* fila;
		};

		pthread_mutex_t mutex[SIZE_TABLE];
		pthread_mutex_t mutex_maximum;

};

#endif /* LISTA_ATOMICA_H__ */