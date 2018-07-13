#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#define CLIENTES 1
#define TAM_PROTEINA 609

pthread_t cliente[CLIENTES];
void *thread_result_cliente;
int idClientes[CLIENTES];
char proteinaOriginal[TAM_PROTEINA];
char novaProteina[TAM_PROTEINA];
pthread_mutex_t mutex;
char ipServer[CLIENTES][14];

typedef struct {		// Struct mensagem
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;

void buscarServer(){
	int i;

	FILE *servidores;
	servidores = fopen("servidores.txt", "r");

	if(servidores != NULL){
		for(i = 0; i < CLIENTES; i++){
			fgets(ipServer[i], sizeof(ipServer[i]), servidores);	// grava o ip do servidor de acordo com o id/posição
		}
		fclose(servidores);
	}else{
		printf("Arquivo indisponível.\n");
		exit(1);
	}
}

void leituraProteina(){
	FILE *proteina;
	proteina = fopen("proteina.txt", "r");
	fgets(proteinaOriginal, sizeof(proteinaOriginal), proteina);
	fclose(proteina);
}

void armazenarArquivo(){
	FILE *file = fopen("novaProteina.txt", "w");
	int results = fputs(novaProteina, file);
}

void printProteina(){
		int i;
		for(i = 0; i < TAM_PROTEINA; i++){
			printf("%s", novaProteina);
		}
}

void completarProteina(aatp_msg recebido){
	int i, j;
	for(i = 0; i < recebido.size; i++){
		for(j = 0; j < TAM_PROTEINA; j++){
			if(toupper(recebido.payload[i]) == proteinaOriginal[j] && novaProteina[j] == '-'){ // Compara com a proteína original e com a cópia para verificar se precisa completar naquela posição
				pthread_mutex_lock(&mutex);
				novaProteina[j] = recebido.payload[i];  // Adiciona no array de cópia
				armazenarArquivo();                     // Armazena a modificação no arquivo
				printProteina();
				pthread_mutex_unlock(&mutex);
				break;
			}
		}
	}
}

void *client(void *id){
	char *rem_hostname;
	int rem_port;
	struct sockaddr_in rem_addr;												/* Estrutura: familia + endereco IP + porta */
	int rem_sockfd;
	char linha[81];

	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (familia, IP, porta) */
	rem_hostname = ipServer[*(int *)id];
	rem_port = 1337;
	rem_addr.sin_family = AF_INET; 											/* familia do protocolo*/
	rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); /* endereco IP local */
	rem_addr.sin_port = htons(rem_port); 								/* porta local  */
	rem_sockfd = socket(AF_INET, SOCK_STREAM, 0);				/* parametros(familia, tipo, protocolo) */

	if (rem_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}
	printf("> Conectando no servidor '%s : %d'\n", rem_hostname, rem_port);

	/* Estabelece uma conexao remota */
	/* parametros(descritor socket, estrutura do endereco remoto, comprimento do endereco) */
	if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
		perror("Conectando stream socket");
		exit(1);
	}

	aatp_msg m = { 0 }; 																/* Inicializando mensagem */
	m.method = 'S'; 																		/* Solicitação */
	m.size = 5;
	memset(&m.payload, 0, sizeof m.payload); 						/* Zerando payload */
	int r = send(rem_sockfd, &m, sizeof(m), 0); 				/* Enviando solicitação */

	aatp_msg recv_buffer;																/* Criando buffer */
	size_t recv_buffer_len = sizeof(recv_buffer);
	recv(rem_sockfd, &recv_buffer, recv_buffer_len, 0);	/* Recebendo mensagem */

	printf("\t Payload: %s\n", recv_buffer.payload);		/* Imprimindo resultados */
	if(toupper(recv_buffer.method) == 'R'){
			completarProteina(recv_buffer);
	}

	close(rem_sockfd);	                                /* Fechando socket remota */
}

int main(int argc, char *argv[]) {
		leituraProteina();
    buscarServer();
    int i;
		for(i = 0; i < TAM_PROTEINA; i++){
			novaProteina[i] = '-';
		}
		pthread_mutex_init(&mutex, NULL);

    for(i = 0; i < CLIENTES; i++) {
			idClientes[i] = i;
    	pthread_create(&cliente[i], NULL, client, &idClientes);
   	}

    for(i = 0; i < CLIENTES; i++) {
        pthread_join(cliente[i], &thread_result_cliente);
    }

}
