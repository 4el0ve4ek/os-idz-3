#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 3

int clientSockets[MAX_CLIENTS];
pthread_t threads[MAX_CLIENTS];
pthread_mutex_t mutex;
pthread_mutex_t flower_mutex;

#define PERIOD_OF_DRY 80000
#define FLOWERS_AMOUNT 40
int flowers[FLOWERS_AMOUNT];


int process_flower(int flower_number){
    if (flower_number >= FLOWERS_AMOUNT || flower_number < 0) {
        return -1;  /* wrong input */
    }
    pthread_mutex_lock(&flower_mutex);
    int ret = flowers[flower_number];
    if (ret == 0) {
        flowers[flower_number] = 1; /* caller water flower */
    }
    pthread_mutex_unlock(&flower_mutex);
    return ret;
}

void dry_some_flower() {
    if (rand() % PERIOD_OF_DRY != 0) {
        return;
    }

    int flower_number = (rand() % FLOWERS_AMOUNT + FLOWERS_AMOUNT) % FLOWERS_AMOUNT;

    pthread_mutex_lock(&flower_mutex);
    flowers[flower_number] = 0;
    pthread_mutex_unlock(&flower_mutex);

    printf("%d flower now need water\n", flower_number);
}

void* handleClient(void* arg) {
    int clientSocket = *(int*)arg;
    char buffer[1024];

    while (1) {
        // receive request for client
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) { /* error occurred */
            break;
        }

        int flower_number = strtol(buffer, NULL, 10);
        int ret = process_flower(flower_number);
        dry_some_flower();

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", ret);
        send(clientSocket, buffer, strlen(buffer), 0);
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] == clientSocket) {
            clientSockets[i] = -1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(clientSocket);
    free(arg);
    pthread_exit(NULL);
}

void init_flowers() {
    for (int i = 0; i < 40; ++i) {
        flowers[i] = 1;
    }

    printf("All flowers are initially watered\n");
}

void mutex_init() {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&flower_mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
}

int main(int arc, char* argv[]) {
    srand(time(NULL));

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    long port = strtol(argv[1], NULL, 10);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clientSockets[i] = -1;
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, addrLen) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // start listening
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %ld...\n", port);

    mutex_init();

    init_flowers();

    // start server work
    while (1) {

        // new client accept
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New client connected.\n");

        // binding new client for an empty slot
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clientSockets[i] == -1) {
                clientSockets[i] = clientSocket;
                break;
            }
        }

        if (i == MAX_CLIENTS) { /* too many clients */
            printf("Maximum number of clients reached. Connection rejected.\n");
            close(clientSocket);
        } else {
            int* newClientSocket = malloc(sizeof(int));
            *newClientSocket = clientSocket;

            // handle new client
            if (pthread_create(&threads[i], NULL, handleClient, (void*)newClientSocket) != 0) {
                perror("Thread creation failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    pthread_mutex_destroy(&mutex);
    close(serverSocket);

    return 0;
}