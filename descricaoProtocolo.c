# Mensagens

Esta sessão descreve as mensagens que fazem parte do protocolo.

## Solicitação

Esta mensagem é utilizada para se requisitar dados a um outro
nó na rede. Ela contém os seguintes campos:

    - size: Quantos aminoácidos estão sendo solicitados
    - method: Solicitação de aminoácios (S)
    - payload: Deve ser desconsiderado na requisição.

Os campos estão ordenados de forma pouco intuitiva mas isso evita o padding dos
campos, que resulta em menos bytes enviados pela rede. Leitura relacionada:

http://www.catb.org/esr/structure-packing/

## Resposta

Esta mensagem é utilizada para responder uma solicitação de aminoácidos. Ela
utiliza a mesma mensagem. Os campos são os seguintes:

    - size: Quantos aminoácidos estão sendo enviados
    - method: Resposta (R)
    - payload: Lista com aminoácidos solicitados

## Implementação de referência

A seguir está um exemplo de implementação de linguagem de programação C.

Struct que representa a mensagem:

typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;

Exemplo de como enviar uma solicitação:

/* Inicializando mensagem */
aatp_msg m = { 0 };
/* Preenchendo dados */
m.method = 'S'; /* Solicitação */
m.size = 5;
/* Zerando payload */
memset(&m.payload, 0, sizeof m.payload);
/* Enviando solicitação */
int r = send(remote_socket_fd, &m, sizeof(m), 0);

Exemplo de como enviar uma resposta:

/* Inicializando mensagem */
aatp_msg m = { 0 };
/* Preenchendo dados */
m.method = 'R'; /* Requisição */
m.size = 5; /* Enviar a quantidade que foi solicitada */
/* Zerando payload para evitar enviar lixo
   caso seja feita uma solicitação de menos de 5 aminoácidos */
memset(&m.payload, 0, sizeof m.payload);
/* Preencher com os dados */
m.payload[0] = 'A';
m.payload[1] = 'B';
m.payload[2] = 'C';
m.payload[3] = 'D';
m.payload[4] = 'E';
/* Enviando solicitação */
int r = send(remote_socket_fd, &m, sizeof(m), 0);

Exemplo de como receber a mensagem:

/* Criando buffer */
aatp_msg recv_buffer;
size_t recv_buffer_len = sizeof(recv_buffer);
/* Recebendo mensagem */
recv(remote_socket_fd, &recv_buffer, recv_buffer_len, 0);
/* Imprimindo resultados */
printf("\t Method: %c\n", recv_buffer.method);
printf("\t Size: %d\n", recv_buffer.size);
printf("\t Payload: %s\n", recv_buffer.payload);
