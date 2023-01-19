//==================================================================//
/// Arvore B em disco para a disciplina de Tecnicas de Programacao ///
/// IFMG Bambui - Engenharia de Computacao 2023                    ///
/// Alunos:                                                        ///
/// 0034077 - Felipe de Freitas Silva                              ///
/// 0036703 - Jean Gustavo Ferreira Rezende                        ///
//==================================================================//

#ifndef TYPEDFILE_H
#define TYPEDFILE_H

#include <fstream>
#include <type_traits>
#include <header.h>

const ios::openmode mode = ios::in | ios::out | ios::binary;

template <class T>
class typedFile : private fstream
{
public:
    typedFile();
    typedFile(const string name, const string type, const unsigned int version, ios::openmode openmode = mode);
    virtual ~typedFile();
    bool open(const string name, const string type, const unsigned int version, ios::openmode openmode = mode);
    bool isOpen();
    bool close();
    void clear();
    bool readRecord(record<T> &r, unsigned long long int i);
    bool writeRecord(record<T> &r, unsigned long long int i);
    bool insertRecord(record<T> &r);
    bool deleteRecord(record<T> &r, unsigned long long int i);
    unsigned long long int getFirstValid();
    unsigned long long int getFirstDeleted();
    unsigned long long int nextWriteIndex();
protected:
    header head;
    bool readHeader(header &h);
    bool writeHeader(header &h);
    unsigned long long int index2pos(unsigned long long int i);
    unsigned long long int pos2index(unsigned long long int p);
    bool setNewRoot(unsigned long long int i);
};

template <class T>
typedFile<T>::typedFile() : fstream()
{}

template <class T>
typedFile<T>::typedFile(const string name, const string type, const unsigned int ver, ios::openmode openmode) : fstream(name.c_str(), mode)
{
    if(this->open(name,type,ver,mode)) {}
    else {}
}

template <class T>
typedFile<T>::~typedFile() {}

template <class T> //funcao que abre e cria o arquivo em memoria
bool typedFile<T>::open(const string name, const string type, const unsigned int ver, ios::openmode openmode)
{
    fstream::open(name.c_str(), openmode); //tenta abrir
    if(!isOpen())
    {
        // caso tenha erro ao abrir
        fstream::open(name.c_str(), ios::out);
        this->close();
        fstream::open(name.c_str(), openmode);

        if(isOpen()) //se o arquivo for aberto e criado
        {
            //inicializa o cabecalho e escreve
            this->head = header(type, ver); //cria novo objeto para o cabecalho
            this->writeHeader(this->head); //necessario ja que no if nao escreve

            record<T> root;
            insertRecord(root); //inicializa a raiz

            if(writeHeader(head)) //escreve no arquivo
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        if(readHeader(head))
        {
            if(this->head.getType() == type && this->head.getVersion() == ver)
            {
                return true;
            }
            else
            {
                this->close();
                return false;
            }
        }
    }
}

template <class T> //funcao que testa se o arquivo esta aberto retornando um boolean
bool typedFile<T>::isOpen()
{
    return fstream::is_open();
}

template <class T> //funcao para fechar o arquivo
bool typedFile<T>::close()
{
    fstream::close();
    return true;
}

template <class T> //funcao para limpar o buffer do fstream
void typedFile<T>::clear() //muito importante usar no windows
{
    fstream::clear();
}

template <class T> //funcao para ler o record com base na posicao do arquivo
bool typedFile<T>::readRecord(record<T> &r, unsigned long long int i)
{
    if(isOpen())
    {
        this->clear();
        char *aux = new char[r.size()]; //apontador de char pra receber a leitura

        fstream::seekg(index2pos(i), ios::beg); //cabeca de leitura na pos ref ao index
        fstream::read(aux, r.size());

        r.fromString(string(aux,r.size())); //armazena em r

        delete aux;
        return fstream::good();
    }
    else
    {
        //cerr << " erro de leitura " << endl;
        return false;
    }
}

template <class T> //funcao para escrever o registro em alguma posicao
bool typedFile<T>::writeRecord(record<T> &r, unsigned long long int i)
{
    if(isOpen())
    {
        this->clear();
        fstream::seekp(this->index2pos(i), ios::beg); //cabeca de escrita na pos ref ao index
        fstream::write(r.toString().c_str(), r.size()); //escreve
        return fstream::good();
    }
    else
    {
        //cerr << "erro de escrita" << endl;
        return false;
    }
}

template <class T> //funcao que insere a primeira raiz
bool typedFile<T>::insertRecord(record<T> &r) //utilizado apenas para raiz
{
    //se tem registros deletados
    //atualiza o primeiro valido
    if(this->head.getFirstDeleted() != 0 ) //lista de deletados nao esta vazia
    {
        unsigned long long int i = this->head.getFirstDeleted();
        record<T> aux;

        if(this->readRecord(aux,i)) //le o primeiro deletado e copia, verdade se funcinar
        {
            this->head.setFirstDeleted(aux.getNext()); //cabeca de deletados agora e o proximo
            aux = r; //coloca o record no aux
            aux.setNext(this->head.getFirstValid());
            this->head.setFirstValid(i);

            this->writeRecord(aux, i);
            this->writeHeader(this->head);
            //cout << "inserido" << endl;
            return true;
        }
        return false;
    }
    else
    {
        //se nao tem registros deletados
        unsigned long long int pos;
        seekp(0,ios::end);
        pos = fstream::tellp();
        r.setNext(this->head.getFirstValid());
        this->head.setFirstValid(this->pos2index(pos));

        this->writeRecord(r, this->pos2index(pos));
        this->writeHeader(this->head);

        return true;
    }
    return false;
}

template <class T> //funcao que atualiza o endereco do raiz caso atualize
bool typedFile<T>::setNewRoot(unsigned long long int i) //função utilizada para atualizar a lista de validos
{
    //é necessaria quando há uma fusão que o merge troca a raiz
    this->head.setFirstValid(i);
    this->writeHeader(this->head);
    return true;
}

template <class T> //deleta um record e coloca na lista de deletados
bool typedFile<T>::deleteRecord(record<T> &r, unsigned long long int i)
{
    //coloca o registro na cabeca dos deletados e atualiza o cabecalho
    r.setNext(this->getFirstDeleted());
    head.setFirstDeleted(i);
    this->writeHeader(this->head);
    return true;
}

template <class T> //funcao que retorna o primeiro valido(raiz da arvore)
unsigned long long int typedFile<T>::getFirstValid()
{
    return this->head.getFirstValid();
}

template <class T> //funcao que retorna o primeiro deletado
unsigned long long int typedFile<T>::getFirstDeleted()
{
    return this->head.getFirstDeleted();
}

template <class T> //funcao que lê o cabecalho
bool typedFile<T>::readHeader(header &h)
{
    if(isOpen())
    {
        char *aux = new char[h.size()];
        this->clear(); //totalmente necessario para funcionar
        //leitura e posicionamento do fstream
        fstream::seekg(0, ios::beg);
        fstream::read(aux, h.size());
        //utilizando a funcao fromString para atualizar o header
        h.fromString(string(aux, h.size()));

        delete aux; //desalocacao de ponteiro
        return true;
    }
    return false;
}

template <class T> //funcao que escreve o cabecalho
bool typedFile<T>::writeHeader(header &h)
{
    if (isOpen())
    {
        this->clear();
        fstream::seekp(0, ios::beg);
        fstream::write(h.toString().c_str(), h.size());
        return true;
    }
    return false;
}

template <class T> //funcao que converte o index para uma posicao no arquivo
unsigned long long int typedFile<T>::index2pos(unsigned long long int i)
{
    record<T> aux;
    return this->head.size() + ((i - 1) * aux.size());
}

template <class T> //funcao que converte uma posicao para o index do registro
unsigned long long int typedFile<T>::pos2index(unsigned long long int p)
{
    record<T> aux;
    return ((p - this ->head.size()) / aux.size()) + 1;
}

template <class T> //funcao que retorna uma posicao para escrever o proximo registro
unsigned long long int typedFile<T>::nextWriteIndex()
{
    if(this->head.getFirstDeleted() != 0)
    {
        return this->head.getFirstDeleted();
    }
    else
    {
        unsigned long long int pos;
        seekp(0,ios::end);
        pos = fstream::tellp();
        return pos2index(pos);
    }
}

#endif // TYPEDFILE_H
