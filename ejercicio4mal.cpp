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
        return pos >= capacidad;
    }
    
    bool vacio() {
        return pos == 1; 
    }

    void swap(int pos1, int pos2) {
        if (pos1 < 1 || pos1 >= pos || pos2 < 1 || pos2 >= pos) {
            std::cerr << "Swap indices out of bounds: " << pos1 << ", " << pos2 << std::endl;
            return;
        }
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
        } else if (vec[pos]->prioridad == vec[posPadre]->prioridad && vec[pos]->paraLlevar && !vec[posPadre]->paraLlevar) {
            menor = true;
        } else if (vec[pos]->prioridad == vec[posPadre]->prioridad && vec[pos]->paraLlevar == vec[posPadre]->paraLlevar && vec[pos]->id < vec[posPadre]->id) {
            menor = true;
        }
        return menor;
    }

    void flotar(int pos) {
        if (pos == 1) {
            return;
        }
        int posPadre = padre(pos);
        if (comparar(pos, posPadre)) {
            swap(pos, posPadre);
            flotar(posPadre);
        }
    }

    void hundir(int pos1) {
        int izq = pos1 * 2;
        int der = pos1 * 2 + 1;
        int menor = pos1;
        if (izq < pos && vec[izq] && comparar(izq, menor)) {
            menor = izq;
        }
        if (der < pos && vec[der] && comparar(der, menor)) {
            menor = der;
        }
        if (menor != pos1) {
            swap(pos1, menor);
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
        posicion* nuevo = new posicion;
        nuevo->id = id;
        nuevo->pos = pos;
        nuevo->sig = hash[indice];
        hash[indice] = nuevo;
        cantElem++;  // Incrementa cuando se inserta un nuevo elemento
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
        if (vacio()) return NULL;
        pedido* desencolado = vec[1];
        swap(1, pos - 1);
        vec[pos - 1] = NULL;
        pos--;
        hundir(1);
        return desencolado;
    }

    void eliminar(int id) {
        int posicion = buscarAux(id);
        if (posicion == -1) return;
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
        if (posicion == -1) return;
        vec[posicion]->paraLlevar = !vec[posicion]->paraLlevar;
        flotar(posicion);
        hundir(posicion);
    }

    int cantElemEncolados() {
        return pos - 1;
    }
};

int main() {
    int N;
    cin >> N;  // Número de pedidos
    
    int O;
    cin >> O;  // Número de operaciones a realizar
    colaDePrioridad cola(N);  // Inicializa la cola de prioridad con N

    for (int i = 0; i < O; i++) {
        string operacion;
        cin >> operacion;

        if (operacion == "I") {
            int id;
            int prioridad;
            string paraLlevarStr;
            string items;
            
            cin >> id >> prioridad >> paraLlevarStr;
            cin.ignore();  // Ignorar el espacio antes de items
            getline(cin, items);  // Leer items como una línea completa
            
            bool paraLlevar = (paraLlevarStr == "true");

            cola.encolar(id, prioridad, items, paraLlevar);

        } else if (operacion == "E") {
            int id;
            cin >> id;
            cola.eliminar(id);  // Eliminar pedido por ID

        } else if (operacion == "C") {
            int id;
            cin >> id;
            cola.cambiarPedido(id);  // Cambiar estado para llevar del pedido
        }
    }

    cout << cola.cantElemEncolados() << endl;  // Imprime la cantidad de pedidos encolados
    return 0;
}