#include <iostream>
#include <clocale>
#include "header.h"
#include "record.h"
#include "typedfile.h"
#include "intserial.h"
#include "tree.h"

#include "serialCurrency.h"

#include <locale.h>  // for std::setlocale

using namespace std;

int main() {
   //setlocale(LC_ALL, "Portuguese");
   setlocale(LC_ALL, "C"); //usando o internacional o double printa com o . entre os decimais
   ///int op, n, i;
   int op, i;
   double n;
   double insercao[21] = {5.50, 2.01, -5.33, 10.05, 21.13, 1.88, -3.14, 2.86, 37.55, 2.99, 34.31, 49.00, 38.26, 20.85, 23.02, 17.01, 14.92, 13.81, 39.89, -13.22, 7.20};

   ///int insercao[15] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
   ///tree<intSerial> diskTree("file.dat", "TPG", 1);
   ///intSerial r;

   tree<serialCurrency> diskTree("file.dat", "TPG", 1);
   serialCurrency r;

   if (diskTree.isOpen()) {
      //cout << "Arquivo aberto com sucesso!\n\n";

      do {
         cout << "Selecione uma opção:\n\n"
              << "1. Inserir um registro\n"
              << "2. Excluir um registro\n"
              << "3. Pesquisar um registro\n"
              << "4. Listar todos os registros\n"
              << "5. TESTE INSERCAO\n"
              << "6. Sair\n\n"
              << "Sua opção: ";
         cin >> op;
         switch (op) {
            case 1:
               cout << "Digite um número inteiro: ";
               cin >> n;

               ///r = intSerial(n);
               r = serialCurrency(n);

               if (diskTree.insert(r)){
                  cout << "Valor " << n << " inserido com sucesso.\n" << endl;
                  diskTree.print();
               }else{
                    cout << "não foi possivel inserir" << endl;
               }
               break;
            case 2:
               cout << "Digite valor deseja remover do arquivo: ";
               cin >> n;
               ///i = diskTree.search(intSerial(n));
               i = diskTree.search(serialCurrency(n));
               if (i != 0) {
                  if (diskTree.remove(i))
                     cout << "Valor " << n << " removido do arquivo.\n" << endl;
               } else {
                  cout << "Valor " << n << " não encontrado no arquivo\n" << endl;
               }
               break;
            case 3:
               cout << "Digite o valor a ser pesquisado: ";
               cin >> n;
               ///i = diskTree.search(intSerial(n));
               i = diskTree.search(serialCurrency(n));

               if (i != 0)
                  cout << "Valor " << n << " encontrado no registro " << i << ".\n" << endl;
               else
                  cout << "Valor " << n << " não encontrado.\n" << endl;

               break;
            case 4:
               cout << "Listando todos os registros válidos do arquivo: " << endl;
               diskTree.print();

               break;
            case 5:
                cout << "teste de inserção ";
                for(int i = 0; i < 21; i++){
                        r.setValue(insercao[i]);
                    if(diskTree.insert(r)){
                        diskTree.print();
                    }
                    cout << "------------------------------" << endl;
                }
                break;
            case 6:
               cout << "Encerrando o programa... ";
               //diskTree.close();
               cout << "concluído." << endl;
               return 0;
               break;
            default:
               cout << "Opção inválida! Tente novamente..." << endl;
               break;
         }
      } while (true);
   } else {
      cout << "Não foi possível abrir o arquivo!" << endl;
      return 1;
   }
   return 0;
}
