#include <iostream>
#include <fstream>
#include "ListaAtomica.hpp"

using namespace std;

#define THREADS_NUM 10
#define debug 0

Lista<string> lista;
 
// funcion que ejecutan los threads
void *llenarLista(void* thread_id) {

  int* my_id = (int*) thread_id;
  ifstream inFile;
  inFile.open("corpus"); 
  string word;

  // leemos archivo y agregamos palabras a lista
  while (inFile >> word)
    lista.push_front(word);

  //cerramos archivo
  inFile.close();

  if (debug) printf("[%d] Thread finalizado.\n", *my_id);
  pthread_exit(NULL);
}


int main(void) {
  
  pthread_t threads[THREADS_NUM];  // aca se guardan los threads
  int thread_id[THREADS_NUM];      // aca se guardan los threads_id
  int return_code;

  // en cada iteracion creamos un nuevo thread
  for (int t = 0; t < THREADS_NUM; t++) {
    thread_id[t] = t;
    return_code = pthread_create(&threads[t], NULL, llenarLista, &thread_id[t]);
    if (debug) printf("Thread %d creado.\n", t);
  
    if (return_code) {
      printf("[ERROR] pthread_create() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  // esperamos a que todos los threads finalicen
  void *status;
  for (int t = 0; t < THREADS_NUM; t++) {
    return_code = pthread_join(threads[t], &status);
    if (return_code) {
      printf("[ERROR] pthread_join() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  //imprimimos valores
  for (auto it = lista.CrearIt(); it.HaySiguiente(); it.Avanzar()) {
    auto t = it.Siguiente();
    cout << t << endl;
  }

  return 0;
}