#include <iostream>

using namespace std;

template <typename T>
struct Nodo {
    T data;
    Nodo<T>* derecha;
    Nodo<T>* izquierda;
    Nodo<T>* padre;
    Nodo<T>* hijo;
    bool mark; // indica si perdió o no un hijo
    int rank; // cantidad de hijos
    Nodo(): data(0), derecha(nullptr), izquierda(nullptr), padre(nullptr), hijo(nullptr), mark(false), rank(0){};
    Nodo(T valor): data(valor), derecha(nullptr), izquierda(nullptr), padre(nullptr), hijo(nullptr), mark(false), rank(0){};
};

//Función opcional para imprimir la lista enlazada desde un nodo
template<typename T>
void print_nodo(Nodo<T> *nodo);

template<typename T>
class FHeap{
    private:
    Nodo<T>* rootlist: // lista raiz del heap
    Nodo<T>* ptr_min; // puntero al nodo min de rootlist
    int cont; // contador de la cantidad de nodos en el heap
    public:
    FHeap(): rootlist(nullptr), ptr_min(nullptr), cont(0){};
    // declaracion de los metodos 
    // de la clase principal
    void insert(T valor);
    T valor_min();
    bool empty();
    int size();

    FHeap Union(FHeap<T>);
    FHeap decreaseMin();
    //Función opcional para imprimir rootlist
    void print() {
        print_nodo(rootlist);
    }

};

template<typename T>
void print_nodo(Nodo<T> *nodo) {
    std::cout << "Se van a imprimir los valores del nodo.\n";
    auto temp = nodo;
    while (true) {
        std::cout << temp->data << ' ';
        if (temp->derecha == nodo) break;
        temp = temp->derecha;
    }
    std::cout << '\n';
}

//Función para que se comparen dos nodos raíz, y quien tenga mayor valor se vuelve hijo del otro
template<typename T>
Nodo<T> *linking(Nodo<T> *N1, Nodo<T> *N2) {
    Nodo<T> *NDown = N1->data >= N2->data ? N1 : N2;
    Nodo<T> *NUp = NDown == N1 ? N2 : N1;

    if (NUp->hijo == nullptr) {
        NUp->hijo = NDown;
        NDown->izquierda = NDown;
        NDown->derecha = NDown;
    } else {
        Nodo<T> *primerHijo = NUp->hijo;
        Nodo<T> *ultimoHijo = primerHijo->izquierda;

        NDown->izquierda = ultimoHijo;
        NDown->derecha = primerHijo;
        ultimoHijo->derecha = NDown;
        primerHijo->izquierda = NDown;
    }
    NDown->padre = NUp;

    NUp->rank++;
    return NUp;
}

template <typename T>
void FHeap<T>::insert(T valor){
    // creamos una pequeña lista circular
    // con el nodo a insertar
    Nodo<T>* nnodo = new Nodo<T>(valor);
    nnodo->derecha = nnodo->izquierda = nnodo;

    // preguntamos si el rootlist esta vacío o no
    if(rootlist == nullptr){
        // en caso lo sea, el nuevo nodo ocupada ese lugar
        rootlist = nnodo;
        // al igual que el puntero minimo
        ptr_min = nnodo;
    } else {
        // en caso rootlist no sea vacío
        nnodo->derecha = rootlist;
        nnodo->izquierda = rootlist->izquierda;
        rootlist->izquierda->derecha = nnodo;
        root->izquirda = nnodo;
        // actualizamos el puntero minimo si es el caso
        if (nnodo->data < ptr_min->data){
            ptr_min = nnodo;
        }
    }
    // incrementamos en 1 la cantidad de nodos en el Heap
    cont = cont + 1;
};

template <typename T>
T FHeap<T>::valor_min(){
    return ptr_min->data;
};

template <typename T>
bool FHeap<T>::empty(){
    return rootlist == nullptr;
};

template <typename T>
int FHeap<T>::size(){
    return cont;
}

//Función para unir el FHeap actual con otro. Se vacía luego FH2
template<typename T>
FHeap<T> FHeap<T>::Union(FHeap<T> FH2) {
    if (FH2.rootlist != nullptr) {
        this->ptr_min =
                this->valor_min() < FH2.valor_min() ? this->ptr_min : FH2.ptr_min; //Obtenemos el nodo con menor valor

        //Empezamos con la unión
        auto temp = rootlist->izquierda;
        this->rootlist->izquierda->derecha = FH2.rootlist;
        this->rootlist->izquierda = FH2.rootlist->izquierda;
        FH2.rootlist->izquierda->derecha = this->rootlist;
        FH2.rootlist->izquierda = temp;

        this->cont += FH2.cont;

        //Eliminamos FH2
        FH2.rootlist = nullptr;
        FH2.ptr_min = nullptr;
        FH2.cont = 0;
    }
    return *this;
}

//Función que elimina el menor valor del FHeap. Funciona al 100%
template<typename T>
FHeap<T> FHeap<T>::decreaseMin() {
    if (this->rootlist != nullptr) {
        if (cont == 1) { //Si solo hay un nodo, lo eliminamos
            delete[] ptr_min;
            ptr_min = nullptr;
            delete[] rootlist;
            rootlist = nullptr;
            cont--;
            return *this;
        }

        if (this->ptr_min->hijo != nullptr) {
            ptr_min->hijo->derecha->izquierda = rootlist;
            ptr_min->hijo->derecha = rootlist->derecha;
            rootlist->derecha->izquierda = ptr_min->hijo;
            rootlist->derecha = ptr_min->hijo->derecha;

            ptr_min->hijo->padre = nullptr;
            ptr_min->hijo = nullptr;
        }
        auto t = ptr_min;
        ptr_min->izquierda->derecha = ptr_min->derecha;
        ptr_min->derecha->izquierda = ptr_min->izquierda;
        ptr_min = ptr_min->derecha; //Esto de forma momentánea.
        delete t;
        this->cont--;

        //Empezamos con la rutina: unir árboles del rootList con igual rango (rank).
        int D = static_cast<int>(std::log(this->cont) / std::log(2)) + 1; //Máxima cantidad de hijos de un árbol
        auto **lista = new Nodo<T> *[D]; //puntero de punteros a Nodo de largo log_2(n_nodos en FHeap) + 1
        for (int i = 0; i < D; i++) {
            *(lista + i) = nullptr;
        }

        auto temp = rootlist;
        Nodo<T> *temp2 = temp; //Para recorrer toda la rootList
        do {
            auto temp3 = temp2->derecha; //Guarda lo que estaba originalmente a la derecha de temp2
            // Mientras el rango de temp2 ya esté ocupado en la lista
            while (*(lista + temp2->rank) != nullptr) {
                int d = temp2->rank;   // Guardas el rank actual
                temp2 = linking(temp2, *(lista + d));  // Unes los árboles con el mismo rango
                *(lista + d) = nullptr;  // Limpias la entrada
            }

            *(lista + temp2->rank) = temp2;  // Asigna temp2 a su rango en lista
            temp2 = temp3;  // Mueve temp2 a la derecha

        } while (temp != temp2);  // Continúa hasta que completes el ciclo

        //Creamos un nuevo heap con cada valor de la lista.
        this->rootlist = nullptr;
        for (int i = 0; i < D; i++) {
            if (*(lista + i) != nullptr) {
                if (rootlist == nullptr) {
                    rootlist = *(lista + i);
                    rootlist->izquierda = rootlist->derecha = rootlist;
                } else {
                    (*(lista + i))->izquierda = rootlist->izquierda;
                    (*(lista + i))->derecha = rootlist;
                    rootlist->izquierda->derecha = *(lista + i);
                    rootlist->izquierda = *(lista + i);
                }
                if ((*(lista+i))->data < ptr_min->data) ptr_min = *(lista+i);
            }
        }
    }
    return *this;
}

int main(){
    return 0;
};
