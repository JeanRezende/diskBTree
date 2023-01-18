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
    vector<double> insercao = {5.50, 2.01, -5.33, 10.05, 21.13, 1.88, -3.14, 2.86, 37.55, 2.99, 34.31, 49.00, 38.26, 20.85, 23.02, 17.01, 14.92, 13.81, 39.89, -13.22, 7.20, 52.28, 78.32, 29.20, 64.03, 3.10, 4.50, 27.87, 90.99, 72.63, 55.44, 33.22, 22.11, 11.00, 66.55, 77.66, 88.77, 99.88, 2.45, 2.56, 2.67, 2.78, 2.89, 10.11, 10.23, 10.45, 10.56, 10.78, 10.89, 10.60, 30.11, 30.22, 30.33, 30.44, 30.55, 30.66, 30.77, 30.88, 30.99, 20.10, 20.22, 20.33, 20.44, 20.55, 20.66, 20.77, 20.88, 20.99, 70.10, 70.20, 70.30, 70.40, 70.50, 70.60, 70.70, 90.00, 90.11, 90.22, 90.33, 90.44, 90.55, 90.66, 90.99 };
    vector<double> remocao = {5.50, 2.01, -5.33, 10.05, 21.13, 1.88, -3.14, 2.86, 37.55, 2.99, 34.31, 49.00, 38.26, 20.85, 23.02, 17.01, 14.92, 13.81, 39.89, -13.22, 7.20, 52.28, 78.32, 29.20, 64.03, 3.10, 4.50, 27.87, 90.99, 72.63, 55.44, 33.22, 22.11, 11.00, 66.55, 77.66, 88.77, 99.88, 2.45, 2.56, 2.67, 2.78, 2.89, 10.11, 10.23, 10.45, 10.56, 10.78, 10.89, 10.60, 30.11, 30.22, 30.33, 30.44, 30.55, 30.66, 30.77, 30.88, 30.99, 20.10, 20.22, 20.33, 20.44, 20.55, 20.66, 20.77, 20.88, 20.99, 70.10, 70.20, 70.30, 70.40, 70.50, 70.60, 70.70, 90.00, 90.11, 90.22, 90.33, 90.44, 90.55, 90.66, 90.99 };

    //vector<double> insercao = {5.50, 2.01, -5.33, 10.05, 21.13, 1.88, -3.14, 2.86, 37.55, 2.99, 34.31, 49.00, 38.26, 20.85, 23.02, 17.01, 14.92, 13.81, 39.89, -13.22, 7.20};
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

                r = serialCurrency(n);

                if (diskTree.remove(r, diskTree.getRoot(), diskTree.getRootIndex()))
                {
                    cout << "Valor " << n << " removido do arquivo.\n" << endl;
                }else{
                    cout << "Valor " << n << " n�o encontrado no arquivo\n" << endl;
                }

                diskTree.print();

                break;
            case 3:
                cout << "Digite o valor a ser pesquisado: ";
                cin >> n;


                i = diskTree.search(serialCurrency(n), diskTree.getRoot());

                if (i >= 0)
                    cout << "Valor " << n << " encontrado no registro " << i << ".\n" << endl;
                else
                    cout << "Valor " << n << " n�o encontrado.\n" << endl;

                break;
            case 4:
                cout << "Listando todos os registros validos do arquivo: " << endl;
                diskTree.print();
                break;
            case 5:
                cout << "teste de inser��o ";
                for(int i = 0; i < insercao.size(); i++)
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
            case 7:
                cout << "teste de remo��o ";
                for(int i = 0; i < remocao.size(); i++)
                {
                    r.setValue(remocao[i]);
                    if(diskTree.remove(r, diskTree.getRoot(), diskTree.getRootIndex()))
                    {
                        diskTree.print();
                    }
                    cout << "------------------------------" << endl;
                }
                break;
            default:
                cout << "Op��o invalida! Tente novamente..." << endl;
                break;
            }
        }
        while (true);
    }
    else
    {
        cout << "N�o foi possivel abrir o arquivo!" << endl;
        return 1;
    }
    return 0;
}
