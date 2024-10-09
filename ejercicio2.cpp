#include <cassert>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

class Biblioteca {
private:
    struct libro {
        int id;
        string titulo;
        bool habilitado;
    };

    // funciones auxiliares
    int funcionHash1(int c) {
        return c % largo;
    }

    int funcionHash2(int c) {
        return (c * 2 % largo) + 1;
    }

    int dobleHash(int c, int i) {
        return (funcionHash1(c) + i * funcionHash2(c)) % largo;
    }
    void rehash() {
        int nuevoLargo = largo * 2;
        bool* nuevoOcupado = new bool[nuevoLargo]();
        libro** nuevoHash = new libro*[nuevoLargo]();
        int largoAnt = largo;
        largo = nuevoLargo;
        for (int i = 0; i < largoAnt; ++i) {
            if (ocupado[i] && hash[i]) { // Verificar si hash[i] no es nullptr
                int clave = hash[i]->id;
                int j = 0;
                int bucket = dobleHash(clave, j);
                while (nuevoOcupado[bucket]) {
                    j++;
                    bucket = dobleHash(clave, j);
                }
                nuevoOcupado[bucket] = true;
                libro* nuevo = new libro;
                nuevo->id = hash[i]->id;
                nuevo->titulo = hash[i]->titulo;
                nuevo->habilitado = hash[i]->habilitado;
                nuevoHash[bucket] = nuevo;
                delete hash[i];
            }
        }

        delete[] ocupado;
        delete[] hash;
        ocupado = nuevoOcupado;
        hash = nuevoHash;
        largo = nuevoLargo;
    }

    void insertarAux(int d, string nombre) {
        if (cantElem >= 0.7 * largo) {
            rehash();
        }

        int intento = 0;
        int indice = dobleHash(d, intento);
        while (ocupado[indice] && hash[indice] && hash[indice]->id != d) {
            intento++;
            indice = dobleHash(d, intento);
        }

        if (ocupado[indice] && hash[indice] && hash[indice]->id == d) {
            hash[indice]->titulo = nombre;
            if(!hash[indice]->habilitado){
                hash[indice]->habilitado = true;
                cantHab++;
                cantInhab--;
            }
        } else {
            libro* nuevo = new libro;
            nuevo->id = d;
            nuevo->titulo = nombre;
            nuevo->habilitado = true;
            hash[indice] = nuevo;
            ocupado[indice] = true;
            cantElem++;
            cantHab++;
        }
    }

    void buscarAux(int d) {
        int intento = 0;
        int indice = dobleHash(d, intento);
        while (ocupado[indice] && hash[indice] && hash[indice]->id != d) {
            intento++;
            indice = dobleHash(d, intento);
        }
        if (ocupado[indice] && hash[indice] && hash[indice]->id == d) {
            cout << hash[indice]->titulo << " " << (hash[indice]->habilitado ? "H" : "D") << endl;
        } else {
            cout << "libro_no_encontrado" << endl;
        }
    }

    void habilitarAux(int d) {
        int intento = 0;
        int indice = dobleHash(d, intento);
        while (ocupado[indice] && hash[indice] && hash[indice]->id != d) {
            intento++;
            indice = dobleHash(d, intento);
        }

        if (ocupado[indice] && hash[indice] && hash[indice]->id == d) {
            if (hash[indice]->habilitado) {
                cantHab--;
                cantInhab++;
            } else {
                cantHab++;
                cantInhab--;
            }
            hash[indice]->habilitado = !hash[indice]->habilitado;
        } else {
            cout << "libro_no_encontrado" << endl;
        }
    }

    void cantLibrosAux() {
        cout << cantElem << " " << cantHab << " " << cantInhab << endl;
    }

    // atributos
    libro** hash;
    bool* ocupado;
    int largo;
    int cantElem;
    int cantHab;
    int cantInhab;

public:
    Biblioteca(int lar) {
        largo = lar;
        hash = new libro*[lar]();
        ocupado = new bool[lar]();
        cantElem = 0;
        cantHab = 0;
        cantInhab = 0;
    }

    void insertar(int d, string nombre) {
        insertarAux(d, nombre);
    }

    void buscar(int d) {
        buscarAux(d);
    }

    void habilitar(int d) {
        habilitarAux(d);
    }

    void cantLibros() {
        cantLibrosAux();
    }

    ~Biblioteca() {
        delete[] ocupado;
        for (int i = 0; i < largo; ++i) {
            if (hash[i]) {
                delete hash[i];
            }
        }
        delete[] hash;
    }
};

int main() {
    int N;
    cin >> N;  // Leer el número de operaciones
    Biblioteca biblioteca(N*0.25);

    for (int i = 0; i < N; ++i) {
        string operacion;
        cin >> operacion;

        if (operacion == "ADD") {
            int id;
            string titulo;
            cin >> id >> titulo;
            biblioteca.insertar(id, titulo);
        } else if (operacion == "FIND") {
            int id;
            cin >> id;
            biblioteca.buscar(id);
        } else if (operacion == "TOGGLE") {
            int id;
            cin >> id;
            biblioteca.habilitar(id);
        } else if (operacion == "COUNT") {
            biblioteca.cantLibros();
        }
    }

    return 0;
}
