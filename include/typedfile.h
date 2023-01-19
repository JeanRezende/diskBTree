#ifndef TYPEDFILE_H
#define TYPEDFILE_H

#include <fstream>
#include <type_traits>
#include <header.h>
using namespace std;

const ios::openmode mode = ios::in | ios::out | ios::binary;

template <class T>
class typedFile : private fstream
{
    static_assert(is_base_of<serializable, T>::value, "T must be serializable");
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
    unsigned long long int search(T data);
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
    if(this->open(name,type,ver,mode))
    {
        //cout << "arquivo aberto" << endl;
        cerr << "arquivo aberto" << endl;
    }
    else
    {
        cout << "erro ao abrir o arquivo" << endl;
    }
}

template <class T>
typedFile<T>::~typedFile(){}

template <class T>
bool typedFile<T>::open(const string name, const string type, const unsigned int ver, ios::openmode openmode)
{
    fstream::open(name.c_str(), openmode); //tenta abrir
    //fstream::open(name.c_str(), openmode | ios::trunc);
    if(!isOpen())
    {
        // caso tenha erro ao abrir
        fstream::open(name.c_str(), ios::out);
        this->close();
        fstream::open(name.c_str(), openmode);

        if(isOpen())
        {
            //cout << "---- arquivo criado ----" << endl;

            //inicializa o cabe�alho e escreve
            this->head = header(type, ver); //cria novo objeto para o cabecalho
            //cout << head.getFirstValid() << head.getFirstDeleted() << head.getType() <<head.getVersion() << endl;
            this->writeHeader(this->head); //necessario ja que no if nao escreve

            record<T> root;
            insertRecord(root); //inicializa a raiz

            if(writeHeader(head)) //escreve no arquivo
            {
                //cout << "-- header escrito --" << endl;
                return true;
            }
            else
            {
                //cout << "-- falha ao escrever o header --" << endl;
                return false;
            }
        }
        else
        {
            //cout << "---- falha ao criar o arquivo ----" << endl;
            return false;
        }
    }
    else
    {
        //cout << "+++ arquivo aberto +++" << endl;

        if(readHeader(head))
        {
            //cout << "+++ header lido +++" << endl;

            //cout << "tipo : " << this->head.getType()<< endl;
            //cout << "versao : " << this->head.getVersion()<< endl;

            if(this->head.getType() == type && this->head.getVersion() == ver)
            {
                //cout << "+++ header compativel +++" << endl;
                return true;
            }
            else
            {
                //cout << "+++ erro no header +++" << endl;
                this->close();
                return false;
            }
        }
    }
}

template <class T>
bool typedFile<T>::isOpen()
{
    return fstream::is_open();
}

template <class T>
bool typedFile<T>::close()
{
    fstream::close();
    return true;
}

template <class T>
void typedFile<T>::clear()
{
    fstream::clear();
}

template <class T>
bool typedFile<T>::readRecord(record<T> &r, unsigned long long int i)
{
    if(isOpen())
    {
        this->clear();
        char *aux = new char[r.size()]; //apontador de char pra receber a leitura

        fstream::seekg(index2pos(i), ios::beg); //cabeca de leitura na pos ref ao index
        fstream::read(aux, r.size()); //l�

        r.fromString(string(aux,r.size())); //armazena em r

        delete aux;
        return fstream::good();
    }
    else
    {
        cout << " erro de leitura " << endl;
        return false;
    }
}

template <class T>
bool typedFile<T>::writeRecord(record<T> &r, unsigned long long int i)
{
    if(isOpen())
    {
        this->clear();
        fstream::seekp(this->index2pos(i), ios::beg); //cabeca de escrita na pos ref ao index
        fstream::write(r.toString().c_str(), r.size()); //escreve
        //cout << "record inserido" << endl;
        return fstream::good();
    }
    else
    {
        cout << "erro de escrita" << endl;
        return false;
    }
}

template <class T>
bool typedFile<T>::insertRecord(record<T> &r) //utilizado apenas para raiz
{
    //se tem registros deletados
    //atualiza o primeiro valido
    if(this->head.getFirstDeleted() != 0 ) //lista de deletados nao esta vazia
    {
        //cout << "lista de deletados tem posicao para reciclar" << endl;
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

template <class T>
bool typedFile<T>::setNewRoot(unsigned long long int i) //função utilizada para atualizar a lista de validos
{ //é necessaria quando há uma fusão que o merge troca a raiz
    this->head.setFirstValid(i);
    this->writeHeader(this->head);
    return true;
}

template <class T>
bool typedFile<T>::deleteRecord(record<T> &r, unsigned long long int i)
{
    //coloca o registro na cabeca dos deletados e atualiza o cabecalho
    r.setNext(this->getFirstDeleted());
    head.setFirstDeleted(i);
    this->writeHeader(this->head);

    return true;
}

template <class T>
unsigned long long int typedFile<T>::getFirstValid()
{
    return this->head.getFirstValid();
}

template <class T>
unsigned long long int typedFile<T>::getFirstDeleted()
{
    return this->head.getFirstDeleted();
}

template <class T>
unsigned long long int typedFile<T>::search(T data)
{
    unsigned long long int i = this->getFirstValid(); //index do primeiro valido
    record<T> aux;

    while(fstream::good() && i != 0)
    {
        this->readRecord(aux,i);

//        if(data == aux.getData()){return i;}
        i = aux.getNext();
    }
    return 0;
}

template <class T>
bool typedFile<T>::readHeader(header &h)
{
    if(isOpen())
    {
        char *aux = new char[h.size()];
        this->clear(); //totalmente necessario para funcionar
        //leitura e posicionamento do fstream
        fstream::seekg(0, ios::beg);
        fstream::read(aux, h.size());
        //utilizando a fun��o fromString para atualizar o header
        h.fromString(string(aux, h.size()));

        delete aux; //desaloca��o de ponteiro
        return true;
    }
    return false;
}

template <class T>
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

template <class T>
unsigned long long int typedFile<T>::index2pos(unsigned long long int i)
{
    //TODO tests
    record<T> aux;
    return this->head.size() + ((i - 1) * aux.size());

}

template <class T>
unsigned long long int typedFile<T>::pos2index(unsigned long long int p)
{
    record<T> aux;
    return ((p - this ->head.size()) / aux.size()) + 1;
}

template <class T>
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
