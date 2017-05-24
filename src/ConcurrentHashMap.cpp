#include "ConcurrentHashMap.hpp"

// const int SIZE_TABLE = 26;
#define SIZE_TABLE 26

ConcurrentHashMap::ConcurrentHashMap() {
  for (int i = 0; i < SIZE_TABLE; ++i) {
    Lista< pair<string, unsigned int> > * list = new Lista< pair<string, unsigned int> >();
    tabla.push_back(list);
  }
}
  
ConcurrentHashMap::~ConcurrentHashMap() {
  for (int i = 0; i < SIZE_TABLE; i++) {
    delete tabla[i];
  }
}

void ConcurrentHashMap::addAndInc(string key){
  unsigned int posicion = fHash(key[0]);
  bool pertenece = false;
  auto it = tabla[posicion]->CrearIt();

  while(it.HaySiguiente() && !pertenece) {
    if(it.Siguiente().first == key) {
      
      it.Siguiente().second++;
      
      pertenece = true;
    }
    it.Avanzar();
  }
  
  if (!pertenece) {
    pair<string, unsigned int> dicc_entry (key,1);
    tabla[posicion]->push_front(dicc_entry);
  }
}

bool ConcurrentHashMap::member(string key){
  unsigned int posicion = fHash(key[0]);
  auto it = tabla[posicion]->CrearIt();

  while(it.HaySiguiente()){
    if(it.Siguiente().first == key){
      return true;
    }
    it.Avanzar();
  }
  return false;
}

pair<string, unsigned int> ConcurrentHashMap::maximum(unsigned int nt) {

}


/*EJERCICIO 2.2
tome un archivo de texto y devuelva el ConcurrentHashMap cargado con las palabras del archivo. (NO CONCURRENTE)*/

ConcurrentHashMap ConcurrentHashMap::count_words(string arch){
  ConcurrentHashMap map;

  ifstream inFile;
  inFile.open(arch.c_str()); 

  string word;

  while (inFile >> word) {
    map.addAndInc(word);
  }

  inFile.close();
}

/*EJERCICIO 2.3
Implementar una función ConcurrentHashMap count words(list<string>archs), que to-
me como parámetro una lista de archivos de texto y devuelva el ConcurrentHashMap cargado
con esas palabras. Deberá utilizar un thread por archivo.*/

struct thread_data{
   //int  thread_id;
   ConcurrentHashMap* map;
   string archivo;
};


 void *llenarHashMap(void *threadarg){
  
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;

    string arch = my_data->archivo;

  ifstream inFile;
  inFile.open(arch.c_str()); 

  string word;

  while (inFile >> word){
    //todo revisar esto
    (my_data->map)->addAndInc(word);
  }

  inFile.close();
 }

// ConcurrentHashMap count_words2(list<string>archs){

//  unsigned int num_threads = archs.size();
//  pthread_t threads[num_threads];
//  ConcurrentHashMap map;

//  for (int t = 0; t < num_threads; ++t){
//    thread_data datos = {&map, archs[t]}; 

//    rc = pthread_create(&threads[t], NULL, llenarHashMap, (void *)datos);
//    if (rc){
//      printf("ERROR; return code from pthread_create() is %d\n", rc);
//      exit(-1);
//    }

//     pthread_exit(NULL);

// }


/*pthread_create arguments:
thread: An opaque, unique identifier for the new thread returned by the subroutine.
attr: An opaque attribute object that may be used to set thread attributes. You can specify a thread attributes object, or NULL for the default values.
start_routine: the C routine that the thread will execute once it is created.
arg: A single argument that may be passed to start_routine. It must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed.
*/


