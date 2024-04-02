// Nome: Matheus da Cruz Percine Pinto
// DRE: 121068501

//Encontra o menor e o maior valor em um vetor de float
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define TESTE

//escopo global (par ser compartilhado com as threads)
float *vetor; //vetor de entrada

//define o tipo de dado de entrada das threads
typedef struct {
   int id; //id da thread
   long int tamBloco; //tamanho do bloco (cada thread processa um bloco)
} tArgs;

//define o tipo de retorno das threads
typedef struct {
   float soma;
} tSoma;


//fluxo das threads
void * SomarElementos(void * arg) {
   tArgs *args = (tArgs *) arg; 

   tSoma *somaLocal; //variavel local com os resultados da thread
   somaLocal = (tSoma*) malloc(sizeof(tSoma));
   if(somaLocal==NULL) {
      pthread_exit(NULL);
   }
   
   long int ini = args->id * args->tamBloco; //elemento inicial do bloco da thread
   long int fim = ini + args->tamBloco; //elemento final(nao processado) do bloco da thread

   //soma elementos do bloco da thread
   somaLocal->soma = vetor[ini];
   for(long int i=ini+1; i<fim; i++) {
        somaLocal->soma += vetor[i];
   }   

   //retorna o resultado da soma local
   pthread_exit((void *) somaLocal); 
}

//fluxo principal
int main(int argc, char *argv[]) {
   float somaGeral; //valores encontrados pela solucao concorrente
   long int dim; //dimensao do vetor de entrada
   long int tamBloco; //tamanho do bloco de cada thread 
   int nthreads; //numero de threads que serao criadas

   pthread_t *tid; //vetor de identificadores das threads no sistema
   tSoma *retorno; //valor de retorno das threads

#ifdef TESTE
   float somaCorreta; //valores de referencia para os testes
#endif

   //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
   if(argc < 2) {
       fprintf(stderr, "Digite: %s <numero threads>\n", argv[0]);
       return 1; 
   }
   nthreads = atoi(argv[1]);
   printf("nthreads=%d\n", nthreads); 

   //carrega o vetor de entrada
   scanf("%ld", &dim); //primeiro pergunta a dimensao do vetor
   printf("dim=%ld\n", dim); 

   //aloca o vetor de entrada
   vetor = (float*) malloc(sizeof(float)*dim);
   if(vetor == NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 2;
   }
   //preenche o vetor de entrada
   for(long int i=0; i<dim; i++)
      scanf("%f", &vetor[i]);
#ifdef TESTE
   //le os valores esperados da soma
   scanf("%f", &somaCorreta);
   for(long int i=0; i<dim; i++)
      printf("%f ", vetor[i]); 
#endif   
   //inicializa as variaveis de saida com o valor na primeira posicao do vetor

   somaGeral = 0;

   //cria as threads
   tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
   if(tid==NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 3;
   }
   tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
   if(!tamBloco) 
      printf("\nA quantidade de threads eh maior que a quantidade de elementos, a execucao sera sequencial!\n");
   for(int i=0; i<nthreads; i++) {
       //aloca e preenche argumentos para thread
       tArgs *args = (tArgs*) malloc(sizeof(tArgs));
       if (args == NULL) {
          printf("--ERRO: malloc()\n"); 
          pthread_exit(NULL);
       }
       args->id = i; 
       args->tamBloco = tamBloco; 
       if(pthread_create(tid+i, NULL, SomarElementos, (void*) args)){
          fprintf(stderr, "ERRO--pthread_create\n");
          return 5;
       }
   }
   
   //processa a parte final do vetor
   if(dim%nthreads) {
      puts("\nMain com tarefa");
      for(long int i=dim-(dim%nthreads); i<dim; i++) {
        somaGeral += vetor[i];
      }
   } 

   //aguarda o termino das threads
   for(int i=0; i<nthreads; i++) {
      if(pthread_join(*(tid+i), (void**) &retorno)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 6;
      }
      //somar os valores retornados pelas threads
      somaGeral += retorno->soma;
   }

   //exibir os resultados
   printf("\nSoma Geral:%f \n", somaGeral);

#ifdef TESTE
   printf("Soma Correta:%f \n", somaCorreta);
#endif

   //libera as areas de memoria alocadas
   free(vetor);
   free(tid);

   return 0;
}
