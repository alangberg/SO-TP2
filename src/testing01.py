#!/usr/bin/python

#python random_ejer2.py tam_entradas cant_entradas > out.txt
#Crea cant_entradas de tam tam_entradas y las guarda en out.txt

import sys
import math
import random
import numpy as np

cant_archivos = int(sys.argv[1])
pals_per_file = int(sys.argv[2])

corpus = open("corpus").readlines()
# print len(corpus)
corpus_sin_reps = list(set(corpus))
target = open("corpus_copia", 'w')
# print len(corpus_sin_reps)
ganador_global = "funciona\n"

# for item in corpus_sin_reps:
#   target.write("%s" % item)

perct_global = 35/100.0
perct_local = 50/100.0
perct_otros = 15/100.0
# print perct_local
# print perct_global


i = 0
while i < cant_archivos:
	j = 0
	file = open("test01_"+str(i)+".txt","w")
	ganador_local = corpus_sin_reps[i]
	existe = np.random.choice(3, pals_per_file, p=[perct_global,perct_local,perct_otros])
	while j < pals_per_file:
	#creo el archivo i
		#randomizo en que lineas meto a la palabra ganadora global
		#randomizo en que lineas meto a la palabra ganadora local
		a_imprimir = ganador_global
		if existe[j] == 1:
			a_imprimir = ganador_local
		if existe[j] == 2:
			a_imprimir = random.choice(corpus_sin_reps)
		file.write("%s" %a_imprimir)
		j+=1
	i+=1

# for c in range(0, cant_entradas):
# 	ejes = int(random.uniform(0, nodos * (nodos - 1) / 2) + 1)	#Puede desde no tener aristas a ser completo

# 	Matrix = [[0 for x in range(nodos)] for y in range(nodos)] 


# 	print str(nodos) + " " + str(ejes)
# 	flag_forzar_Max = 0
# 	i = 0
# 	while(i < ejes):
# 		c1 = int(random.uniform(1, nodos + 1))
# 		c2 = int(random.uniform(1, nodos + 1))
# 		while c2 == c1:
# 			c2 = int(random.uniform(1, nodos + 1))
		
# 		peso = int(random.uniform(0, 51))	#Peso entre 0 y 50 incluido
# 		if flag_forzar_Max == 0:	
# 			flag_forzar_Max = 1
# 			peso = 75
# 		if Matrix[c1 - 1][c2 - 1] == 0:
# 			Matrix[c1 - 1][c2 - 1] = 1
# 			# Matrix[c2 - 1][c1 - 1] = 1
# 			print str(c1) + " " + str(c2) + " " + str(peso)
# 			i += 1
# print "-1 -1"