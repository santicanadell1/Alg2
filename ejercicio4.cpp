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
    //heap
    int capacidad;
    int pos; // primero libre
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
        if (pos1 < 1 || pos1 >= pos || pos2 < 1 || pos2 >= pos) {
            std::cerr << "Swap indices out of bounds: " << pos1 << ", " << pos2 << std::endl;
            return;
        }
        actualizarHash(vec[pos1]->id, pos2);
        actualizarHash(vec[pos2]->id, pos1);
        pedido* aux = vec[pos1];
        vec[pos1] = vec[pos2];
        vec[pos2] = aux;
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
        if (pos == 1) return; 
        int posPadre = padre(pos);
        if (comparar(pos, posPadre)) {
            swap(pos, posPadre);
            flotar(posPadre);
        }
        else{
            return;
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

    //funciones auxiliares de la lista
    void insertarLista(posicion*& lista, int id, int pos) {
        if(!lista) {
            posicion* nuevo = new posicion;
            nuevo->id = id;
            nuevo->pos = pos;
            nuevo->sig = lista;
            lista = nuevo;
            cantElem++;
        }
        else if(lista->id == id){
            return;
        }
        else{
            insertarLista(lista->sig , id , pos);
        }
        
    }

    


    void actualizarLista(posicion*& lista, int id, int pos) {
        if(!lista) {
            return;
        }
        else if(lista->id == id){
            lista->pos = pos;
            return;
        }
        else{
            actualizarLista(lista->sig , id , pos);
        }
    }

    void eliminarLista(posicion*& lista, int id) {
        if(!lista) {
            return;
        }
        else if(lista->id == id){
            posicion* aux = lista;
            lista = lista->sig;
            delete aux;
            cantElem--;
            return;
        }
        else{
            eliminarLista(lista->sig , id);
        }
    }   
    //tabla de hash
    posicion** hash;
    int largo;
    int cantElem;
    
    int funcionHash1(int c) {
        return c % largo;
    }
    
void reHash()
    {
        int largoNuevo = (largo *2) + 1 ;
        posicion ** nuevo = new posicion * [largoNuevo]();
        int largoAnt = largo;
        largo = largoNuevo;
        for (int i = 0; i < largoAnt ; i++)
        {
            posicion * aux = hash[i];
            while (aux)
            {
                int pos = funcionHash1(aux->id);
                insertarLista(nuevo[pos],aux->id, aux->pos);
                posicion * temp = aux;
                aux = aux->sig;
                delete temp;
            }
        }
        delete[] this->hash;
        this->hash = nuevo;
    }

    void insertarHash(int id, int pos) {
        if(cantElem >= largo*0.7){
            cout << "Rehashing" << endl;
            reHash();

        }
        int posHash = funcionHash1(id);
        insertarLista(hash[posHash], id, pos);
    }

    int buscarHash(int id) {
        int posHash = funcionHash1(id);
        posicion* aux = hash[posHash];
        while(aux){
            if(aux->id == id){
                return aux->pos;
            }
            aux = aux->sig;
        }
        return -1;
    }

    void actualizarHash(int id, int pos) {
        int posHash = funcionHash1(id);
        actualizarLista(hash[posHash], id, pos);
    }

    void eliminarHash(int id) {
        int posHash = funcionHash1(id);
        eliminarLista(hash[posHash], id);
    }
public:
    colaDePrioridad(int tam) {
        
        vec = new pedido*[tam + 1](); // Inicialización a null
        capacidad = tam;
        pos = 1; // Comenzar desde 1
        cantElem = 0;
        largo = tam;
        hash = new posicion*[largo];
        for (int i = 0; i < largo; i++) {
              hash[i] = NULL;  // Asegúrate de que todas las posiciones se inicialicen en NULL
            }           

    }

    ~colaDePrioridad() {
        for (int i = 1; i < pos; i++) { // Liberar solo hasta pos
            delete vec[i];
        }
        delete[] vec;
        for(int i = 0; i < largo; i++){
            posicion* aux = hash[i];
            while(aux){
                posicion* aux2 = aux;
                aux = aux->sig;
                delete aux2;
            }
        }
        delete[] hash;
    }

    void encolar(int id, int prioridad, const string& items, bool paraLlevar) {
        if (!lleno()) {
            pedido* nuevo = new pedido;
            nuevo->id = id;
            nuevo->prioridad = prioridad;
            nuevo->items = items;
            nuevo->paraLlevar = paraLlevar;
            vec[pos] = nuevo;
            insertarHash(id, pos);
            flotar(pos);
            pos++;
        }
    }

    pedido* desencolar() {
        if (vacio()) return NULL; // Comprobar si el heap está vacío
        swap(1, pos - 1);
        pedido* desencolado = vec[pos - 1];
        vec[pos - 1]= NULL;
        hundir(1);
        eliminarHash(desencolado->id);
        return desencolado;
    }

    void eliminar(int id) {
        int posicion = buscarHash(id);
        if(pos != -1){
            pedido* desencolado = vec[posicion];
            swap(posicion, pos - 1);
            vec[pos - 1] = NULL;
            pos--;
            flotar(posicion);
            hundir(posicion);
            eliminarHash(id);
            delete desencolado;
        }
    }

    void cambiarPedido(int id) {
        int posicion = buscarHash(id);
        if(posicion != -1){
            vec[posicion]->paraLlevar = !vec[posicion]->paraLlevar;
            flotar(posicion);
            hundir(posicion);
        }
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
            cout<< "encolado" << endl;

        } else if (operacion == "E") {
            int id;
            cin >> id;
            cola.eliminar(id);  // Eliminar pedido por ID
            cout<< "eliminado" << endl;

        } else if (operacion == "C") {
            int id;
            cin >> id;
            cola.cambiarPedido(id);  // Cambiar estado para llevar del pedido
            cout<< "cambiado" << endl;
        }
    }

    while(cola.cantElemEncolados() > 0) {
        pedido* p = cola.desencolar();
        cout << p->id << " " << p->prioridad << " " << (p->paraLlevar ? "true" : "false") << " " << p->items << endl;
        delete p;
    }
    return 0;
}