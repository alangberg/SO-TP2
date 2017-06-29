#include <iostream>
#include <cstdlib>
#include "ConcurrentHashMap.hpp"
#include <string> 
#include <time.h>
using namespace std;

int main(int argc, char const *argv[])
{
	// Calculate time taken by a request
	struct timespec requestStart, requestEnd;
	double tiempo5;
	double tiempo6;
	double acum5;
	double acum6;

	pair<string, unsigned int> p;


	int p_arch_fijo = 5;
	int p_max_fijo = 5;

	int p_max_init = 1;
	int p_arch_init =1;

	int p_max_tope = 20;
	int p_arch_tope = 20;


	list<string> l = { "test01_0.txt", "test01_1.txt", "test01_2.txt", "test01_3.txt", "test01_4.txt" };



	int i;
	int iter_p_max = p_max_init;
	cout<<"ejercicio;p_archivos;p_max;tiempo"<<endl;
	while(iter_p_max <= p_max_tope){
		i = 0;
		acum5 = 0;
		acum6 = 0;
		while(i < 30){
			clock_gettime(CLOCK_REALTIME, &requestStart);
			p = ConcurrentHashMap::maximum(p_arch_fijo, iter_p_max, l);
			clock_gettime(CLOCK_REALTIME, &requestEnd);
			tiempo5 = requestEnd.tv_nsec - requestStart.tv_nsec;
			acum5 = acum5 + tiempo5;

			clock_gettime(CLOCK_REALTIME, &requestStart);
			p = ConcurrentHashMap::maximum_cw(p_arch_fijo,iter_p_max,l);
			clock_gettime(CLOCK_REALTIME, &requestEnd);
			tiempo6 = requestEnd.tv_nsec - requestStart.tv_nsec;
			acum6 = acum6 + tiempo6;



			i++;
		}
		double promedio5 = acum5/30000000.00;
		double promedio6 = acum6/30000000.00;
		cout<<"maximum"<<";"<<p_arch_fijo<<";"<<iter_p_max<<";"<<promedio5<<endl;
		cout<<"maximum_cw"<<";"<<p_arch_fijo<<";"<<iter_p_max<<";"<<promedio6<<endl;
		iter_p_max++;
	}
		
	int iter_p_arch = p_arch_init;
	while(iter_p_arch <= p_arch_tope){
		i = 0;
		acum5 = 0;
		acum6 = 0;
		while(i < 30){
			clock_gettime(CLOCK_REALTIME, &requestStart);
			p = ConcurrentHashMap::maximum(iter_p_arch, p_max_fijo, l);
			clock_gettime(CLOCK_REALTIME, &requestEnd);
			tiempo5 = requestEnd.tv_nsec - requestStart.tv_nsec;
			acum5 = acum5 + tiempo5;

			clock_gettime(CLOCK_REALTIME, &requestStart);
			p = ConcurrentHashMap::maximum_cw(iter_p_arch,p_max_fijo,l);
			clock_gettime(CLOCK_REALTIME, &requestEnd);
			tiempo6 = requestEnd.tv_nsec - requestStart.tv_nsec;
			acum6 = acum6 + tiempo6;



			i++;
		}
		double promedio5 = acum5/30000000.00;
		double promedio6 = acum6/30000000.00;
		cout<<"maximum"<<";"<<iter_p_arch<<";"<<p_max_fijo<<";"<<promedio5<<endl;
		cout<<"maximum_cw"<<";"<<iter_p_arch<<";"<<p_max_fijo<<";"<<promedio6<<endl;
		iter_p_arch++;
	}

	//Fin tiempo ejer5
		
		
	





	// // cout <<"tiempo ejer5: "<< tiempo << endl;


	// //Comienzo a medir tiempo del maximum del ejer6
	// clock_gettime(CLOCK_REALTIME, &requestStart);

	// p = ConcurrentHashMap::maximum_cw(50,50,l);

	// //Fin tiempo ejer6
	// clock_gettime(CLOCK_REALTIME, &requestEnd);

	// tiempo = requestEnd.tv_nsec - requestStart.tv_nsec;

	// cout <<"tiempo ejer6: "<< tiempo << endl;

	return 0;
}