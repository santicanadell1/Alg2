#include <cassert>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

struct objeto {
    int precio;
    int id;
};

class minHeap {
private:
    int capacidad;
    int pos; // primero libre
    objeto** vec;

    int min(int i1, int i2) {
        return vec[i1]->precio < vec[i2]->precio ? i1 : i2;
    }

    bool lleno() {
        return pos > capacidad;
    }

    bool vacio() {
        return pos <= 1; // Si pos es 1 o menor, el heap está vacío
    }

    void swap(int pos1, int pos2) {
        objeto* aux = vec[pos1];
        vec[pos1] = vec[pos2];
        vec[pos2] = aux;
    }

    int padre(int pos) {
        return pos / 2;
    }

    bool comparar(int pos, int posPadre) {
        if (vec[pos]->precio < vec[posPadre]->precio) {
            return true;
        } else if (vec[pos]->precio == vec[posPadre]->precio && vec[pos]->id > vec[posPadre]->id) {
            return true;
        }
        return false;
    }

    void flotar(int pos) {
        if (pos == 1) return; // Si es la raíz, no hay que flotar
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




public:
    minHeap(int tam) {
        vec = new objeto*[tam + 1](); // Inicialización a nullptr
        capacidad = tam;
        pos = 1; // Comenzar desde 1
    }

    ~minHeap() {
        for (int i = 1; i < pos; i++) { // Liberar solo hasta pos
            delete vec[i];
        }
        delete[] vec;
    }

    void encolar(int id, int precio) {
        if (!lleno()) {
            objeto* nuevo = new objeto;
            nuevo->id = id;
            nuevo->precio = precio;
            vec[pos] = nuevo;
            flotar(pos);
            pos++;
        }
    }

    objeto* desencolar() {
        if (vacio()) return nullptr; // Comprobar si el heap está vacío
        objeto* desencolado = vec[1];
        vec[1] = vec[pos - 1]; // Mover el último elemento a la raíz
        vec[pos - 1] = nullptr; // Liberar el último
        pos--;
        hundir(1);
        return desencolado;
    }
};

int main() {
    int N;
    cin >> N;
    int* ids = new int[N + 1];
    fill_n(ids, N + 1, numeric_limits<int>::max()); // Inicializar con valores altos
    bool* cambiado = new bool[N+1]();
    int pos;
    int precio;
    for (int i = 1; i <= N; i++) {
        cin >> pos >> precio;
        if (precio < ids[pos]) {
            ids[pos] = precio; // Solo asignar si el nuevo precio es menor
            cambiado[pos] = true;
        }
    }

    minHeap heap = minHeap(N+1);
    for (int i = 1; i <= N; i++) {
        if(cambiado[i]){
        heap.encolar(i, ids[i]);
        }
    }
    

    int K;
    cin >> K;
    for (int i = 1; i <= K; i++) {
        objeto* desencolado = heap.desencolar();
        if (desencolado) { // Comprobar si desencolado no es nulo   
                std::cout << desencolado->id << endl;
            delete desencolado;
        }
    }

    delete[] ids; // Liberar el array de ids
    return 0;
}