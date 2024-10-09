#include <cassert>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

// Función auxiliar para obtener el máximo entre dos valores
int max(int a, int b){
    return a > b ? a : b;
}

class Biblioteca {
    private:
        struct nodoAvl{
            int id;
            string titulo;
            nodoAvl* der;
            nodoAvl* izq;
            int altura;
            bool habilitado;
        };
        
        // Funciones auxiliares
        int alturaAux(nodoAvl* nodo){
            if (!nodo) return 0;
            return 1 + max(alturaAux(nodo->der), alturaAux(nodo->izq));
        }

        int balanceAux(nodoAvl* nodo){
            int alturaDer = nodo->der ? nodo->der->altura : 0;
            int alturaIzq = nodo->izq ? nodo->izq->altura : 0;
            return alturaDer - alturaIzq;
        }

        nodoAvl* rotaIzq(nodoAvl* A){
            nodoAvl* B = A->der;
            nodoAvl* T2 = B->izq;
            A->der = T2;
            B->izq = A;
            A->altura = 1 + max(A->der ? A->der->altura : 0, A->izq ? A->izq->altura : 0);
            B->altura = 1 + max(B->der ? B->der->altura : 0, B->izq ? B->izq->altura : 0);
            return B;
        }

        nodoAvl* rotaDer(nodoAvl* B){
            nodoAvl* A = B->izq;
            nodoAvl* T2 = A->der;
            B->izq = T2;
            A->der = B;
            B->altura = 1 + max(B->der ? B->der->altura : 0, B->izq ? B->izq->altura : 0);
            A->altura = 1 + max(A->der ? A->der->altura : 0, A->izq ? A->izq->altura : 0);
            return A;
        }

        nodoAvl* insertarAux(nodoAvl* nodo, int d, string titulo){
            if (!nodo) {
                nodoAvl* nuevo = new nodoAvl;
                nuevo->altura = 1;
                nuevo->id = d;
                nuevo->titulo = titulo;
                nuevo->habilitado = true;
                nuevo->der = NULL;
                nuevo->izq = NULL;
                cantHab++;
                cantTotal++;
                return nuevo;
            }

            if (nodo->id < d) {
                nodo->der = insertarAux(nodo->der, d, titulo);
            } else if (nodo->id > d) {
                nodo->izq = insertarAux(nodo->izq, d, titulo);
            } else {
                nodo->titulo = titulo;
                if(nodo->habilitado == false){
                    cantHab++;
                    cantInhab--;
                }
                nodo->habilitado = true;

                return nodo;
            }

            nodo->altura = 1 + max(nodo->izq ? nodo->izq->altura : 0, nodo->der ? nodo->der->altura : 0);
            int balance = balanceAux(nodo);
            bool desbalanceDer = balance > 1;
            bool desbalanceIzq = balance < -1;

            // Caso derecha-derecha
            if (desbalanceDer && nodo->der && nodo->der->id < d) {
                return rotaIzq(nodo);
            }

            // Caso derecha-izquierda
            if (desbalanceDer && nodo->der && nodo->der->id > d) {
                nodo->der = rotaDer(nodo->der);
                return rotaIzq(nodo);
            }

            // Caso izquierda-derecha
            if (desbalanceIzq && nodo->izq && nodo->izq->id < d) {
                nodo->izq = rotaIzq(nodo->izq);
                return rotaDer(nodo);
            }

            // Caso izquierda-izquierda
            if (desbalanceIzq && nodo->izq && nodo->izq->id > d) {
                return rotaDer(nodo);
            }

            return nodo;
        }

        void inOrderAux(nodoAvl* nodo){
            if (!nodo) return;
            inOrderAux(nodo->izq);
            cout << nodo->id << endl;
            inOrderAux(nodo->der);
        }

        void buscarAux(nodoAvl* nodo, int d){
            if (!nodo) {
                cout << "libro_no_encontrado" << endl;
            } else if (d > nodo->id) {
                buscarAux(nodo->der, d);
            } else if (d < nodo->id) {
                buscarAux(nodo->izq, d);
            } else {
                cout << nodo->titulo << " " << (nodo->habilitado ? "H" : "D") << endl;
            }
        }

        void habilitarAux(nodoAvl* nodo, int d){
            if (!nodo) {
                cout << "libro_no_encontrado" << endl;
                return;
            }
            if (nodo->id == d) {
                if (nodo->habilitado) {
                    cantHab--;
                    cantInhab++;
                } else {
                    cantHab++;
                    cantInhab--;
                }
                nodo->habilitado = !nodo->habilitado;
            } else if (nodo->id > d) {
                habilitarAux(nodo->izq, d);
            } else if (nodo->id < d) {
                habilitarAux(nodo->der, d);
            }
        }

        void cantLibrosAux(nodoAvl* nodo){
            cout << to_string(cantTotal) + " " + to_string(cantHab) + " " + to_string(cantInhab) << endl;
        }

        void destruirNodo(nodoAvl* nodo){
            if (!nodo) return;
            destruirNodo(nodo->der);
            destruirNodo(nodo->izq);
            delete nodo;
        }

        // Atributos
        nodoAvl* raiz;
        int cantHab;
        int cantInhab;
        int cantTotal;

    public:
        // Funciones públicas
        void insertar(int d, string titulo){
            raiz = insertarAux(raiz, d, titulo);
        }

        int altura(){
            if (!raiz) return 0;
            return raiz->altura;
        }

        void inOrder(){
            inOrderAux(raiz);
        }

        void buscar(int d){
            buscarAux(raiz, d);
        }

        void habilitar(int id){
            habilitarAux(raiz, id);
        }

        void cantLibros(){
            cantLibrosAux(raiz);
        }

        void eliminarAVL(){
            destruirNodo(raiz);
            raiz = NULL;
        }

        Biblioteca() {
            raiz = NULL;
            cantHab = 0;
            cantInhab = 0;
            cantTotal = 0;
        }

        ~Biblioteca() {
            destruirNodo(raiz);
            raiz = NULL;
        }
};

int main() {
    int N;
    cin >> N;  // Leer el número de operaciones
    Biblioteca biblioteca;

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
