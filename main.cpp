#include <iostream>
#include <clocale>
#include "header.h"
#include "record.h"
#include "typedfile.h"
#include "intserial.h"

using namespace std;

int main() {
   setlocale(LC_ALL, "Portuguese");
   int op, n, i;
   typedFile<intSerial> arq("teste.dat", "TPG", 65);
   record<intSerial> r;

   if (arq.isOpen()) {
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

               r = record<intSerial>(n);

               if (arq.insertRecord(r))
                  cout << "Valor " << n << " inserido com sucesso.\n" << endl;

               break;
            case 2:
               cout << "Digite valor deseja remover do arquivo: ";
               cin >> n;
               i = arq.search(intSerial(n));
               if (i != 0) {
                  if (arq.deleteRecord(i))
                     cout << "Valor " << n << " removido do arquivo.\n" << endl;
               } else {
                  cout << "Valor " << n << " n�o encontrado no arquivo\n" << endl;
               }
               break;
            case 3:
               cout << "Digite o valor a ser pesquisado: ";
               cin >> n;
               i = arq.search(intSerial(n));

               if (i != 0)
                  cout << "Valor " << n << " encontrado no registro " << i << ".\n" << endl;
               else
                  cout << "Valor " << n << " n�o encontrado.\n" << endl;

               break;
            case 4:
               cout << "Listando todos os registros v�lidos do arquivo: " << endl;
               i = arq.getFirstValid();

               if (i == 0) {
                  cout << "N�o existem registros no arquivo.\n" << endl;
               } else {
                  while (i != 0 ) { //i != 0
                     arq.readRecord(r, i);
                     cout << "- " << r.getData().getValue() << endl;
                     i = r.getNext();
                  }
                  cout << endl;
               }

               break;
            case 5:
               cout << "Encerrando o programa... ";
               arq.close();
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
