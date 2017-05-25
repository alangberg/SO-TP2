#include "ConcurrentHashMap.hpp"

ConcurrentHashMap::ConcurrentHashMap() {
  for (int i = 0; i < SIZE_TABLE; i++) {
    Lista< pair<string, unsigned int> > * list = new Lista< pair<string, unsigned int> >();
    tabla.push_back(list);
    pthread_mutex_init(&mutex[i], NULL);
  }
}
  
ConcurrentHashMap::~ConcurrentHashMap() {
  // for (int i = 0; i < SIZE_TABLE; i++) {
  //   cout<<"borrando "<<i<<endl;
  //   delete tabla[i];
  //   cout<<"chau "<<i<<endl;
  // }
}


void ConcurrentHashMap::addAndInc(string key) {
  unsigned int posicion = fHash(key[0]);
  bool pertenece = false;
  auto it = tabla[posicion]->CrearIt();
  pthread_mutex_lock(&mutex[posicion]);

  while (it.HaySiguiente() && !pertenece) {
    if (it.Siguiente().first == key) {
      
      it.Siguiente().second++;
      
      pertenece = true;
    }
    it.Avanzar();
  }
  
  if (!pertenece) {
    pair<string, unsigned int> dicc_entry (key,1);
    tabla[posicion]->push_front(dicc_entry);
  }

  pthread_mutex_unlock(&mutex[posicion]);
}

bool ConcurrentHashMap::member(string key){
  unsigned int posicion = fHash(key[0]);
  auto it = tabla[posicion]->CrearIt();

  while(it.HaySiguiente()) {
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
  return map;
}

/*EJERCICIO 2.3
Implementar una función ConcurrentHashMap count words(list<string>archs), que to-
me como parámetro una lista de archivos de texto y devuelva el ConcurrentHashMap cargado
con esas palabras. Deberá utilizar un thread por archivo.*/

struct thread_data { 
  unsigned int thread_id;
  ConcurrentHashMap* map;
  string archivo;
};

void *llenarHashMap(void *thread_args) {
  struct thread_data* my_data;
  my_data = (struct thread_data*) thread_args;

  string arch = my_data->archivo;

  ifstream inFile;
  inFile.open(arch.c_str()); 

  string word;

  while (inFile >> word)
    (my_data->map)->addAndInc(word);

  inFile.close();

  printf("[%d] Thread dead.\n", my_data->thread_id);
  pthread_exit(NULL);
}

ConcurrentHashMap ConcurrentHashMap::count_words(list<string> archs) {

  unsigned int num_threads = archs.size();
  pthread_t threads[num_threads];
  ConcurrentHashMap map;

  int rc;
  list<string>::iterator it = archs.begin();
  for (unsigned int t = 0; t < num_threads; t++) {
    thread_data args = {t, &map, *it};  
    rc = pthread_create(&threads[t], NULL, llenarHashMap, (void*) &args);
    printf("Thread %d created.\n", t);
  
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
    it++;
  }

  void *status;
  for (int t = 0; t < num_threads; t++) {
    rc = pthread_join(threads[t], &status);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
  }

  return map;
}

/*pthread_create arguments:
thread: An opaque, unique identifier for the new thread returned by the subroutine.
attr: An opaque attribute object that may be used to set thread attributes. You can specify a thread attributes object, or NULL for the default values.
start_routine: the C routine that the thread will execute once it is created.
arg: A single argument that may be passed to start_routine. It must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed.
*/


