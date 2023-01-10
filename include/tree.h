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
    bool split(record<T>& x, unsigned int index, unsigned int indexofX);

    bool removeFromLeaf(record<T> x, unsigned int index, unsigned long long int pageIndex);
    bool removeFromInternalPage(record<T> x, unsigned int index, unsigned long long int pageIndex);
    bool downAndOrganize(T key, record<T> x, unsigned int index, unsigned long long int fatherIndex);
    bool merge(record<T>& father, unsigned int index, unsigned long long int fatherIndex);
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
bool tree<T>::split(record<T>& x, unsigned int index, unsigned int indexofX)
{
    cout << " split " << endl;
    record<T> left, right;

    left = this->readPage(x.getChildren(index)); //pag antiga cheia
    right.setLeaf(left.isLeaf()); // se pag antiga for folha a direita tbm sera

    //copia para right a quantidade de chaves MIN, de MIN até MAX
    for(int j = 0; j < right.MIN; j++)
    {
        right.setKey(j, left.getKey(j + right.MIN + 1)); //set pos 0 ate MIN
        //cout << " z.setKey(getkey(y)): " << left.getKey(j + right.MIN + 1).getValue() << endl;
    }

    if(!right.isLeaf()) //se nao for folha tem que copiar os filhos
    {
        for(int j = 0; j < right.MIN; j++) //de 0 ate MIN+1
        {
            right.setChildren(j, left.getChildren(j+ right.MIN));  //setchildren pos 0 ate MIN + 1
            //cout << " z.setKey(getkey(y)): " << left.getKey(j + right.MIN + 1).getValue() << endl;
        }
    }

    right.setLenght(right.MIN); // novo tamanho do filho a direita
    left.setLenght(left.MIN); //atualiza o tamanho do filho da esquerda, ignora os copiados

    for(int j = x.getLenght(); j >= index + 1; j--)  //reorganizando os filhos de x
    {
        x.setChildren(j + 1, x.getChildren(j));
        if(j==0)
        {
            break;
        }
    }

    //posição que z terá
    unsigned long long int pos = typedFile<T>::nextWriteIndex();
    //filho da direita recebe a pos de um registro deletado ou do final do arquivo
    x.setChildren(index + 1, pos);

    if(x.getLenght() > 0)  //realoca chaves de x para esquerda
    {
        for(int j = x.getLenght() - 1; j >= index; j--)
        {
            x.setKey(j + 1, x.getKey(j));
            if(j==0)
            {
                break;   //necessario
            }
        }
    }

    x.setKey(index, left.getKey(left.MIN));
    x.setLenght(x.getLenght() + 1);

    //escreve
    typedFile<T>::writeRecord(x, indexofX);
    typedFile<T>::writeRecord(left, x.getChildren(index));
    typedFile<T>::writeRecord(right, pos);

    return true;
}

template<class T>
bool tree<T>::insert(T key)
{
    record<T> x = this->getRoot(); //crio ponteiros auxiliares

    unsigned long long int rootIndex = typedFile<T>::getFirstValid(); //index da raiz
    unsigned int i = 0;

    //cout << "----------------------------------------------------------" << key.getValue() << endl;

    if(x.getLenght() == x.MAX) //se chegou no tamanho maximo
    {
        record<T> newRoot, aux;
        newRoot.setLeaf(false); //nao folha

        newRoot.setChildren(0, rootIndex); //aponta para a raiz antiga

        typedFile<T>::insertRecord(newRoot); // insere a nova raiz

        split(newRoot, 0, rootIndex); //chama o split para dividir o filho com root index antigo

        //rootIndex = typedFile<T>::getFirstValid(); //atualiza o index da raiz

        //busca posicao para inserir chave
        if ( newRoot.getKey(0) < key)  //esquerda ou direita
        {
            i++;
        }
        aux = this->readPage(newRoot.getChildren(i)); // carrega na memoria o filho em que vai inserir

        insertNotFull(aux, key, newRoot.getChildren(i)); //insere

        this->setRoot(newRoot); //atualiza a raiz

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
    //busca a chave na pagina e soma no index
    while( x.getLenght() > index && key.getValue() > x.getKey(index).getValue())
    {
        index++;
    }
    cout << "Pagina " << index;

    if(key.getValue() == x.getKey(index).getValue())
    {
        cout << "Index do registro " << index << endl;
        return index;
    }

    if(!x.isLeaf())
    {
        //esquerda
        if(key.getValue() < x.getKey(index).getValue())
        {
            child = readPage(x.getChildren(index));
            search(key, child);
        }
        else
        {
            //direita
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
    //busca a chave na pagina e soma no index
    while( x.getLenght() > index && key.getValue() > x.getKey(index).getValue())
    {
        index++;
    }
    //testa na posição do index se chegou no valor desejado, e se é folha
    //remocao caso 1
    if(key.getValue() == x.getKey(index).getValue() && x.isLeaf())
    {
        return this->removeFromLeaf(x, index, pageIndex);
    }
    //caso seja pagina interna
    if(!x.isLeaf())
    {
        unsigned long long int fatherIndex = this->getRootIndex();

        if(key.getValue() == x.getKey(index).getValue())//achou o valor na pagina interna
        {
            return this->removeFromInternalPage(x, index, pageIndex);
        }

        //função que vai reorganizar as paginas chamando rotacao e fusao e remocao
        downAndOrganize(key, x, index, fatherIndex);
    }

    return false;
}
template<class T>
bool tree<T>::removeFromLeaf(record<T> x, unsigned int index, unsigned long long int pageIndex)
{
    cout << "remove from leaf " << endl;
    cout << "index = " << index << endl;
    //reorganiza chaves, sobreescrevendo o valor removido
    for(int i = index; i < x.getLenght(); i++ )
    {
        x.setKey(i, x.getKey(i+1)); //set key recebe o proximo
        //como e folha não precisa reorganizar filhos
        cout << x.getKey(i+1).getValue() <<  endl;
    }
    x.setLenght(x.getLenght() - 1); //diminui 1 do tamanho da pag
    typedFile<T>::writeRecord(x, pageIndex); //escreve

    //caso for removido da raiz deve atualizar o root
    if(pageIndex == this->getRootIndex())
    {
        this->root = x;
    }
    cout << "chegou no return " << endl;
    return true;
}
template<class T>
bool tree<T>::removeFromInternalPage(record<T> x, unsigned int index, unsigned long long int pageIndex)
{
    cout << "remove internal page " << endl;
    T key = x.getKey(index); //pego novamente a chave para não ter que passar por parametro
    record<T> right, left;

    left = readPage(x.getChildren(index)); //carrega o filho da esquerda
    right = readPage(x.getChildren(index + 1)); //filho da direita

    if( left.getLenght() > left.MIN) //tamanho maior que o minimo
    {
        x.setKey(index, left.getKey(left.getLenght() - 1)); //sobe o predescessor de x

        left.setLenght( left.getLenght() - 1 ); //diminui o filho a esquerda
        typedFile<T>::writeRecord(x, pageIndex); //escreve
        typedFile<T>::writeRecord(left, x.getChildren(index));

    }else if (right.getLenght() > right.MIN)
    {
        x.setKey(index, right.getKey(0)); //sobe o sucessor de x

        for(int i = 0; i < right.getLenght(); i++)  //reorganizar chaves
        {
            right.setKey(i, right.getKey(i + 1) );
        }
        right.setLenght(right.getLenght() - 1 ); //atualiza o tamanho
        // escreve no disco
        typedFile<T>::writeRecord(x, pageIndex);
        typedFile<T>::writeRecord(right, x.getChildren(index + 1));

    }else{
        //caso ambos tenham t-1 chaves

        merge(x,index, pageIndex); //reorganiza as paginas na fusao

        left = readPage(x.getChildren(index)); //atualiza a pag apos merge

        remove(key, x, pageIndex); //chama o remover de novo na pag unida
    }
    if(pageIndex == this->getRootIndex()) //se for remover da raiz
    {
        this->root = x; //atualiza raiz
    }
    return true;
}
template<class T>
bool tree<T>::downAndOrganize(T key, record<T> x, unsigned int index, unsigned long long int fatherIndex)
{
    cout << "downAndOrganize" << endl;
    //pagina interna, não folha
    record<T> right, left;

    left = readPage(x.getChildren(index));
    right = readPage(x.getChildren(index + 1));

    ///esquerda e direita com t-1
    if(left.getLenght() <= left.MIN && right.getLenght() <= right.MIN)
    {
        //fusao
        cout << " fusao " << endl;
        merge(x, index, fatherIndex);
        left = readPage(x.getChildren(index)); //atualiza a esquerda
        for(unsigned int j = 0; j < left.getLenght(); j++){//passando pelas chaves da direita
            cout << "chaves do left " << j << " :" << left.getKey(j).getValue() << endl;
        }
        remove(key, left,x.getChildren(index));
        return true;
    }
    ///esquerda com t-1, direita + que t
    if(left.getLenght() == left.MIN && right.getLenght() > right.MIN)
    {
        rotate(x, index, fatherIndex, true); //rotaciona
        left = readPage(x.getChildren(index)); //atualiza a esquerda
        remove(key, left, x.getChildren(index)); // chama o remover passando o filho rotacionado
        return true;
    }else{ /// esquerda tem t chaves
        //se esquerda tem t ou mais chaves
        cout << "chamando o remove pro filho com t chaves" << endl;
        cout << fatherIndex << endl;

        fatherIndex = x.getChildren(index); //fatherIndex atualiza
        remove(key, left, fatherIndex);
        return true;
    }
    ///direita com t-1, esquerda + que t
    if(left.getLenght() > left.MIN && right.getLenght() <= right.MIN)
    {
        rotate(x, index, fatherIndex, false); //rotaciona
        right = readPage(x.getChildren(index)); //atualiza a direita
        remove(key, right, x.getChildren(index)); // chama o remover passando o filho rotacionado
        return true;
    }
    else {/// caso direita com t chaves remove
        fatherIndex = x.getChildren(index); //fatherIndex atualiza
        remove(key, right, fatherIndex);
        return true;
    }

    return true;
}

template<class T>
bool tree<T>::merge(record<T>& father, unsigned int index, unsigned long long int fatherIndex)
{
    cout << "merge" << endl;
    record<T> right, left;

    left = readPage(father.getChildren(index));
    right = readPage(father.getChildren(index + 1));
    int rightIndex(father.getChildren(index + 1));

    //cout << "left key 0 " << left.getKey(0).getValue() << endl;
    //cout << "right key 0 " << right.getKey(0).getValue() << endl;

    //desce a chave do pai para ser a mediana do filho a esquerda
    left.setKey(left.getLenght(),father.getKey(index));
    left.setLenght(left.getLenght() + 1 ); //atualiza o tamanho

    //reorganiza chaves do pai
    for(unsigned int j = index; j < father.getLenght(); j++)
    {
        father.setKey(j, father.getKey(j + 1));
    }
    //reorganiza filhos do pai
    for(unsigned int j = index; j <= father.getLenght(); j++)
    {
        father.setChildren(j, father.getChildren(j + 1));
    }
    father.setLenght(father.getLenght() - 1); //atualiza o tamanho do pai
    //copia dos filhos da direita para esquerda
    for(unsigned int j = 0; j < right.getLenght(); j++){//passando pelas chaves da direita
        left.setKey(j + left.getLenght(), right.getKey(j));
    }
    //se não for folha copia os filhos da direita
    if(!left.isLeaf())
    {
        for(unsigned int j = 0; j < right.getLenght(); j++){//passando pelas chaves da direita
            left.setChildren(j + left.getLenght(), right.getChildren(j));
        }
    }
    //novo tamanho da esquerda
    left.setLenght(left.getLenght() + right.getLenght());

    /*
    //remove print
    cout << "father" << endl;
    for(unsigned int j = 0; j < father.getLenght(); j++){//passando pelas chaves da direita
            cout << father.getKey(j).getValue() << endl;
    }
    cout << "lenght " << father.getLenght() << endl;
    cout << "left" << endl;
    for(unsigned int j = 0; j < left.getLenght(); j++){//passando pelas chaves da direita
            cout << left.getKey(j).getValue() << endl;
    }
    cout << "lenght " << left.getLenght() << endl;
    */

    //escreve as mudancas no disco
    typedFile<T>::writeRecord(father, fatherIndex);
    typedFile<T>::writeRecord(left, father.getChildren(index));
    //o filho da direita ja nao tem como ser acessado pois nao tem mais referencia
    //mas é deletado no typedfile e atualiza a lista de invalidos para possivel reuso
    typedFile<T>::deleteRecord(right, rightIndex);

    if(fatherIndex == this->getRootIndex()) //se for remover da raiz
    {
        cout << "atualizou o root" << endl;
        this->root = father;
        return true;
    }

    return true;
}

template<class T>
bool tree<T>::rotate( record<T> father, unsigned long long int index, unsigned long long int fatherIndex, bool side)
{
    record<T> right, left;

    left = readPage(father.getChildren(index));
    right = readPage(father.getChildren(index + 1));

    if(side == true) //rotação esquerda(t-1) direita(t>min)
    {   //desce a chave para o filho esquerdo
        left.setKey(left.getLenght(), father.getKey(index));
        left.setLenght(left.getLenght() + 1); //aumenta o tamanho

        //caso nao seja folha deve copiar o filho da direita
        if(!left.isLeaf()){
            //filho da direita na posicao 0
            left.setChildren(left.getLenght(), right.getChildren(0));
        }
        //chave da direita sobe para o pai
        father.setKey(index, right.getKey(0));
        //reorganizar chaves na pagina direita
        for(int i = 0; i < right.getLenght(); i++)
        {
            right.setKey(i, right.getKey(i + 1));
        }
        right.setLenght( right.getLenght() - 1 ); //diminui o tamanho da direita
    }
    else  //rotação direita(t-1) esquerda(t>min)
    {
        //reorganizar chaves da pagina da direita pra receber a chave da esquerda
        for(int i = right.getLenght(); i >= 1; i--) //TODO test for
        {
            right.setKey(i, right.getKey(i - 1));
        }
        right.setKey(0, father.getKey(index)); //coloco a chave do pai na pos 0
        right.setLenght(right.getLenght() + 1); // atualizo o tamanho

        //se nao for folha copia o filho da chave na pag esq
        if(!left.isLeaf()){
            right.setChildren(right.getLenght(), left.getChildren(left.getLenght()));
        }

        father.setKey(index, left.getKey(left.getLenght()-1)); //pai recebe a chave da esquerda
        left.setLenght(left.getLenght() - 1 ); //esquerda diminui de tamanho
    }

    //escreve as mudancas no disco
    typedFile<T>::writeRecord(father, fatherIndex);
    typedFile<T>::writeRecord(left, father.getChildren(index));
    typedFile<T>::writeRecord(right, father.getChildren(index+1));

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
