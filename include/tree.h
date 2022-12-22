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
    int search(T key, record<T> x);
    bool remove(T key, record<T> x, unsigned long long int pageIndex);
    bool rotate(record<T> father, unsigned long long int index, unsigned long long int fatherIndex, bool side);
    record<T> getRoot();
    void setRoot(record<T> root);
    record<T> readPage(int i);
    unsigned long long getRootIndex();
    bool isOpen();
    bool close();
protected:
    void printAux(record<T> x, vector<string> &v, unsigned int lvl);
    bool insertNotFull(record<T>& x, T key, unsigned int i);
    bool split(record<T>& x, unsigned int i, unsigned int iX);

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
bool tree<T>::close()
{
    return typedFile<T>::close();
}

template<class T>
unsigned long long tree<T>::getRootIndex()
{
    return typedFile<T>::getFirstValid();
}

template<class T>
tree<T>::~tree() {}

template<class T>
record<T> tree<T>::readPage(int i)
{
    record<T> aux;
    typedFile<T>::readRecord(aux, i);
    return aux;
}

template<class T>
bool tree<T>::insertNotFull(record<T>& x, T key, unsigned int i)
{
    //se for folha insere
    if(x.isLeaf())
    {
        int j = x.getLenght() - 1; //j = tamanho -1 vetor começa em 0
        while(j >= 0 && key < x.getKey(j))
        {
            x.setKey(j + 1, x.getKey(j));
            j--;
        }
        x.setKey(j + 1, key);
        x.setLenght(x.getLenght() + 1);
        typedFile<T>::writeRecord(x,i);

        return true;
    }

    //caso nao for folha
    int j = x.getLenght() - 1; //j = tamanho -1 vetor começa em 0
    record<T> aux;
    //verificar por onde descer
    while(j >= 0 && x.getKey(j) > key)
    {
        j--;
    }
    j++;

    //aux = this->readPage(x.getChildren(n));
    aux = this->readPage(x.getChildren(j));

    if(aux.getLenght() == aux.MAX)   //se a pagina tiver cheia divide
    {
        split(x,j,i); // split
        //verificar descida
        if(x.getKey(j) < key)
        {
            j++;
        }
    }
    aux = this->readPage(x.getChildren(j));

    return insertNotFull(aux, key, x.getChildren(j));
}

template<class T>
bool tree<T>::split(record<T>& x, unsigned int i, unsigned int iX)
{
    cout << " split " << endl;
    record<T> y, z;

    y = this->readPage(x.getChildren(i));
    z.setLeaf(y.isLeaf()); // se y for folha z tambem será

    //copia para z o irmão a direita
    for(int j = 0; j < z.MIN; j++)
    {
        z.setKey(j, y.getKey(j + z.MIN + 1));
        cout << " z.setKey(getkey(y)): " << y.getKey(j + z.MIN + 1).getValue() << endl;
    }
    z.setLenght(z.MIN); // novo tamanho do z
    cout << " z.Lenght: " << z.MIN << endl;


    if(!y.isLeaf())  //copia dos filhos
    {
        for(int j=0; j < y.MIN + 1; j++)  //ate t + 1
        {
            z.setChildren(j, y.getChildren(j + z.MIN));
        }
    }
    y.setLenght(y.MIN); //atualiza o tamanho de y ignorando os copiados para z

    for(int j = x.getLenght(); j >= i + 1; j--)  //reorganizando os filhos de x
    {
        x.setChildren(j+1, x.getChildren(j));
        if(j==0)
        {
            break;
        }
    }

    //posição que z terá
    unsigned long long int pos = typedFile<T>::nextWriteIndex();

    x.setChildren(i + 1, pos); //filho da direita recebe a pos de um registro deletado ou do final do arquivo

    if(x.getLenght() > 0)  //realoca chaves de x para esquerda
    {
        for(int j = x.getLenght() - 1; j >= i; j--)
        {
            x.setKey(j + 1, x.getKey(j));
            if(j==0)
            {
                break;   //necessario
            }
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
bool tree<T>::insert(T key)
{
    record<T> x = this->getRoot(); //crio ponteiros auxiliares

    unsigned long long int rootIndex = typedFile<T>::getFirstValid(); //index da raiz
    unsigned int i = 0;

    cout << "----------------------------------------------------------" << key.getValue() << endl;

    if(x.getLenght() == x.MAX)
    {

        record<T> newRoot(false), aux; //nao folha

        newRoot.setChildren(0, rootIndex); //aponta para a raiz antiga

        typedFile<T>::insertRecord(newRoot); // insere a nova raiz

        split(newRoot, 0, rootIndex); //chama o split para dividir o filho

        //rootIndex = typedFile<T>::getFirstValid(); //atualiza o index da raiz

        //inserção da chave
        if ( newRoot.getKey(0) < key)  //esquerda ou direita
        {
            i++;
        }
        aux = this->readPage(newRoot.getChildren(i)); // carrega na memoria o filho em questao

        insertNotFull(aux, key, newRoot.getChildren(i));

        this->setRoot(newRoot);

        return true;
    }
    else
    {
        insertNotFull(x, key, rootIndex); //insere

        this->setRoot(x);

        return true;
    }
    return false;
}

template<class T>
void tree<T>::print()
{
    int lvl = 0;
    vector<string> levels(1);
    printAux(this->root, levels, lvl);

    for (string s : levels)
    {
        cout << s << endl;
    }
}

template<class T>
void tree<T>::printAux(record<T> x, vector<string> &v, unsigned int lvl)
{
    string aux = "[";
    string bef, aft, str = "";
    size_t found;
    unsigned int i = 0;

    if (v.size() < lvl + 1)
    {
        v.resize(lvl + 1);
    }
    if (!x.isLeaf())   //se pag não for folha
    {
        for (i = 0; i <= x.getLenght(); i++)
        {
            if (x.getChildren(i) != -1)
            {
                record<T> rec= this->readPage(x.getChildren(i));
                printAux(rec, v, lvl + 1);
            }
        }
    }
    for (i = 0; i < x.getLenght(); i++)
    {
        aux += x.getKey(i).getString() + ", ";
    }

    if (aux.length() > 1)
    {
        aux += "\b\b] ";
    }
    else
    {
        aux += "] ";
    }

    v[lvl].append(aux);
}

template<class T>
int tree<T>::search(T key, record<T> x )
{
    unsigned int index = 0;
    record<T> child;

    while( x.getLenght() > index && key.getValue() > x.getKey(index).getValue())
    {
        //cout << " entrou no while " << endl;
        index++;
        //cout << index << endl;
    }
    cout << "Pagina " << index;

    if(key.getValue() == x.getKey(index).getValue())
    {
        //cout << x.getKey(index).getValue()  << endl;
        //cout << key.getValue()  << endl;
        //cout << "igua=l " << index << endl;
        cout << "Index do registro " << index << endl;
        return index;
    }

    if(!x.isLeaf())
    {
        //esquerda
        if(key.getValue() < x.getKey(index).getValue())
        {
            //cout << "menor " << endl;
            child = readPage(x.getChildren(index));
            search(key, child);
        }
        else
        {
            //direita
            //cout << "maior " << endl;
            child = readPage(x.getChildren(index));
            search(key,child);
        }
    }
    return index;
}

template<class T>
bool tree<T>::remove(T key, record<T> x, unsigned long long int pageIndex)
{

    if(x.getLenght() == 0)
    {
        // vazio
        return true;
    }

    unsigned int index = 0;
    record<T> child;
    record<T>& father = x;
    unsigned long long int fatherIndex = this->getRootIndex();

    while( x.getLenght() > index && key.getValue() > x.getKey(index).getValue())
    {
        //cout << " entrou no while " << endl;
        index++;
        //cout << index << endl;
    }

    if(key.getValue() == x.getKey(index).getValue() && x.isLeaf())
    {

        //se tiver numero minimo de chaves
        //rotacao -> verifica se os algum dos irmaos tem mais que t-1 chaves
        //->desce a chave do nó pai e sobe o predescessor ou sucessor -> remove
        //fusao -> se o irmao da esquerda e o no == t-1 chaves fusão com a chave do pai


        //remocao caso 1
        for(int i = index; i < x.getLenght(); i++ )//reorganiza chaves
        {
            //cout << "FOR == " << i << endl;
            x.setKey(i, x.getKey(i+1));
            //x.setChildren(i, x.getChildren(i+1)); //folhao não precisa reorganizar filhos
        }
        x.setLenght(x.getLenght() - 1);
        typedFile<T>::writeRecord(x, pageIndex);

        if(pageIndex == this->getRootIndex()) //se for remover da raiz
        {
            this->root = x;
        }
        return true;
    }

    if(!x.isLeaf())
    {
        father = x; //pai
        record<T> right, left;

        if(key.getValue() == x.getKey(index).getValue())//achou o valor na pagina interna
        {
            if( x.getLenght() > x.MIN)
            {
                //teste
                child = readPage(x.getChildren(index)); //filho da esquerda
                if(child.getLenght() > child.MIN)
                {
                    x.setKey(index, child.getKey(child.getLenght() - 1));
                    child.setLenght( child.getLenght() - 1 );
                    typedFile<T>::writeRecord(x, pageIndex);
                    typedFile<T>::writeRecord(child, x.getChildren(index));

                }else{
                    right = readPage(x.getChildren(index + 1)); //filho da direita
                    if(right.getLenght() > right.MIN){
                        x.setKey(index, right.getKey(0));

                        for(int i = 0; i < right.getLenght(); i++){ //reorganizar chaves
                            right.setKey(i, right.getKey(i + 1) );
                        }

                        right.setLenght( right.getLenght() - 1 );
                        typedFile<T>::writeRecord(x, pageIndex);
                        typedFile<T>::writeRecord(right, x.getChildren(index + 1));
                    }
                }
            }
            if(pageIndex == this->getRootIndex()) //se for remover da raiz
                    {
                        this->root = x;
                    }
        }

        //esquerda
        if(key.getValue() < x.getKey(index).getValue())
        {
            //cout << "menor " << endl;
            child = readPage(x.getChildren(index));
            right = readPage(x.getChildren(index + 1));

            if( child.getLenght() <= child.MIN)   //caso tem t-1 chaves
            {
                if(right.getLenght() > right.MIN)
                {
                    rotate(father, index, fatherIndex, true);
                    child = readPage(x.getChildren(index));
                    remove(key, child, x.getChildren(index)); // TODO
                }
                else
                {
                    //fusao
                }
            }
            else   // caso tem mais que t chaves recursao remove
            {
                fatherIndex = x.getChildren(index); //fatherIndex atualiza
                remove(key, child, x.getChildren(index));
            }


        }
        else
        {
            //direita
            //cout << "maior " << endl;
            child = readPage(x.getChildren(index));
            left = readPage(x.getChildren(index - 1));

            if( child.getLenght() <= child.MIN)   //caso tem t-1 chaves
            {
                if(left.getLenght() > left.MIN)
                {
                    rotate(father, index, fatherIndex, false);
                    child = readPage(x.getChildren(index));
                    remove(key, child, x.getChildren(index)); // TODO
                }
                else
                {
                    //fusao
                }
            }
            else   // caso tem mais que t chaves recursao remove
            {
                fatherIndex = x.getChildren(index); //fatherIndex atualiza
                remove(key, child, x.getChildren(index));
            }

    }
    }

    return false;
}

template<class T>
bool tree<T>::rotate( record<T> father, unsigned long long int index, unsigned long long int fatherIndex, bool side)
{
    record<T> right, left;

    left = readPage(father.getChildren(index));
    right = readPage(father.getChildren(index + 1));

    //cout << "pos0 do fathe r " << father.getKey(index).getValue() << endl;

    if(side == true) //rotação esquerda(t-1) direita(t>min)
    {
        left.setKey(left.getLenght(), father.getKey(index));
        left.setLenght(left.getLenght() + 1);

        cout << father.getKey(index).getValue() << " chave father(index)" << endl;

        father.setKey(index, right.getKey(0));

        for(int i = 0; i < right.getLenght(); i++){ //reorganizar chaves
             right.setKey(i, right.getKey(i + 1));
             //atualizar children
        }
        right.setLenght( right.getLenght() - 1 );

        typedFile<T>::writeRecord(father, fatherIndex);
        typedFile<T>::writeRecord(left, father.getChildren(index));
        typedFile<T>::writeRecord(right, father.getChildren(index+1));
    }
    else {//rotação direita(t-1) esquerda(t>min)
        for(int i = right.getLenght(); i <= 0; i--){ //reorganizar chaves
             right.setKey(i, right.getKey(i - 1) );
             //atualizar children
        }
        right.setLenght(right.getLenght() + 1);

        right.setKey(0, father.getKey(index));

        father.setKey(index, left.getKey(left.getLenght()-1));

        left.setLenght( left.getLenght()  - 1 );

        typedFile<T>::writeRecord(father, fatherIndex);
        typedFile<T>::writeRecord(left, father.getChildren(index));
        typedFile<T>::writeRecord(right, father.getChildren(index+1));
    }
    cout << fatherIndex << "   ===   " << this->getRootIndex() << endl;

    if(fatherIndex == this->getRootIndex()) //se for remover da raiz
    {
        this->root = father;
        return true;
    }
    return false;
}

//getter e setter da raiz
template<class T>
record<T> tree<T>::getRoot()
{
    return this->root;
}
template<class T>
void tree<T>::setRoot(record<T> root)
{
    this->root = root;
}

template<class T>
bool tree<T>::isOpen()
{
    return typedFile<T>::isOpen();
}

#endif // TREE_H
