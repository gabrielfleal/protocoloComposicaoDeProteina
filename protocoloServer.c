#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define CLIENTES 1

pthread_t servidor[CLIENTES];
void *thread_result_servidor;

typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;

char geraAminoacido() {
 	char aminoacidos[20] = "ARNDCQEGHILKMFPSTWYV";
  int pos = rand()%20;
  return aminoacidos[pos];
}

void *server(){
	int loc_sockfd, loc_newsockfd, tamanho;
	char linha[81];
	struct sockaddr_in loc_addr;									/* Estrutura: familia + endereco IP + porta */
	loc_sockfd = socket(AF_INET, SOCK_STREAM, 0);	/* parametros(familia, tipo, protocolo) */

	if (loc_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}

	/* Preenchendo a estrutura socket loc_addr (familia, IP, porta) */
	loc_addr.sin_family = AF_INET; 							/* familia do protocolo*/
	loc_addr.sin_addr.s_addr = INADDR_ANY; 			/* endereco IP local */
	loc_addr.sin_port = htons(1337); 	/* porta local  */
	bzero(&(loc_addr.sin_zero), 8);

  /* Bind para o endereco local*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco) */
	if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
		perror("Ligando stream socket");
		exit(1);
	}

	/* parametros(descritor socket, numeros de conex�es em espera sem serem aceites pelo accept)*/
	listen(loc_sockfd, 5);
	printf("> aguardando conexao\n");

	tamanho = sizeof(struct sockaddr_in);
  /* Accept permite aceitar um pedido de conexao, devolve um novo "socket" ja ligado ao emissor do pedido e o "socket" original*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco)*/
  loc_newsockfd =	accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho);

	aatp_msg recv_buffer;
	size_t recv_buffer_len = sizeof(recv_buffer);
	while(recv(loc_newsockfd, &recv_buffer, recv_buffer_len, 0)){	/* Recebendo mensagem */
		if(toupper(recv_buffer.method) == 'S'){
			aatp_msg m = { 0 };		/* Inicializando mensagem */
			m.method = 'R'; 			/* Resposta */
			m.size = 5; 					/* Enviar a quantidade que foi solicitada */
			memset(&m.payload, 0, sizeof m.payload);	/* Zerando payload para evitar enviar lixo caso seja feita uma solicitação de menos de 5 aminoácidos */

			int i;
			for(i = 0; i < recv_buffer.size; i++){
				m.payload[i] = geraAminoacido();;
			}
			int r = send(loc_newsockfd, &m, sizeof(m), 0);	/* Enviando solicitação */
		}
	}

	/* fechamento do socket local */
	close(loc_sockfd);
	close(loc_newsockfd);
}

int main(int argc, char *argv[]) {
		pthread_create(&servidor, NULL, server, NULL);
    sleep(1);
    pthread_join(servidor, &thread_result_servidor);
}
