#ifndef LISTA_ATOMICA_H__
#define LISTA_ATOMICA_H__

#include <atomic>
#include <iostream>
using namespace std;

template <typename T>
class Lista {
private:
	struct Nodo {
		Nodo(const T& val) : _val(val), _next(nullptr) {}
		T _val;
		Nodo *_next;
	};

	std::atomic<Nodo *> _head;

public:
	Lista() : _head(nullptr) {}
	~Lista() {
		Nodo *n, *t;
		n = _head.load();
		while (n) {
			t = n;
			n = n->_next;
			delete t;
		}
	}

	void push_front(const T& val) {
		/* Completar. Debe ser atómico. */
		Nodo* new_node = new Nodo(val);
    new_node->_next = _head.load(memory_order_relaxed); //siguiente del nodo nuevo = cabeza actual

    // ponemos new_node como nueva cabeza, pero si la cabeza dejo de ser lo que guardamos en new_node->next,
    // (porque otro thread inserto un nodo justo ahora) reemplazamos su valor con la nueva cabeza e intentamos de nuevo 
    while(!_head.compare_exchange_weak(new_node->_next, new_node, memory_order_release, memory_order_relaxed)); // el ciclo es vacio
	}

	T& front() const {
		return _head.load()->_val;
	}

	T& iesimo(int i) const {
		Nodo *n = _head.load();
		int j;
		for (int j = 0; j < i; j++)
			n = n->_next;
		return n->_val;
	}

	class Iterador {
	public:
		Iterador() : _lista(nullptr), _nodo_sig(nullptr) {}

		Iterador& operator = (const typename Lista::Iterador& otro) {
			_lista = otro._lista;
			_nodo_sig = otro._nodo_sig;
			return *this;
		}

		bool HaySiguiente() const {
			return _nodo_sig != nullptr;
		}

		T& Siguiente() {
			return _nodo_sig->_val;
		}

		void Avanzar() {
			_nodo_sig = _nodo_sig->_next;
		}

		bool operator == (const typename Lista::Iterador& otro) const {
			return _lista._head.load() == otro._lista._head.load() && _nodo_sig == otro._nodo_sig;
		}

	private:
		Lista *_lista;

		typename Lista::Nodo *_nodo_sig;

		Iterador(Lista<T>* lista,typename Lista<T>::Nodo* sig) : _lista(lista), _nodo_sig(sig) {}
		friend typename Lista<T>::Iterador Lista<T>::CrearIt();
	};

	Iterador CrearIt() {
		return Iterador(this, _head);
	}
};

#endif /* LISTA_ATOMICA_H__ */
