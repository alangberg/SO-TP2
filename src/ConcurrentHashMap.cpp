#include "ConcurrentHashMap.hpp"

#define debug 0

ConcurrentHashMap::ConcurrentHashMap() {
  // inicializamos listas vacias y mutex que usa la clase
  for (int i = 0; i < SIZE_TABLE; i++) {
    Lista< pair<string, unsigned int> > * list = new Lista< pair<string, unsigned int> >();
    tabla.push_back(list);
    pthread_mutex_init(&mutex[i], NULL);
  }
  pthread_mutex_init(&mutex_maximum, NULL);
}
  
ConcurrentHashMap::~ConcurrentHashMap() {
  // for (int i = 0; i < SIZE_TABLE; i++) {
    // cout<<"borrando "<<i<<endl;
    // delete tabla[i];
    // cout<<"chau "<<i<<endl;
  // }
}


void ConcurrentHashMap::addAndInc(string key) {
  // aplicamos la funcion de hash sobre el primer caracter y obtenemos la posicion en la tabla
  unsigned int posicion = fHash(key[0]);
  bool pertenece = false;

  // lock para que no se pueda utilizar maximum a la vez 
  pthread_mutex_lock(&mutex_maximum);
  // lock para que nadie mas pueda acceder a la lista donde trabajamos 
  pthread_mutex_lock(&mutex[posicion]);
  // creamos iterador para la lista 
  auto it = tabla[posicion]->CrearIt();

  // mientras quedan elementos por iterar, y el elemento no fue encontrado
  while (it.HaySiguiente() && !pertenece) {
    if (it.Siguiente().first == key) {
      // si lo encontramos incrementamos el número
      it.Siguiente().second++;
      pertenece = true;
    }
    it.Avanzar();
  }
  
  // si no lo encontramos lo agregamos con valor 1
  if (!pertenece) {
    pair<string, unsigned int> dicc_entry (key,1);
    tabla[posicion]->push_front(dicc_entry);
  }

  // unlock para liberar la lista donde trabajamos 
  pthread_mutex_unlock(&mutex[posicion]);
  // unlock para que se pueda utilizar maximum 
  pthread_mutex_unlock(&mutex_maximum);
}

bool ConcurrentHashMap::member(string key){
  // aplicamos la funcion de hash sobre el primer caracter y obtenemos la posicion en la tabla
  unsigned int posicion = fHash(key[0]);
  // creamos iterador para la lista
  auto it = tabla[posicion]->CrearIt();

  // buscamos string 
  while(it.HaySiguiente()) {
    if(it.Siguiente().first == key)
      return true;
    it.Avanzar();
  }
  return false;
}

void *ConcurrentHashMap::maximum_thread(void *thread_args) {
  
  struct thread_data_max* my_data;
  my_data = (struct thread_data_max*) thread_args;
  unsigned int fila_actual;
  
  // mientras queden filas para las cuales buscar maximo, tomamos una e incrementamos la fila (atomicamente)
  while ((fila_actual = atomic_fetch_add(my_data->fila, 1)) < SIZE_TABLE) {
    if (debug) printf("[%d] Buscando maximo en lista %d\n", my_data->thread_id, fila_actual);
    
    // creamos iterador de lista para recorrerla
    Lista< pair<string, unsigned int> >::Iterador it = my_data->map->tabla[fila_actual]->CrearIt();
    // buscamos maximo en la fila
    while (it.HaySiguiente()) {
      pair<string, unsigned int> t = it.Siguiente();
      if (t.second > (*my_data->maximos)[fila_actual].second)
        (*my_data->maximos)[fila_actual] = t;
      it.Avanzar();
    }
  }

  if (debug) printf("[%d] Thread finalizado.\n", my_data->thread_id);
  pthread_exit(NULL);
}

pair<string, unsigned int> ConcurrentHashMap::maximum(unsigned int nt) {
  // si la cantidad de threads es mayor que la cantidad de listas, la cambiamos
  if (nt > SIZE_TABLE) 
    nt = SIZE_TABLE;

  pthread_t threads[nt];    // aca se guardan los threads
  thread_data_max args[nt]; // aca se guardan las estructuras thread_data_max que le damos a cada thread
  atomic<int> fila(0);      // lleva la cuenta de las filas ya tomadas por algun thread

  pair<string, unsigned int> max_inicial("max_inicial", 0);
  // aca se guardan los maximos por fila
  vector< pair<string, unsigned int> > maximos(SIZE_TABLE, max_inicial);  
  int return_code;

  // lock para que no se pueda utilizar addAndInc a la vez 
  pthread_mutex_lock(&mutex_maximum); 

  // en cada iteracion creamos un nuevo thread, al que le damos una estructura thread_data_max 
  for (unsigned int t = 0; t < nt; t++) {
    args[t] = {t, this, &maximos, &fila}; 
    return_code = pthread_create(&threads[t], NULL, ConcurrentHashMap::maximum_thread, (void*) &args[t]);
    if (debug) printf("Thread %d creado.\n", t);
  
    if (return_code) {
      printf("[ERROR] pthread_create() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  // esperamos a que todos los threads finalicen
  void *status;
  for (int t = 0; t < nt; t++) {
    return_code = pthread_join(threads[t], &status);
    if (return_code) {
      printf("[ERROR] pthread_join() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  // unlock para que se pueda utilizar addAndInc 
  pthread_mutex_unlock(&mutex_maximum);

  // busqueda lineal del maximo entre los maximos de cada fila
  pair<string, unsigned int> max = maximos[0];
  for (int i = 1; i < SIZE_TABLE; ++i) {
    if (max.second < maximos[i].second)
      max = maximos[i];
  }

  return max;
}

void* ConcurrentHashMap::mergeHashMap(void *thread_args) {
  
  struct thread_data_merge* my_data;
  my_data = (struct thread_data_merge*) thread_args;
  unsigned int fila_actual;
  
  // mientras queden filas por mergear tomamos una e incrementamos la fila (atomicamente)
  while ((fila_actual = atomic_fetch_add(my_data->fila, 1)) < SIZE_TABLE) {
    if (debug) printf("[%d][%d] Mergeando lista %d\n", my_data->map_id, my_data->thread_id, fila_actual);
    
    // creamos iterador de lista para recorrerla
    Lista< pair<string, unsigned int> >::Iterador it = my_data->map->tabla[fila_actual]->CrearIt();
    
    // en cada iteracion agregamos el elemento al ConcurrentHashMap en que estamos mergeando
    while (it.HaySiguiente()) {
      pair<string, unsigned int> t = it.Siguiente();
      // hacemos addAndInc t.second veces
      for (int i = 0; i < t.second; i++) {
        my_data->merge_map->addAndInc(t.first);
      }
      it.Avanzar(); // avanzamos iterador
    }
  }

  if (debug) printf("[%d][%d] Thread finalizado.\n", my_data->map_id, my_data->thread_id);
  pthread_exit(NULL);
}

pair<string, unsigned int> ConcurrentHashMap::maximum(unsigned int p_archivos, unsigned int p_maximos, list<string>archs) {
  
  unsigned int archs_size = archs.size();  
  // si la cantidad de threads para parsear es mayor que la cantidad de archivos, la cambiamos
  if (p_archivos > archs_size)
    p_archivos = archs_size;

  // si la cantidad de threads para calcular el maximo es mayor que la cantidad de listas, la cambiamos
  if (p_maximos > SIZE_TABLE) 
    p_maximos = SIZE_TABLE;

  pthread_t threads[p_archivos][p_maximos];   // aca se guardan los threads
  ConcurrentHashMap maps[p_archivos];         // aca se guardan los ConcurrentHashMaps que vamos a llenar
  thread_data2 args_read[p_archivos];         // aca se guardan las estructuras thread_data2 que le damos 
                                              // a cada thread para parsear
  list<string>::iterator it_inicio = archs.begin();   // iterador que utilizaran los threads
  list<string>::iterator it_fin = archs.end();        // iterador al final de archs
  pthread_mutex_t mutex_it;                           // mutex para el uso del iterador
  pthread_mutex_init(&mutex_it, NULL);
  int return_code;

  // en cada iteracion creamos un nuevo thread para leer archivos, al que le damos una estructura thread_data2 
  for (unsigned int t = 0; t < p_archivos; t++) {
    args_read[t] = {t, &maps[t], &it_inicio, it_fin, &mutex_it};
    return_code = pthread_create(&threads[t][0], NULL, ConcurrentHashMap::llenarHashMap2, (void*) &args_read[t]);
    if (debug) printf("Thread %d creado.\n", t);
  
    if (return_code) {
      printf("[ERROR] pthread_create() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  // esperamos a que todos los threads usados para leer finalicen
  void *status;
  for (int t = 0; t < p_archivos; t++) {
    return_code = pthread_join(threads[t][0], &status);
    if (return_code) {
      printf("[ERROR] pthread_join() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  if (debug) cout << "Parseo finalizado\n";

  // aca se guardan las estructuras thread_data_merge que le damos a cada thread para encontrar el maximo
  thread_data_merge args_max[p_archivos][p_maximos];

  // lleva la cuenta de las filas ya tomadas por algun thread, una para cada ConcurrentHashMap
  atomic<int> filas[p_archivos];       
  for (int m = 0; m < p_archivos; m++)
    filas[m].store(0);
  
  ConcurrentHashMap merge_map;        // en este ConcurrentHashMap vamos a mergear todos los anteriores

  // este ciclo itera sobre la cantidad de threads por ConcurrentHashMap
  for (unsigned int t = 0; t < p_maximos; ++t) {
    // este ciclo itera sobre los ConcurrentHashMap y crea un thread para cada uno de ellos
    for (unsigned int m = 0; m < p_archivos; ++m) {
      args_max[m][t] = {m, t, &maps[m], &merge_map, &filas[m]};  
      return_code = pthread_create(&threads[m][t], NULL, ConcurrentHashMap::mergeHashMap, (void*) &args_max[m][t]);
      if (debug) printf("ConcurrentHashMap %d: Thread %d creado.\n", m, t);
  
      if (return_code) {
        printf("[ERROR] ConcurrentHashMap %d: pthread_create() falló para el thread %d (ret_code = %d)\n", m, t, return_code);
        exit(-1);
      }
    }
  }

  // esperamos a que todos los threads usados para calcular el maximo finalicen
  for (unsigned int t = 0; t < p_maximos; ++t) {
    for (unsigned int m = 0; m < p_archivos; ++m) {
      return_code = pthread_join(threads[m][t], &status);
      if (return_code) {
        printf("[ERROR] ConcurrentHashMap %d: pthread_join() falló para el thread %d (ret_code = %d)\n", m, t, return_code);
        exit(-1);
      }
    }
  }

  if (debug) cout << "Merge finalizado\n";

  // calculo del maximo sobre el ConcurrentHashMap en el que mergeamos todos los anteriores
  pair<string, unsigned int> max = merge_map.maximum(p_maximos);
  return max;
}

pair<string, unsigned int> ConcurrentHashMap::maximum_cw(unsigned int p_archivos, unsigned int p_maximos, list<string>archs) {
  // leemos archivos usando count_words
  ConcurrentHashMap map = count_words(p_archivos, archs);
  // calculo del maximo sobre el ConcurrentHashMap en el que mergeamos todos los anteriores
  pair<string, unsigned int> max = map.maximum(p_maximos);
  return max;
}


/*EJERCICIO 2.2
tome un archivo de texto y devuelva el ConcurrentHashMap cargado con las palabras del archivo. (NO CONCURRENTE)*/

ConcurrentHashMap ConcurrentHashMap::count_words(string arch){
  
  ConcurrentHashMap map;
  
  // abrimos archivo
  ifstream inFile;
  inFile.open(arch.c_str()); 
  string word;

  // leemos archivo y agregamos palabras al ConcurrentHashMap
  while (inFile >> word) {
    map.addAndInc(word);
  }

  //cerramos archivo
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

  // abrimos archivo
  ifstream inFile;
  inFile.open(arch.c_str()); 
  string word;

  // leemos archivo y agregamos palabras al ConcurrentHashMap
  while (inFile >> word)
    (my_data->map)->addAndInc(word);

  //cerramos archivo
  inFile.close();

  if (debug) printf("[%d] Thread finalizado.\n", my_data->thread_id);
  pthread_exit(NULL);
}

ConcurrentHashMap ConcurrentHashMap::count_words(list<string> archs) {

  unsigned int num_threads = archs.size();
  pthread_t threads[num_threads];           // aca se guardan los threads
  ConcurrentHashMap map;                    // ConcurrentHashMap que vamos a llenar
  thread_data args[num_threads]; // aca se guardan las estructuras thread_data que le damos a cada thread
  list<string>::iterator it = archs.begin(); // iterador para recorrer archs
  int return_code;

  // en cada iteracion creamos un nuevo thread, al que le damos una estructura thread_data 
  for (unsigned int t = 0; t < num_threads; t++) {
    args[t] = {t, &map, *it};  
    return_code = pthread_create(&threads[t], NULL, ConcurrentHashMap::llenarHashMap, (void*) &args[t]);
    if (debug) printf("Thread %d creado.\n", t);
  
    if (return_code) {
      printf("[ERROR] pthread_create() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
    // avanzamos iterador
    it++;
  }

  // esperamos a que todos los threads finalicen
  void *status;
  for (int t = 0; t < num_threads; t++) {
    return_code = pthread_join(threads[t], &status);
    if (return_code) {
      printf("[ERROR] pthread_join() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  return map;
}

ConcurrentHashMap ConcurrentHashMap::count_words(unsigned int num_threads, list<string> archs) {
  // si la cantidad de threads es mayor que la de archivos la cambiamos
  if (num_threads > archs.size()) 
    num_threads = archs.size();

  pthread_t threads[num_threads];           // aca se guardan los threads
  ConcurrentHashMap map;                    // ConcurrentHashMap que vamos a llenar
  list<string>::iterator it_inicio = archs.begin();  // iterador que utilizaran los threads 
  list<string>::iterator it_fin = archs.end();       // iterador al final de archs
  pthread_mutex_t mutex_it;                          // mutex para el uso del iterador
  pthread_mutex_init(&mutex_it, NULL);
  thread_data2 args[num_threads]; // aca se guardan las estructuras thread_data2 que le damos a cada thread
  int return_code;

  // en cada iteracion creamos un nuevo thread, al que le damos una estructura thread_data2 
  for (unsigned int t = 0; t < num_threads; t++) {
    args[t] = {t, &map, &it_inicio, it_fin, &mutex_it};
    return_code = pthread_create(&threads[t], NULL, ConcurrentHashMap::llenarHashMap2, (void*) &args[t]);
    if (debug) printf("Thread %d creado.\n", t);
  
    if (return_code) {
      printf("[ERROR] pthread_create() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  // esperamos a que todos los threads finalicen
  void *status;
  for (int t = 0; t < num_threads; t++) {
    return_code = pthread_join(threads[t], &status);
    if (return_code) {
      printf("[ERROR] pthread_join() falló para el thread %d (ret_code = %d)\n", t, return_code);
      exit(-1);
    }
  }

  return map;
}

void *ConcurrentHashMap::llenarHashMap2(void *thread_args) {

  struct thread_data2* my_data;
  my_data = (struct thread_data2*) thread_args;
  list<string>::iterator* it = my_data->it_inicio;
  list<string>::iterator it_fin = my_data->it_fin;
  ifstream inFile;
  string arch;
  
  while (true) {
    // lock para usar iterador
    pthread_mutex_lock(my_data->mutex_it);
    // comparamos el iterador que apunta al ultimo archivo leido, contra el iterador que apunta al 
    // final de la lista para ver si ya no quedan archivos por parsear  
    if (*it != it_fin) {
      arch = *(*it);
      (*it)++;
    }
    else
      break;
    // unlock para liberar iterador
    pthread_mutex_unlock(my_data->mutex_it);

    // abrimos archivo
    inFile.open(arch.c_str()); 
    string word;

    // leemos archivo y agregamos palabras al ConcurrentHashMap
    while (inFile >> word)
      (my_data->map)->addAndInc(word);

    //cerramos archivo
    inFile.close();
  }
  // unlock para liberar iterador (cuando no quedan mas archivos)
  pthread_mutex_unlock(my_data->mutex_it);

  if (debug) printf("[%d] Thread finalizado.\n", my_data->thread_id);
  pthread_exit(NULL);
}