#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <string>
#include <vector>
#include "record.h"
#include "typedfile.h"

using namespace std;

template <class T>
class tree : private typedFile<T>
{
    public:
        tree();
        tree(const string name, const string type, const unsigned int version);
        virtual ~tree();
        bool insert(T key);
        void print();
        bool search(T key);
        bool remove(T key);
        record<T> getRoot();
        void setRoot(record<T> root);
        record<T> readPage(int i);
        bool isOpen();

    protected:
        void printAux(record<T> x, vector<string> &v, unsigned int lvl);
        unsigned int compareKey(record<T> x, T key);
        bool insertNotFull(record<T> x, T key, unsigned int i);
        bool split(record<T> x, unsigned int i, unsigned int iX);


    private:
        record<T> root;
};

template<class T>
tree<T>::tree()
{
    unsigned long long int rootIndex = typedFile<T>::getFirstValid();
    this->setRoot(this->readPage(rootIndex));
}

template<class T>
tree<T>::tree(const string name, const string type, const unsigned int version):typedFile<T>::typedFile(name,type,version)
{
    unsigned long long int rootIndex = typedFile<T>::getFirstValid();
    this->setRoot(this->readPage(rootIndex));
}


template<class T>
tree<T>::~tree(){}

template<class T>
record<T> tree<T>::readPage(int i)
{
    record<T> aux;
    typedFile<T>::readRecord(aux, i);
    return aux;
}

template<class T>
unsigned int tree<T>::compareKey(record<T> x, T key){
    unsigned int i = 0;

    while( i < x.getLenght() && key > x.getKey(i))
    {
        i++;
    }
    return i;
}

template<class T>
bool tree<T>::insertNotFull(record<T> x, T key, unsigned int i){
    //carrega o tamanho do no
    int j = x.getLenght() - 1;

    //se for folha insere
    if(x.isLeaf()){
        while(j >= 0 && key < x.getKey(j)){
            x.setKey(j+1, x.getKey(j));
            j--;
        }
        j++;
        x.setKey(j, key);
        x.setLenght(x.getLenght() + 1);
        typedFile<T>::writeRecord(x,i);
        return true;
    }else{//caso nao for folha
        record<T> aux;
        //verificar por onde descer
        int n = 0;
        while(n <= j && x.getKey(n) < key){
            n++;
        }

        aux = this->readPage(x.getChildren(n));

        if(aux.getLenght() == aux.MAX) { //se a pagina tiver cheia divide
            split(x,n,i); // split
            //verificar descida
            if(x.getKey(n) < key){
                n++;
            }
        }
        aux = this->readPage(x.getChildren(n));
        return insertNotFull(aux, key, x.getChildren(n));

    }

}

template<class T>
bool tree<T>::split(record<T> x, unsigned int i, unsigned int iX){
    record<T> y, z;

    y = this->readPage(x.getChildren(i));
    z.setLeaf(y.isLeaf()); // se y for folha z tambem ser�



    //copia para z o irm�o a direita
    for(int j = 0; j < z.MIN; j++){
        z.setKey(j, y.getKey(j + z.MIN));
    }
    z.setLenght(z.MIN); // novo tamanho do z

    if(y.isLeaf() == false){ //copia dos filhos
        for(int j=0; j < y.MIN + 1; j++){ //ate t + 1
            z.setChildren(j, y.getChildren(j + z.MIN));
        }
    }
    y.setLenght(y.MIN); //atualiza o tamanho de y ignorando os copiados para z

    for(int j = x.getLenght(); j >= i + 1; j--){ //reorganizando os filhos de x
        x.setChildren(j+1, x.getChildren(j));
        if(j==0){break;}
    }

    //posi��o que z ter�
    unsigned long long int pos = typedFile<T>::nextWriteIndex();

    x.setChildren(i + 1, pos); //filho da direita recebe a pos de um registro deletado ou do final do arquivo

    if(x.getLenght() > 0){ //realoca chaves de x para esquerda
        for(int j = x.getLenght() - 1; j >= i; j--){
            x.setKey(j + 1, x.getKey(j));
            if(j==0){break;}
        }
    }

    x.setKey(i, y.getKey(y.MIN));
    x.setLenght(x.getLenght() + 1);



    //escreve
    typedFile<T>::writeRecord(x, iX);
    typedFile<T>::writeRecord(y, x.getChildren(i));
    typedFile<T>::writeRecord(z, pos);

    return true;


}

template<class T>
bool tree<T>::insert(T key){
    record<T> x = this->getRoot(); //crio ponteiros auxiliares

    unsigned long long int rootIndex = typedFile<T>::getFirstValid(); //index da raiz
    unsigned int i = 0;

    if(x.getLenght() < x.MAX){//se raiz ainda nao estiver cheia
        insertNotFull(x, key, rootIndex); //insere
        this->setRoot(x);
        return true;
    }else { // se a raiz esta no tamanho maximo
        //prepara��o da raiz
        //nova raiz
        record<T> newRoot(false), aux; //nao folha

        newRoot.setChildren(0, rootIndex); //aponta para a raiz antiga

        typedFile<T>::insertRecord(newRoot); // insere a nova raiz

        rootIndex = typedFile<T>::getFirstValid(); //atualiza o index da raiz

        split(newRoot, 0, rootIndex); //chama o split para dividir o filho

        //inser��o da chave
        if ( newRoot.getKey(0) < key){ //esquerda ou direita
            i++;
        }
        aux = this ->readPage(newRoot.getChildren(i)); // carrega na memoria o filho em questao

        insertNotFull(aux, key, newRoot.getChildren(i));

        this->setRoot(newRoot);
    }
}

template<class T>
void tree<T>::print(){
    int lvl = 0;
    vector<string> levels(1);
    printAux(root, levels, lvl);

    for (string s : levels) {
        cout << s << endl;
    }
}

template<class T>
void tree<T>::printAux(record<T> x, vector<string> &v, unsigned int lvl){
    string aux = "[";
   unsigned int i = 0;

   if (v.size() < lvl + 1) {
      v.resize(lvl + 1);
   }

   if (!x.isLeaf()) {
      for (i = 0; i <= x.getLenght(); i++) {
         if (x.getChildren(i) != -1)
            //record<T> aux = this->readPage(x.getChildren(i));
            printAux(x.getChildren(i), v, lvl + 1);
      }
   }

   for (i = 0; i < x.getLenght(); i++) {
      aux += (x.getKey(i).getValue()) + ", "; //aux += to_string(x.getKey(i)) + ", ";
   }

   if (aux.length() > 1) {
      aux += "\b\b] ";
   } else {
      aux += "] ";
   }

   v[lvl] += aux;
}

template<class T>
bool tree<T>::search(T key){
    return false; //
}

template<class T>
bool tree<T>::remove(T key){
    return false;
}

//getter e setter da raiz
template<class T>
record<T> tree<T>::getRoot(){
    return this->root;
}
template<class T>
void tree<T>::setRoot(record<T> root){
    this->root = root;
}

template<class T>
bool tree<T>::isOpen(){
    return typedFile<T>::isOpen();
}

#endif // TREE_H
