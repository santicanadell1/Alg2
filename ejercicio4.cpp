#include <cassert>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

struct posicion {
    int id;
    int pos;
    posicion* sig;
};

struct pedido {
    int id;
    int prioridad;
    string items;
    bool paraLlevar;
};

class colaDePrioridad {
private:
    int capacidad;
    int pos; // Primero libre
    pedido** vec;
    int min(int i1, int i2) {
        return vec[i1]->prioridad < vec[i2]->prioridad ? i1 : i2;
    }
    bool lleno() {
        return pos > capacidad;
    }
    bool vacio() {
        return pos == 1; 
    }
    void swap(int pos1, int pos2) {
        pedido* aux = vec[pos1];
        vec[pos1] = vec[pos2];
        vec[pos2] = aux;
        insertarAux(vec[pos1]->id, pos1);
        insertarAux(vec[pos2]->id, pos2);
    }
    int padre(int pos) {
        return pos / 2;
    }
    bool comparar(int pos, int posPadre) {
        bool menor = false;
        if (vec[pos]->prioridad < vec[posPadre]->prioridad) {
            menor = true;
        }
        else if (vec[pos]->prioridad == vec[posPadre]->prioridad && vec[pos]->paraLlevar && !vec[posPadre]->paraLlevar) {
            menor = true;
        }
        else if (vec[pos]->prioridad == vec[posPadre]->prioridad && vec[pos]->paraLlevar == vec[posPadre]->paraLlevar && vec[pos]->id < vec[posPadre]->id) {
            menor = true;
        }
        return menor;
    }
    void flotar(int pos) {
        if (pos == 1) return;
        int posPadre = padre(pos);
        if (comparar(pos, posPadre)) {
            swap(pos, posPadre);
            flotar(posPadre);
        }
    }
    void hundir(int pos) {
        int izq = pos * 2;
        int der = pos * 2 + 1;
        int menor = pos;
        if (izq < capacidad && vec[izq] && comparar(izq, menor)) {
            menor = izq;
        }
        if (der < capacidad && vec[der] && comparar(der, menor)) {
            menor = der;
        }
        if (menor != pos) {
            swap(pos, menor);
            hundir(menor);
        }
    }
    posicion** hash;
    int largo;
    int cantElem;
    int funcionHash1(int c) {
        return c % largo;
    }
    void rehash() {
        int tamNuevo = largo * 2;
        posicion** nuevaTabla = new posicion*[tamNuevo]();
        posicion* aux = NULL;
        int largoAnt = largo;
        largo = tamNuevo;
        for (int i = 0; i < largoAnt; i++) {
            aux = hash[i];
            while (aux) {
                int buck = funcionHash1(aux->id);
                posicion* nuevo = new posicion;
                nuevo->id = aux->id;
                nuevo->pos = aux->pos;
                nuevo->sig = nuevaTabla[buck];
                nuevaTabla[buck] = nuevo;
                aux = aux->sig;
            }
        }
        for (int i = 0; i < largoAnt; i++) {
            posicion* actual = hash[i];
            while (actual) {
                posicion* aux = actual->sig;
                delete actual;
                actual = aux;
            }
        }
        delete[] hash;
        hash = nuevaTabla;
        largo = tamNuevo;
    }
    
    void insertarAux(int id, int pos) {
        if (cantElem >= 0.7 * largo) {
            rehash();
        }
        int indice = funcionHash1(id);
        posicion* buck = hash[indice];
        while (buck) {
            if (buck->id == id) {
                buck->pos = pos;
                return;
            }
            buck = buck->sig;
        }
        if (!buck) {
            buck = new posicion;
            buck->id = id;
            buck->pos = pos;
            buck->sig = hash[indice];
            hash[indice] = buck;
        }
    }

    
    int buscarAux(int id) {
        int indice = funcionHash1(id);
        posicion* buck = hash[indice];
        while (buck) {
            if (buck->id == id) return buck->pos;
            buck = buck->sig;
        }
        return -1;
    }
public:
    colaDePrioridad(int n) {
        hash = new posicion*[n]();
        largo = n;
        cantElem = 0;
        capacidad = n;
        pos = 1; // Primero libre
        vec = new pedido*[capacidad + 1]();
    }
    ~colaDePrioridad() {
        for (int i = 1; i < pos; i++) {
            delete vec[i]; // Liberar memoria de los pedidos
        }
        delete[] vec;
        for (int i = 0; i < largo; i++) {
            posicion* actual = hash[i];
            while (actual) {
                posicion* aux = actual->sig;
                delete actual;
                actual = aux;
            }
        }
        delete[] hash;
    }
    
    void encolar(int id, int prioridad, string items, bool llevar) {
        if (!lleno()) {
            pedido* nuevo = new pedido;
            nuevo->id = id;
            nuevo->prioridad = prioridad;
            nuevo->paraLlevar = llevar;
            nuevo->items = items;
            vec[pos] = nuevo;
            flotar(pos);
            pos++;
            cantElem++;
        }
    }
    pedido* desencolar() {
        if (vacio()) return NULL; // Manejar el caso vacío
        pedido* desencolado = vec[1];
        swap(1, pos - 1);
        vec[pos - 1] = NULL;
        pos--;
        hundir(1);
        return desencolado;
    }
    void eliminar(int id) {
        int posicion = buscarAux(id);
        pedido* desencolado = vec[posicion];
        swap(posicion, pos - 1);
        vec[pos - 1] = NULL;
        pos--;
        cantElem--;
        flotar(posicion);
        hundir(posicion);
        delete desencolado;
    }
    void cambiarPedido(int id) {
        int posicion = buscarAux(id);
        if (posicion == -1) return; // Si no se encuentra el id
        vec[posicion]->paraLlevar = !vec[posicion]->paraLlevar;
        flotar(posicion);
        hundir(posicion);
    }
    int cantElemEncolados() {
        return pos - 1; // Retornar elementos en cola
    }
};

int main() {
    int N;
    cin >> N;
    colaDePrioridad cola(N);
    int O;
    cin >> O;
    for (int i = 0; i < O; i++) {
        string operacion;
        cin >> operacion;
        if (operacion == "I") {
            int id;
            string items;
            bool paraLlevar;
            int prioridad;
            cin >> id >> prioridad >> paraLlevar >> items;
            cola.encolar(id, prioridad, items, paraLlevar);
        } else if (operacion == "E") {
            int id;
            cin >> id;
            cola.eliminar(id);
        } else if (operacion == "C") {
            int id;
            cin >> id;
            cola.cambiarPedido(id);
        }
    }
    
    int cant = cola.cantElemEncolados();
    for (int i = 1; i <= cant; i++) { // Cambiar i < cant a i < cant
        pedido* ped = cola.desencolar();
        if (ped) {
            cout << ped->id << " " << ped->prioridad << " " << (ped->paraLlevar ? "true" : "false") << " " << ped->items << endl;
            delete ped; 
        }
    }
    

    return 0;
}
