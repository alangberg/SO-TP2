#include "ConcurrentHashMap.hpp"

#define debug 0

ConcurrentHashMap::ConcurrentHashMap() {
  for (int i = 0; i < SIZE_TABLE; i++) {
    Lista< pair<string, unsigned int> > * list = new Lista< pair<string, unsigned int> >();
    tabla.push_back(list);
    pthread_mutex_init(&mutex[i], NULL);
  }
  pthread_mutex_init(&mutex_buscando_max, NULL);
}
  
ConcurrentHashMap::~ConcurrentHashMap() {
  // for (int i = 0; i < SIZE_TABLE; i++) {
    // cout<<"borrando "<<i<<endl;
    // delete tabla[i];
    // cout<<"chau "<<i<<endl;
  // }
}


void ConcurrentHashMap::addAndInc(string key) {
  unsigned int posicion = fHash(key[0]);
  bool pertenece = false;
  pthread_mutex_lock(&mutex[posicion]);
  auto it = tabla[posicion]->CrearIt();

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

void *ConcurrentHashMap::maximum_thread(void *thread_args) {
  struct thread_data_max* my_data;
  my_data = (struct thread_data_max*) thread_args;
  pthread_mutex_lock(my_data->mutex_inicio);
  
  unsigned int fila_actual;
  while ((fila_actual = atomic_fetch_add(my_data->fila, 1)) < SIZE_TABLE) {
    Lista< pair<string, unsigned int> >::Iterador it = my_data->map->tabla[fila_actual]->CrearIt();
    
    while (it.HaySiguiente()) {
      pair<string, unsigned int> t = it.Siguiente();
      if (t.second > (*my_data->maximos)[fila_actual].second)
        (*my_data->maximos)[fila_actual] = t;
      it.Avanzar();
    }
  }

  pthread_mutex_unlock(my_data->mutex_inicio);
  if (debug) printf("[%d] Thread dead.\n", my_data->thread_id);
  pthread_exit(NULL);

}

pair<string, unsigned int> ConcurrentHashMap::maximum(unsigned int nt) {
  pthread_t threads[nt];

  int return_code;

  pthread_mutex_t mutex_threads[nt];
  for (int i = 0; i < nt; ++i) {
    pthread_mutex_init(&mutex_threads[i], NULL);
    pthread_mutex_lock(&mutex_threads[i]);
  }

  vector<thread_data_max> args;
  for (int i = 0; i < nt; ++i) {
    thread_data_max arg_i;
    args.push_back(arg_i);
  }
  
  atomic<int> fila(0);
  pair<string, unsigned int> max_inicial("max_inicial", 0);
  vector< pair<string, unsigned int> > maximos(SIZE_TABLE, max_inicial);
  for (unsigned int t = 0; t < nt && t < SIZE_TABLE; t++) {
    args[t] = {t, this, &maximos, &mutex_threads[t], &fila};
    return_code = pthread_create(&threads[t], NULL, ConcurrentHashMap::maximum_thread, (void*) &args[t]);
    if (debug) printf("Thread %d created.\n", t);
  
    if (return_code) {
      printf("ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
  }
  for (int i = 0; i < nt; ++i)
    pthread_mutex_unlock(&mutex_threads[i]);

  void *status;
  for (int t = 0; t < nt; t++) {
    return_code = pthread_join(threads[t], &status);
    if (return_code) {
      printf("ERROR; return code from pthread_join() is %d\n", return_code);
      exit(-1);
    }
  }

  pair<string, unsigned int> max = maximos[0];
  for (int i = 1; i < SIZE_TABLE; ++i) {
    if (max.second < maximos[i].second)
      max = maximos[i];
  }

  return max;
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

void *ConcurrentHashMap::llenarHashMap(void *thread_args) {
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

  int return_code;
  vector<thread_data> args;
  for (int i = 0; i < num_threads; ++i) {
    thread_data arg_i;
    args.push_back(arg_i);
  }
  list<string>::iterator it = archs.begin();
  for (unsigned int t = 0; t < num_threads; t++) {
    args[t] = {t, &map, *it};  
    return_code = pthread_create(&threads[t], NULL, ConcurrentHashMap::llenarHashMap, (void*) &args[t]);
    printf("Thread %d created.\n", t);
  
    if (return_code) {
      printf("ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
    it++;
  }

  void *status;
  for (int t = 0; t < num_threads; t++) {
    return_code = pthread_join(threads[t], &status);
    if (return_code) {
      printf("ERROR; return code from pthread_join() is %d\n", return_code);
      exit(-1);
    }
  }

  return map;
}

// 
/*pthread_create arguments:
thread: An opaque, unique identifier for the new thread returned by the subroutine.
attr: An opaque attribute object that may be used to set thread attributes. You can specify a thread attributes object, or NULL for the default values.
start_routine: the C routine that the thread will execute once it is created.
arg: A single argument that may be passed to start_routine. It must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed.
*/

ConcurrentHashMap ConcurrentHashMap::count_words(unsigned int num_threads, list<string> archs) {

  pthread_t threads[num_threads];
  unsigned int num_archs = archs.size();
  ConcurrentHashMap map;

  int return_code;
  list<string>::iterator it_inicio = archs.begin();
  list<string>::iterator it_fin = archs.end();
  pthread_mutex_t mutex_threads[num_threads];
  pthread_mutex_t mutex_it;
  pthread_mutex_init(&mutex_it, NULL);
  for (int i = 0; i < num_threads; ++i) {
    pthread_mutex_init(&mutex_threads[i], NULL);
    pthread_mutex_lock(&mutex_threads[i]);
  }

  vector<thread_data2> args;
  for (int i = 0; i < num_threads; ++i) {
    thread_data2 arg_i;
    args.push_back(arg_i);
  }

  for (unsigned int t = 0; t < num_threads && t < num_archs; t++) {
    args[t] = {t, &map, &it_inicio, it_fin, &mutex_threads[t], &mutex_it};
    return_code = pthread_create(&threads[t], NULL, ConcurrentHashMap::llenarHashMap2, (void*) &args[t]);
    if (debug) printf("Thread %d created.\n", t);
  
    if (return_code) {
      printf("ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
  }
  for (int i = 0; i < num_threads; ++i)
    pthread_mutex_unlock(&mutex_threads[i]);

  void *status;
  for (int t = 0; t < num_threads; t++) {
    return_code = pthread_join(threads[t], &status);
    if (return_code) {
      printf("ERROR; return code from pthread_join() is %d\n", return_code);
      exit(-1);
    }
  }
  return map;
}

void *ConcurrentHashMap::llenarHashMap2(void *thread_args) {
  
  struct thread_data2* my_data;
  my_data = (struct thread_data2*) thread_args;
  list<string>::iterator* it = my_data->it_inicio;
  list<string>::iterator it_fin = my_data->it_fin; // esta bien por copia? No explota?
  pthread_mutex_lock(my_data->mutex_inicio);
  ifstream inFile;
  string arch;
  // comparamos el iterador que apunta al ultimo archivo leido, contra el iterador que apunta al final de la lista 
  while (true) {
    pthread_mutex_lock(my_data->mutex_it);
    if (*it != it_fin) {
      arch = *(*it);
      (*it)++;
    }
    else
      break;
    pthread_mutex_unlock(my_data->mutex_it);

    inFile.open(arch.c_str()); 
    string word;

    while (inFile >> word)
      (my_data->map)->addAndInc(word);

    inFile.close();
  }
  pthread_mutex_unlock(my_data->mutex_it);
  pthread_mutex_unlock(my_data->mutex_inicio);

  if (debug) printf("[%d] Thread dead.\n", my_data->thread_id);
  pthread_exit(NULL);
}