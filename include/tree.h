//==================================================================//
/// Arvore B em disco para a disciplina de Tecnicas de Programacao ///
/// IFMG Bambui - Engenharia de Computacao 2023                    ///
/// Alunos:                                                        ///
/// 0034077 - Felipe de Freitas Silva                              ///
/// 0036703 - Jean Gustavo Ferreira Rezende                        ///
//==================================================================//

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

    record<T> getRoot();
    void setRoot(record<T> root);
    record<T> readPage(unsigned long long int i);
    unsigned long long getRootIndex();
    bool isOpen();
    bool close();
protected:
    void printAux(record<T> x, vector<string> &v, unsigned int lvl);

    bool insertNotFull(record<T>& x, T key, unsigned int i);
    bool split(record<T>& x, unsigned int index, unsigned int indexofX);

    bool rotate(record<T>& father, unsigned long long int index, unsigned long long int fatherIndex, bool side);
    bool removeFromLeaf(record<T>& x, unsigned int index, unsigned long long int pageIndex);
    bool removeFromInternalPage(record<T>& x, unsigned int index, unsigned long long int pageIndex);
    bool downAndOrganize(T key, record<T> x, unsigned int index, unsigned long long int fatherIndex);
    bool merge(record<T>& father, unsigned int index, unsigned long long int fatherIndex);

    T findPred(record<T> page, unsigned int keyIndex);
    T findSuc (record<T> page, unsigned int keyIndex);
private:
    record<T> root;
};

template<class T>
tree<T>::tree() //função para criar uma nova arvore começando pela raiz
{
    unsigned long long int rootIndex = typedFile<T>::getFirstValid();
    this->setRoot(this->readPage(rootIndex));
}

template<class T> //função para criar uma nova arvore no arquivo e trazer ela para memoria
tree<T>::tree(const string name, const string type, const unsigned int version):typedFile<T>::typedFile(name,type,version)
{
    unsigned long long int rootIndex = typedFile<T>::getFirstValid();
    this->setRoot(this->readPage(rootIndex));
}

template<class T> //função para fechar
bool tree<T>::close()
{
    return typedFile<T>::close();
}

template<class T> //função que retorna o endereço do primeiro registro valido no arquivo
unsigned long long tree<T>::getRootIndex()
{
    return typedFile<T>::getFirstValid();
}

template<class T> //dtor
tree<T>::~tree() {}

template<class T> //função que lê uma pagina(nó) na memoria secundária e retorna um record<T>
record<T> tree<T>::readPage(unsigned long long int i)
{
    record<T> aux;
    typedFile<T>::readRecord(aux, i);
    return aux;
}

template<class T> //função de inserção na arvore, recebe uma chave do tipo T e descobre onde será feita a inserçaõ
bool tree<T>::insert(T key)
{
    record<T> page = this->getRoot(); //crio ponteiros auxiliares

    unsigned long long int rootIndex = getRootIndex(); //index da raiz
    unsigned int i = 0;

    if(page.getLenght() == page.MAX) //se chegou no tamanho maximo
    {
        record<T> newRoot, aux;
        newRoot.setLeaf(false); //nao folha

        newRoot.setChildren(0, rootIndex); //aponta para a raiz antiga

        typedFile<T>::insertRecord(newRoot); // insere a nova raiz

        rootIndex = getRootIndex(); //atualiza o root index

        split(newRoot, 0, rootIndex); //chama o split para dividir o filho com root index antigo

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
        insertNotFull(page, key, rootIndex); //insere

        this->setRoot(page);

        return true;
    }
    return false;
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
    record<T> aux;
    int j = 0;

    while (j < x.getLenght() && x.getKey(j).getValue() < key.getValue())
    {
        j++;
    }

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
    record<T> left, right;

    left = this->readPage(x.getChildren(index)); //pag antiga cheia
    right.setLeaf(left.isLeaf()); // se pag antiga for folha a direita tbm sera

    //copia para right a quantidade de chaves MIN, de MIN até MAX
    for(int j = 0; j < right.MIN; j++)
    {
        right.setKey(j, left.getKey(j + right.MIN + 1)); //set pos 0 ate MIN
    }

    if(!right.isLeaf()) //se nao for folha tem que copiar os filhos
    {
        for(int j = 0; j <= right.MIN; j++) //de 0 ate MIN+1
        {
            right.setChildren(j, left.getChildren(j+ right.MIN + 1));  //setchildren pos 0 ate MIN + 1
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

    this->setRoot(x); //atualiza a raiz
    //typedFile<T>::setNewRoot(indexofX); //atualiza a lista de validos

    //escreve
    typedFile<T>::writeRecord(x, indexofX);
    typedFile<T>::writeRecord(left, x.getChildren(index));
    typedFile<T>::writeRecord(right, pos);

    return true;
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

    if(key.getValue() == x.getKey(index).getValue())
    {
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
    {// vazio
        return false;
    }
    unsigned int index = 0;
    //busca a chave na pagina e soma no index
    while( x.getLenght() > index && key > x.getKey(index))
    {
        index++;
    }
    //testa na posição do index se chegou no valor desejado, e se é folha
    //remocao caso 1
    if(key == x.getKey(index) && x.isLeaf())
    {
        return this->removeFromLeaf(x, index, pageIndex);
    }
    //caso seja pagina interna
    if(!x.isLeaf())
    {
        unsigned long long int fatherIndex = this->getRootIndex();

        if(key == x.getKey(index))//achou o valor na pagina interna
        {
            return this->removeFromInternalPage(x, index, pageIndex);
        }
        //função que vai reorganizar as paginas chamando rotacao e fusao e remocao
        downAndOrganize(key, x, index, fatherIndex);
    }

    return true;
}
template<class T>
bool tree<T>::removeFromLeaf(record<T>& x, unsigned int index, unsigned long long int pageIndex)
{
    //reorganiza chaves, sobreescrevendo o valor removido
    for(int i = index; i < x.getLenght() - 1; i++ )
    {
        x.setKey(i, x.getKey(i+1)); //set key recebe o proximo
        //como e folha não precisa reorganizar filhos
    }
    x.setLenght(x.getLenght() - 1); //diminui 1 do tamanho da pag

    typedFile<T>::writeRecord(x, pageIndex); //escreve

    //caso for removido da raiz deve atualizar o root
    if(pageIndex == this->getRootIndex())
    {
        setRoot(x);
    }
    return true;
}
template<class T>
bool tree<T>::removeFromInternalPage(record<T>& x, unsigned int index, unsigned long long int pageIndex)
{
    //remove from internal page é que a chave foi encontrada na pag interna
    T key = x.getKey(index); //pego novamente a chave para não ter que passar por parametro
    record<T> right, left;

    left = readPage(x.getChildren(index)); //carrega o filho da esquerda
    right = readPage(x.getChildren(index + 1)); //filho da direita

    ///PREDESCESSOR/SUCESSOR
    if( left.getLenght() > left.MIN)
    {
        T predec = findPred(x, index);
        x.setKey(index, predec);

        //escreve as mudancas no disco
        typedFile<T>::writeRecord(x, pageIndex);

    } //caso left tenha t-1 chaves e right tenha t ou mais chaves
    else if(right.getLenght() > right.MIN)
    {
        T successor = findSuc(x, index);

        x.setKey(index, successor);

        //escreve as mudancas no disco
        typedFile<T>::writeRecord(x, pageIndex);
    }
    ///MERGE
    //caso ambos tenham t-1 chaves
    if(left.getLenght() == left.MIN && right.getLenght() == right.MIN)
    {
        merge(x,index, pageIndex); //reorganiza as paginas na fusao

        left = readPage(x.getChildren(index)); //atualiza a pag apos merge

        remove(key, left, x.getChildren(index)); //chama o remover de novo na pag unida
    }

    if(pageIndex == this->getRootIndex()) //se for remover da raiz
    {
        setRoot(x); //atualiza raiz
    }
    return true;
}
template<class T> //função 2 A
T tree<T>::findPred(record<T> page, unsigned int keyIndex)
{
    record<T> right, left;

    //antes do while deve descer para o filho da esquerda
    unsigned long long int pageIndex = page.getChildren(keyIndex);
    page = readPage(page.getChildren(keyIndex)); //atualiza a esquerda

    //O left tem t chaves
    while(!page.isLeaf())
    {
        left = readPage(page.getChildren(keyIndex));
        right = readPage(page.getChildren(keyIndex + 1));

        ///MERGE
        ///esquerda e direita com t-1
        if(left.getLenght() == left.MIN && right.getLenght() == right.MIN)
        {
            //fusao
            merge(page, keyIndex, pageIndex);
            //atualiza o page
            pageIndex = page.getChildren(keyIndex);
            page = readPage(page.getChildren(keyIndex)); //atualiza a esquerda
        }
        ///se right > MIN
        if(right.getLenght() > right.MIN)
        {
            pageIndex = page.getChildren(keyIndex + 1);
            page = readPage(page.getChildren(keyIndex + 1));
        }
        ///ROTACAO
        //rotaciona da esquerda para direita
        if(left.getLenght() > left.MIN && right.getLenght() == right.MIN)
        {
            rotate(page, keyIndex, pageIndex, false); //rotaciona
            pageIndex = page.getChildren(keyIndex + 1);
            page = readPage(page.getChildren(keyIndex + 1)); //atualiza a esquerda
        }
    }
    T key = page.getKey(page.getLenght() - 1);
    page.setLenght(page.getLenght() - 1); //diminui 1 do tamanho da pag
    typedFile<T>::writeRecord(page, pageIndex); //escreve

    return key;
}
template<class T>
T tree<T>::findSuc(record<T> page, unsigned int keyIndex)
{
    record<T> right, left;

    //antes do while deve descer para o filho da direita
    unsigned long long int pageIndex = page.getChildren(keyIndex + 1);
    page = readPage(page.getChildren(keyIndex + 1)); //atualiza a direita

    //O right tem t chaves
    while(!page.isLeaf())
    {
        left = readPage(page.getChildren(keyIndex));
        right = readPage(page.getChildren(keyIndex + 1));

        ///MERGE
        ///esquerda e direita com t-1
        if(left.getLenght() == left.MIN && right.getLenght() == right.MIN)
        {
            //fusao
            merge(page, keyIndex, pageIndex);
            //atualiza o page
            pageIndex = page.getChildren(keyIndex);
            page = readPage(page.getChildren(keyIndex)); //atualiza a esquerda
        }
        ///se left > MIN
        if(left.getLenght() > left.MIN)
        {
            pageIndex = page.getChildren(keyIndex);
            page = readPage(page.getChildren(keyIndex));
        }
        ///ROTACAO
        //rotaciona da direita para esquerda
        if(right.getLenght() > right.MIN && left.getLenght() == left.MIN)
        {
            rotate(page, keyIndex, pageIndex, true); //rotaciona
            pageIndex = page.getChildren(keyIndex);
            page = readPage(page.getChildren(keyIndex)); //atualiza a esquerda
        }
    }
    T key = page.getKey(0);
    for(int i = 0; i < page.getLenght(); i++) //reorganiza as chaves
    {
        page.setKey(i, page.getKey(i + 1));
    }
    page.setLenght(page.getLenght() - 1); //diminui 1 do tamanho da pag
    typedFile<T>::writeRecord(page, pageIndex); //escreve

    return key;
}

template<class T>
bool tree<T>::downAndOrganize(T key, record<T> x, unsigned int index, unsigned long long int fatherIndex)
{
    //pagina interna, não folha
    record<T> right, left;

    left = readPage(x.getChildren(index));
    right = readPage(x.getChildren(index + 1));
    ///MERGE
    ///esquerda e direita com t-1
    if(left.getLenght() == left.MIN && right.getLenght() == right.MIN)
    {//fusao
        merge(x, index, fatherIndex);
        left = readPage(x.getChildren(index)); //atualiza a esquerda
        remove(key, left,x.getChildren(index));
        return true;
    }
    ///ROTACAO OU REMOCAO
    //se a maior chave da esquerda for maior ou igual a chave deve descer para esquerda
    if( left.getKey(left.getLenght() - 1 ) >= key)
    {///esquerda com t-1, direita + que t
        if(left.getLenght() == left.MIN && right.getLenght() > right.MIN)
        {
            rotate(x, index, fatherIndex, true); //rotaciona
            left = readPage(x.getChildren(index)); //atualiza a esquerda
        }
        return remove(key, left, x.getChildren(index)); // chama o remover passando o filho rotacionado
    }
    else
    {///direita com t-1, esquerda + que t
        if(left.getLenght() > left.MIN && right.getLenght() == right.MIN)
        {
            rotate(x, index, fatherIndex, false); //rotaciona
            right = readPage(x.getChildren(index + 1)); //atualiza a direita
        }
        remove(key, right, x.getChildren(index + 1)); // chama o remover passando o filho rotacionado
        return true;
    }
    return false;
}

template<class T>
bool tree<T>::merge(record<T>& father, unsigned int index, unsigned long long int fatherIndex)
{
    record<T> right, left;

    left = readPage(father.getChildren(index));
    right = readPage(father.getChildren(index + 1));
    unsigned long long int rightIndex(father.getChildren(index + 1));

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
    for(unsigned int j = 0; j < right.getLenght(); j++) //passando pelas chaves da direita
    {
        left.setKey(j + left.getLenght(), right.getKey(j));
    }
    //se não for folha copia os filhos da direita
    if(!left.isLeaf())
    {
        for(unsigned int j = 0; j <= right.getLenght(); j++) //passando pelas chaves da direita
        {
            left.setChildren(j + left.getLenght(), right.getChildren(j));
        }
    }
    //novo tamanho da esquerda
    left.setLenght(left.getLenght() + right.getLenght());

    //escreve as mudancas no disco
    typedFile<T>::writeRecord(father, fatherIndex);
    typedFile<T>::writeRecord(left, father.getChildren(index));
    //o filho da direita ja nao tem como ser acessado pois nao tem mais referencia
    //mas é deletado no typedfile e atualiza a lista de invalidos para possivel reuso
    typedFile<T>::deleteRecord(right, rightIndex);

    if(fatherIndex == this->getRootIndex()) //se for remover da raiz
    {
        this->setRoot(father); //atualiza a raiz

        if( father.getLenght() == 0 ) //se raiz estiver vazia
        {
            this->setRoot(left); //raiz fez o merge no left entao atualiza
            typedFile<T>::setNewRoot(father.getChildren(index)); //atuliza o primeiro valido
            typedFile<T>::deleteRecord(father, fatherIndex); //deleta o antigo father
        }
    }

    return true;
}

template<class T>
bool tree<T>::rotate( record<T>& father, unsigned long long int index, unsigned long long int fatherIndex, bool side)
{
    record<T> right, left;

    left = readPage(father.getChildren(index));
    right = readPage(father.getChildren(index + 1));

    if(side == true) //rotação esquerda(t-1) direita(t>min) rotacao da direita para esq
    {
        left.setKey(left.getLenght(), father.getKey(index));
        left.setLenght(left.getLenght() + 1);

        father.setKey(index, right.getKey(0));

        //reorganizar chaves da pagina da direita
        for(int i = 0; i < right.getLenght() - 1; i ++)
        {
            right.setKey(i, right.getKey(i + 1));
        }
        if(!right.isLeaf()) //os dois nao sao folha
        {
            //realoca filhos do right
            left.setChildren(left.getLenght()-1, right.getChildren(0));
            for(int i = 0; i < right.getLenght(); i ++)
            {
                right.setChildren(i, right.getChildren(i + 1));
            }
        }
        right.setLenght(right.getLenght() - 1); //dimunui em 1 o tamanho do right
    }
    else  //rotação direita(t-1) esquerda(t>min) rotacao da esquerda para direita
    { //reorganizar chaves da pagina da direita
        for(int i = right.getLenght(); i > 0; i --)
        {
            right.setKey(i, right.getKey(i - 1));
        }
        right.setKey(0, father.getKey(index)); //puxando a chave do pai para ser o inicio do right

        father.setKey(index, left.getKey(left.getLenght() - 1)); // puxando a chave do left para o father

        if(!left.isLeaf()) //os dois não sao folha
        {
            //realoca filhos do right
            for(int i = right.getLenght() + 1; i > 0; i --)
            {
                right.setChildren(i, right.getChildren(i - 1));
            }
            right.setChildren(0, left.getChildren(left.getLenght())); //copia o ultimo filho do left
        }

        left.setLenght(left.getLenght() - 1); //atualiza o tamanho do filho a esquerda
        right.setLenght(right.getLenght() + 1); //atualiza o tamanho do filho a direita
    }

    //escreve as mudancas no disco
    typedFile<T>::writeRecord(father, fatherIndex);
    typedFile<T>::writeRecord(left, father.getChildren(index));
    typedFile<T>::writeRecord(right, father.getChildren(index+1));

    if(fatherIndex == this->getRootIndex()) //se for remover da raiz
    {
        setRoot(father);
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
