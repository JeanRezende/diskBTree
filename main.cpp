#include <iostream>
#include <clocale>
#include "header.h"
#include "record.h"
#include "typedfile.h"
#include "tree.h"
#include "serialCurrency.h"

using namespace std;

int main()
{
    //setlocale(LC_ALL, "Portuguese");
    setlocale(LC_ALL, "C"); //usando o internacional o double printa com o . entre os decimais
    int option, i;
    double n;
    double insercao[21] = {5.50, 2.01, -5.33, 10.05, 21.13, 1.88, -3.14, 2.86, 37.55, 2.99, 34.31, 49.00, 38.26, 20.85, 23.02, 17.01, 14.92, 13.81, 39.89, -13.22, 7.20};

    tree<serialCurrency> diskTree("diskTree.dat", "VOA", 2);
    serialCurrency r;

    if (diskTree.isOpen())
    {
        do
        {
            cout << "Selecione uma opcao:\n\n"
                 << "1. Inserir um registro\n"
                 << "2. Excluir um registro\n"
                 << "3. Pesquisar um registro\n"
                 << "4. Listar todos os registros\n"
                 << "5. Teste automatizado insercao\n"
                 << "6. Sair\n\n"
                 << "Sua escolha: ";
            cin >> option;
            switch (option)
            {
            case 1:
                cout << "Digite um numero com 2 casas decimais separados por ponto: ";
                cin >> n;

                r = serialCurrency(n);

                if (diskTree.insert(r))
                {
                    cout << "Valor " << n << " inserido com sucesso.\n" << endl;
                    diskTree.print();
                }
                else
                {
                    cout << "nao foi possivel inserir" << endl;
                }
                break;
            case 2:
                cout << "Digite valor deseja remover do arquivo: ";
                cin >> n;
                i = diskTree.search(serialCurrency(n));
                if (i != 0)
                {
                    if (diskTree.remove(i))
                        cout << "Valor " << n << " removido do arquivo.\n" << endl;
                }
                else
                {
                    cout << "Valor " << n << " não encontrado no arquivo\n" << endl;
                }
                break;
            case 3:
                cout << "Digite o valor a ser pesquisado: ";
                cin >> n;
                i = diskTree.search(serialCurrency(n));

                if (i != 0)
                    cout << "Valor " << n << " encontrado no registro " << i << ".\n" << endl;
                else
                    cout << "Valor " << n << " não encontrado.\n" << endl;

                break;
            case 4:
                cout << "Listando todos os registros validos do arquivo: " << endl;
                diskTree.print();
                break;
            case 5:
                cout << "teste de inserção ";
                for(int i = 0; i < 21; i++)
                {
                    r.setValue(insercao[i]);
                    if(diskTree.insert(r))
                    {
                        diskTree.print();
                    }
                    cout << "------------------------------" << endl;
                }
                break;
            case 6:
                cout << "Encerrando o programa... ";
                diskTree.close();
                cout << "concluido." << endl;
                return 0;
                break;
            default:
                cout << "Opção invalida! Tente novamente..." << endl;
                break;
            }
        }
        while (true);
    }
    else
    {
        cout << "Não foi possivel abrir o arquivo!" << endl;
        return 1;
    }
    return 0;
}
