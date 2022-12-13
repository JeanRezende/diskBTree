#include <iostream>
#include <clocale>
#include "header.h"
#include "record.h"
#include "typedfile.h"
#include "intserial.h"
#include "tree.h"

using namespace std;

int main() {
   setlocale(LC_ALL, "Portuguese");
   int op, n, i;
   //typedFile<intSerial> arq("teste.dat", "TPG", 65);
   tree<intSerial> diskTree("file.dat", "TPG", 1);
   intSerial r;

   if (diskTree.isOpen()) {
      //cout << "Arquivo aberto com sucesso!\n\n";

      do {
         cout << "Selecione uma op��o:\n\n"
              << "1. Inserir um registro\n"
              << "2. Excluir um registro\n"
              << "3. Pesquisar um registro\n"
              << "4. Listar todos os registros\n"
              << "5. Sair\n\n"
              << "Sua op��o: ";
         cin >> op;
         switch (op) {
            case 1:
               cout << "Digite um n�mero inteiro: ";
               cin >> n;

               r = intSerial(n);

               if (diskTree.insert(r)){
                  cout << "Valor " << n << " inserido com sucesso.\n" << endl;
                  diskTree.print();
               }else{
                    cout << "n�o foi possivel inserir" << endl;
               }


               break;
            case 2:
               cout << "Digite valor deseja remover do arquivo: ";
               cin >> n;
               i = diskTree.search(intSerial(n));
               if (i != 0) {
                  if (diskTree.remove(i))
                     cout << "Valor " << n << " removido do arquivo.\n" << endl;
               } else {
                  cout << "Valor " << n << " n�o encontrado no arquivo\n" << endl;
               }
               break;
            case 3:
               cout << "Digite o valor a ser pesquisado: ";
               cin >> n;
               i = diskTree.search(intSerial(n));

               if (i != 0)
                  cout << "Valor " << n << " encontrado no registro " << i << ".\n" << endl;
               else
                  cout << "Valor " << n << " n�o encontrado.\n" << endl;

               break;
            case 4:
               cout << "Listando todos os registros v�lidos do arquivo: " << endl;
               diskTree.print();

               break;
            case 5:
               cout << "Encerrando o programa... ";
               //diskTree.close();
               cout << "conclu�do." << endl;
               return 0;
               break;
            default:
               cout << "Op��o inv�lida! Tente novamente..." << endl;
               break;
         }
      } while (true);
   } else {
      cout << "N�o foi poss�vel abrir o arquivo!" << endl;
      return 1;
   }
   return 0;
}
