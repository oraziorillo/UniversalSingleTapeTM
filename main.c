#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define boolean int
#define BUFFER_SIZE 32
#define MAXLINE 30
#define LUNG_CHUNK 64
#define ULTIMA_POS_CHUNK 63
#define MAX(x,y) ((x)>(y)?(x):(y))

//struttura di un nodo della lista di adiacenza che rappresenta la MT
typedef struct nodoListaAdj {
    boolean buco;
    int stato_di_arrivo;
    char carattere_scritto;
    char movimento_testina;
    struct nodoListaAdj *next;
} adjListNode;


//puntatore a adjListNode
typedef adjListNode *pTransition;


//struttura di un nodo del nastro di memoria/input
typedef struct porzione {
    boolean reversed;
    int celle_occupate;
    char contenuto[LUNG_CHUNK];
    struct porzione *next, *prev;
} chunk;


//puntatore ad un nodo del nastro
typedef chunk *pChunk;


//struttura testina, contiene il puntatore ad un chunk e un int che indica la posizione sulla stringa
typedef struct {
    int posizione;
    pChunk currChunk;
} tipoTestina;


//puntatore a tipoTestina
typedef tipoTestina *pTipoTestina;


//struttura che rappresenta una singola computazione non deterministica della macchina
typedef struct elemCoda{
    int stato;
    long int contatore_passi;
    pTipoTestina testina;
    struct elemCoda * next;
} queueElem;


//puntatore a queueElem
typedef queueElem *pConfiguration;


//struttura coda implementata tramite array
typedef struct Queue {
    pConfiguration head;
    pConfiguration tail;
} queue;


//funzione che costruisce il grafo leggendo le transizioni
int SalvaTransizioni(pTransition ***V) {
    int lenght = 0, stato_in, stato_fin, max_stato, colonna;
    char input, output, move;
    pTransition newNode;
    boolean letto_int = scanf("%d %c %c %c %d\n", &stato_in, &input, &output, &move, &stato_fin);
    while (letto_int) {
        newNode = (pTransition) malloc(sizeof(adjListNode));
        if (input == output && stato_in == stato_fin && move == 'S')
            newNode->buco = 1;
        else
            newNode->buco = 0;
        newNode->carattere_scritto = output;
        newNode->movimento_testina = move;
        newNode->stato_di_arrivo = stato_fin;
        max_stato = MAX(stato_in, stato_fin);
        if (max_stato >= lenght) {
            (*V) = (pTransition **) realloc((*V), (max_stato + 1) * sizeof(pTransition *));
            while (lenght <= max_stato) {
                (*V)[lenght] = (pTransition *) malloc(75 * sizeof(pTransition));
                for (int i = 0; i < 75; i++) {
                    (*V)[lenght][i] = NULL;
                }
                lenght++;
            }
        }
        colonna = (int) input - 48;
        newNode->next = (*V)[stato_in][colonna];
        (*V)[stato_in][colonna] = newNode;
        letto_int = scanf("%d %c %c %c %d\n", &stato_in, &input, &output, &move, &stato_fin);
    }
    return lenght;
}


//funzione che aggiunge un blank a destra dell'ultima cella occupata del nastro di memoria
pChunk AllungaNastroDx(pChunk last) {
    pChunk newChunk;
    newChunk = (pChunk) malloc(sizeof(chunk));
    newChunk->reversed = 0;
    newChunk->contenuto[0] = '_';
    newChunk->celle_occupate = 1;
    newChunk->prev = last;
    newChunk->next = NULL;
    last->next = newChunk;
    return newChunk;
}


//funzione che aggiunge un blank a sinistra della prima cella occupata del nastro di memoria
pChunk AllungaNastroSx(pChunk first) {
    pChunk newChunk;
    newChunk = (pChunk) malloc(sizeof(chunk));
    newChunk->reversed = 1;
    newChunk->contenuto[0] = '_';
    newChunk->celle_occupate = 1;
    newChunk->next = first;
    newChunk->prev = NULL;
    first->prev = newChunk;
    return newChunk;
}


//funzione per l'aggiunta di un carattere a destra dell'ultima posizione occupata del nastro di memoria
pChunk InserisciInFondo(pChunk last_chunk, char tmp) {
    pChunk newChunk;
    if (last_chunk == NULL) {
        newChunk = (pChunk) malloc(sizeof(chunk));
        newChunk->reversed = 0;
        newChunk->contenuto[0] = tmp;
        newChunk->celle_occupate = 1;
        newChunk->next = NULL;
        newChunk->prev = NULL;
        return newChunk;
    } else {
        if (last_chunk->celle_occupate == LUNG_CHUNK) {
            newChunk = (pChunk) malloc(sizeof(chunk));
            newChunk->reversed = 0;
            newChunk->contenuto[0] = tmp;
            newChunk->celle_occupate = 1;
            newChunk->next = NULL;
            newChunk->prev = last_chunk;
            last_chunk->next = newChunk;
            return newChunk;
        } else {
            last_chunk->contenuto[last_chunk->celle_occupate] = tmp;
            last_chunk->celle_occupate++;
            return last_chunk;
        }
    }
}


//funzione che copia lunghezza_src elementi da sinistra verso destra
void stringCopyDx(const char src[], char dest[], int lunghezza_src) {
    for (int i = 0; i < lunghezza_src; i++) {
        dest[i] = src[i];
    }
}


//funzione che copia l'intera stringa del chunk
void stringCopy(const char src[], char dest[]) {
    for (int i = 0; i < LUNG_CHUNK; i++) {
        dest[i] = src[i];
    }
}


//funzione che copia il nastro di memoria e restituisce il puntatore alla nuova testina
pTipoTestina CopiaNastro(pTipoTestina src, pTipoTestina dst) {
    pChunk newChunk, newCurr, oldCurr = src->currChunk;
    pChunk p = malloc(sizeof(chunk));
    stringCopy(src->currChunk->contenuto, p->contenuto);
    p->reversed = src->currChunk->reversed;
    p->celle_occupate = src->currChunk->celle_occupate;
    p->next = NULL;
    p->prev = NULL;
    dst->currChunk = p;
    newCurr = dst->currChunk;
    oldCurr = oldCurr->next;
    while (oldCurr != NULL) {
        newChunk = malloc(sizeof(chunk));
        newChunk->reversed = oldCurr->reversed;
        stringCopyDx(oldCurr->contenuto, newChunk->contenuto, oldCurr->celle_occupate);
        newChunk->celle_occupate = oldCurr->celle_occupate;
        newChunk->next = NULL;
        newChunk->prev = newCurr;
        newCurr->next = newChunk;
        newCurr = newCurr->next;
        oldCurr = oldCurr->next;
    }
    newCurr = dst->currChunk;
    oldCurr = src->currChunk->prev;
    while (oldCurr != NULL) {
        newChunk = malloc(sizeof(chunk));
        newChunk->reversed = oldCurr->reversed;
        stringCopyDx(oldCurr->contenuto, newChunk->contenuto, oldCurr->celle_occupate);
        newChunk->celle_occupate = oldCurr->celle_occupate;
        newChunk->prev = NULL;
        newChunk->next = newCurr;
        newCurr->prev = newChunk;
        newCurr = newCurr->prev;
        oldCurr = oldCurr->prev;
    }
    dst->posizione = src->posizione;
    return dst;
}


//lista di funzioni utili per gestire la coda delle computazioni

void Enqueue(queue *Q, pConfiguration config) {
    config->next = NULL;
    if (Q->head == NULL) {
        Q->head = config;
        Q->tail = config;
    }
    else {
        Q->tail->next = config;
        Q->tail = config;
    }
}


pConfiguration Dequeue(queue *Q) {
    pConfiguration x;
    if (Q->head == NULL && Q->tail == NULL)
        return NULL;
    x = Q->head;
    if (Q->head == Q->tail) {
        Q->head = NULL;
        Q->tail = NULL;
    } else {
        Q->head = Q->head->next;
    }
    return x;
}


//funzione di ricerca un elemeno in un array non ordinato
int Ricerca(int elem, const int array[], int dim) {
    for (int i = 0; i < dim; i++) {
        if (array[i] == elem)
            return 1;
    }
    return 0;
}


void FreeTape(pChunk head) {
    pChunk tmp;
    while (head != NULL) {
        tmp = head->next;
        //printf("prima: %s|", head->contenuto);
        free(head);
        //printf("dopo:%s|", head->contenuto);
        head = tmp;
    }
}


void FreeListAdj(pTransition head) {
    pTransition tmp;
    while (head != NULL) {
        tmp = head->next;
        free(head);
        head = tmp;
    }
}


int main() {
    int stati_grafo, dim_accettazione = 0;
    int *temp, *accettazione;
    char buffer[BUFFER_SIZE], tmpchar;
    long int max_passi;
    pTransition **matrice_transizioni = NULL;
    pConfiguration curr_config, newConfig;
    pTransition curr_transition;
    pTipoTestina nuovaTestina;
    pChunk nastro;
    queue *coda_computazioni;
    //freopen("2018_9_7_14_56_41_936.txt", "r", stdin);
    fgets(buffer, MAXLINE, stdin);
    //salvo le transizioni in un grafo
    stati_grafo = SalvaTransizioni(&matrice_transizioni);

#ifdef DEBUG
    pTransition curr;
    printf("TRANSIZIONI SALVATE:\n");
    for (int i = 0; i < stati_grafo; i++)
        for (int j = 0; j < 75; j++)
            if (matrice_transizioni[i][j] != NULL) {
                for (curr = matrice_transizioni[i][j]; curr != NULL; curr = curr->next) {
                    printf("%d  %c  %c  %c  %d   buco: %d\n",
                           i, (char)(j + 48), curr->carattere_scritto, curr->movimento_testina, curr->stato_di_arrivo, curr->buco);
                }
            }
#endif

    //se sono uscito dal while vuol dire che devo leggere "acc\n",
    //perciò inizializzo un array che contiene gli stati di accettazione
    fgets(buffer, MAXLINE, stdin);
    temp = (int *) malloc(100 * sizeof(int));
    fgets(buffer, MAXLINE, stdin);
    while (strncmp(buffer, "max", 3) != 0) {
        temp[dim_accettazione] = atoi(buffer);
        dim_accettazione++;
        fgets(buffer, MAXLINE, stdin);
    }
    accettazione = (int *) malloc(dim_accettazione * sizeof(int));
    for (int i = 0; i < dim_accettazione; i++) {
        accettazione[i] = temp[i];
    }

#ifdef DEBUG
    printf("\nSTATI DI ACCETTAZIONE:\n");
    for (int k = 0; k < dim_accettazione; k++)
        printf("%d\n", accettazione[k]);
#endif

    free(temp);
    //letto "max" sto per leggere il massimo dei passi di computazione possibili,
    //perciò lo salvo nella variabile che ho appositamente inizializzato
    scanf("%ld\n", &max_passi);
    //brucio la riga "run", poi tutto quello che c'è dopo fino a "\n" è stringa di input
    fgets(buffer, MAXLINE, stdin);
    //faccio partire l'esecuzione
    tmpchar = (char) fgetc(stdin);
    do {
        long int lunghezza_input = 0;
        int output;
        boolean loop_found = 0;
        //faccio un controllo su tmpchar nel caso in cui abbia \n finali
        if (tmpchar == '\n') {
            tmpchar = (char) fgetc(stdin);
            continue;
        }
        //inizializzo la prima computazione
        curr_config = (pConfiguration) malloc(sizeof(queueElem));
        curr_config->testina = (pTipoTestina) malloc(sizeof(tipoTestina));
        curr_config->stato = 0;
        curr_config->contatore_passi = 0;
        curr_config->testina->posizione = 0;
        nastro = NULL;
        //salvo sul nastro di memoria la prima stringa di input
        while (tmpchar != '\n' && tmpchar != EOF && lunghezza_input <= max_passi) {
            nastro = InserisciInFondo(nastro, tmpchar);
            lunghezza_input++;
            if (lunghezza_input == 1)
                curr_config->testina->currChunk = nastro;
            tmpchar = (char) fgetc(stdin);
        }

#ifdef DEBUG
        printf("NASTRO: ");
        while (curr_config->testina->currChunk != NULL) {
            for (int i = 0; i < LUNG_CHUNK; i++)
                printf("%c", curr_config->testina->currChunk->contenuto[i]);
            curr_config->testina->currChunk = curr_config->testina->currChunk->next;
        }
        printf("\n");
#endif

        if (lunghezza_input > max_passi)
            while (tmpchar != '\n' && tmpchar != EOF)
                tmpchar = (char) fgetc(stdin);
        //inizializzo la coda delle computazioni
        coda_computazioni = malloc(sizeof(queue));
        coda_computazioni->tail = NULL;
        coda_computazioni->head = NULL;
        //simulazione della MT sulla stringa salvata
        while (1) {
            int input_char, pos_testina, trans_found = 0;

#ifdef DEBUG
            printf("\n[CONFIGURAZIONE PRIMA]     stato:%d     contatore passi:%ld     chunk:%s     posizione:%d\n",
                   curr_config->stato, curr_config->contatore_passi, curr_config->testina->currChunk->contenuto, curr_config->testina->posizione);
#endif

            //controllo che non abbia ecceduto il limite di passi di computazione, in caso affermativo stampo U
            if (curr_config->contatore_passi > max_passi) {
                while (curr_config->testina->currChunk->prev != NULL)
                    curr_config->testina->currChunk = curr_config->testina->currChunk->prev;
                FreeTape(curr_config->testina->currChunk);
                free(curr_config->testina);
                free(curr_config);
                output = 2;
                break;
            }
            //incodo possibili transizione a partire dall'attuale configurazione
            pos_testina = curr_config->testina->posizione;
            input_char = (int) (curr_config->testina->currChunk->contenuto[pos_testina]) - 48;
            curr_transition = matrice_transizioni[curr_config->stato][input_char];
            while (curr_transition != NULL) {

#ifdef DEBUG
                printf("[TRANSIZIONE] %d %c %c %c %d\n", curr_config->stato,
                       curr_config->testina->currChunk->contenuto[curr_config->testina->posizione],
                       curr_transition->carattere_scritto, curr_transition->movimento_testina,
                       curr_transition->stato_di_arrivo);
#endif
                //controllo se sono in un buco
                if (curr_transition->buco) {
                    loop_found = 1;
                    curr_transition = curr_transition->next;
                    continue;
                }
                //controllo se curr_transition possa portarmi in un loop dx/sx
                if (curr_config->testina->currChunk->contenuto[curr_config->testina->posizione] == '_' &&
                    curr_config->stato == curr_transition->stato_di_arrivo &&
                    curr_config->testina->posizione == (curr_config->testina->currChunk->celle_occupate - 1) &&
                    ((curr_transition->movimento_testina == 'R' && !(curr_config->testina->currChunk->reversed)) ||
                     (curr_transition->movimento_testina == 'L' && curr_config->testina->currChunk->reversed))) {
                    loop_found = 1;
                    curr_transition = curr_transition->next;
                    continue;
                }
                trans_found++;
                //se c'è una sola possibile transizione è inutile creare una nuova configurazione
                if (trans_found == 1 && curr_transition->next == NULL)
                    newConfig = curr_config;
                else {
                    newConfig = (pConfiguration) malloc(sizeof(queueElem));
                    nuovaTestina = (pTipoTestina) malloc(sizeof(tipoTestina));
                    //copio il nastro di memoria solo se ho computazioni non deterministiche
                    if (trans_found > 1)
                        nuovaTestina = CopiaNastro(curr_config->testina, nuovaTestina);
                    else
                        nuovaTestina->currChunk = curr_config->testina->currChunk;
                    newConfig->testina = nuovaTestina;
                }
                //aggiorno lo stato della nuova configurazione
                newConfig->stato = curr_transition->stato_di_arrivo;
                //aggiorno il contatore dei passi della nuova configurazione
                newConfig->contatore_passi = curr_config->contatore_passi + 1;
                //aggiorno il carattere scritto sul nastro di memoria
                newConfig->testina->currChunk->contenuto[pos_testina] = curr_transition->carattere_scritto;
                //sposto la testina sul nastro
                switch (curr_transition->movimento_testina) {
                    case 'R':
                        if (!(newConfig->testina->currChunk->reversed)) {
                            //se non è reversed mi comporto normalmente
                            if (pos_testina == ULTIMA_POS_CHUNK) {
                                newConfig->testina->posizione = 0;
                                if (newConfig->testina->currChunk->next == NULL)
                                    newConfig->testina->currChunk = AllungaNastroDx(newConfig->testina->currChunk);
                                else
                                    newConfig->testina->currChunk = newConfig->testina->currChunk->next;
                            } else
                                newConfig->testina->posizione = pos_testina + 1;
                        } else {
                            //se è reversed allora innanzitutto mi sposto sul chunk successivo se la testina è su 0 e mi comporto
                            //in maniera differente a seconda di se il chunk successivo sia reversed o meno, inoltre il nodo
                            //successivo non può essere NULL. Se la testina non è su 0 la decremento anziché incrementarla
                            if (pos_testina == 0) {
                                newConfig->testina->currChunk = newConfig->testina->currChunk->next;
                                if (newConfig->testina->currChunk->reversed)
                                    newConfig->testina->posizione = ULTIMA_POS_CHUNK;
                                else
                                    newConfig->testina->posizione = 0;
                            } else {
                                newConfig->testina->posizione = pos_testina - 1;
                            }
                        }
                        break;
                    case 'L':
                        if (!(curr_config->testina->currChunk->reversed)) {
                            //se non è reversed mi comporto normalmente con alcune accortezze in base al chunk precendete
                            if (pos_testina == 0) {
                                if (newConfig->testina->currChunk->prev == NULL) {
                                    newConfig->testina->currChunk = AllungaNastroSx(newConfig->testina->currChunk);
                                    newConfig->testina->posizione = 0;
                                } else {
                                    newConfig->testina->currChunk = newConfig->testina->currChunk->prev;
                                    if (newConfig->testina->currChunk->reversed)
                                        newConfig->testina->posizione = 0;
                                    else
                                        newConfig->testina->posizione = ULTIMA_POS_CHUNK;
                                }
                            } else
                                newConfig->testina->posizione = pos_testina - 1;
                        } else {
                            //se è reversed allora innanzitutto controllo che il chunk precedente non sia NULL, se lo è allungo il
                            //nastro a sinistra, se non lo è mi sposto semplicemente su di esso. Inoltre, se la testina è su 47,
                            //la riporto a 0 perché il chunk precedente è sicuramente reversed
                            if (pos_testina == ULTIMA_POS_CHUNK) {
                                newConfig->testina->posizione = 0;
                                if (newConfig->testina->currChunk->prev == NULL)
                                    newConfig->testina->currChunk = AllungaNastroSx(newConfig->testina->currChunk);
                                else
                                    newConfig->testina->currChunk = newConfig->testina->currChunk->prev;
                            } else {
                                newConfig->testina->posizione = pos_testina + 1;
                            }
                        }
                        break;
                    default:
                        newConfig->testina->posizione = pos_testina;
                        break;
                }
                if (newConfig->testina->posizione == newConfig->testina->currChunk->celle_occupate) {
                    newConfig->testina->currChunk->contenuto[newConfig->testina->posizione] = '_';
                    newConfig->testina->currChunk->celle_occupate++;
                }

#ifdef DEBUG
                printf("[CONFIGURAZIONE DOPO]      stato:%d     contatore passi:%ld     chunk:%s    posizione:%d\n",
                       newConfig->stato, newConfig->contatore_passi, newConfig->testina->currChunk->contenuto, newConfig->testina->posizione);
#endif

                Enqueue(coda_computazioni, newConfig);
                curr_transition = curr_transition->next;
            }
            //se sono in uno stato pozzo (trans_found = 0) potrebbe essere di accettazione, perciò faccio un controllo,
            //se lo è accetto, se non lo è vado avanti con la computazione liberando il nastro, la testina e la configurazione correnti
            if (trans_found == 0) {
                while (curr_config->testina->currChunk->prev != NULL)
                    curr_config->testina->currChunk = curr_config->testina->currChunk->prev;
                FreeTape(curr_config->testina->currChunk);
                if (Ricerca(curr_config->stato, accettazione, dim_accettazione)) {
                    free(curr_config->testina);
                    free(curr_config);
                    output = 1;
                    break;
                } else if (coda_computazioni->head == NULL) {
                    free(curr_config->testina);
                    free(curr_config);
                    if (loop_found)
                        output = 2;
                    else
                        output = 0;
                    break;
                } else {
                    free(curr_config->testina);
                    free(curr_config);
                    curr_config = Dequeue(coda_computazioni);
                }
            } else if (trans_found == 1) {
                curr_config = Dequeue(coda_computazioni);
                //if (curr_config->contatore_passi == 390)
                    //exit(1);
            } else {
                //se la coda non è vuota, eseguo il successivo passo di computazione della config in testa alla coda,
                //altrimenti, se la coda è vuota rifiuto la stringa
                free(curr_config->testina);
                free(curr_config);
                curr_config = Dequeue(coda_computazioni);
            }
        }
        //dealloco tutte le configurazioni incodate
        while (coda_computazioni->head != NULL) {
            curr_config = Dequeue(coda_computazioni);
            while (curr_config->testina->currChunk->prev != NULL)
                curr_config->testina->currChunk = curr_config->testina->currChunk->prev;
            FreeTape(curr_config->testina->currChunk);
            free(curr_config->testina);
            free(curr_config);
        }
        free(coda_computazioni);
        //stampo il risultato della computazione effettuata
        if (output == 0)
            fputs("0\n", stdout);
        if (output == 1)
            fputs("1\n", stdout);
        if (output == 2)
            fputs("U\n", stdout);
        //faccio un controllo su tmpchar perchè se alla fine dell'ultima stringa letta c'è EOF devo terminare
        if (tmpchar == EOF)
            break;
        //vado avanti con la lettura
        tmpchar = (char) fgetc(stdin);
    } while (tmpchar != EOF);
    free(accettazione);
    for (int i = 0; i < stati_grafo; i++) {
        for (int j = 0; j < 75; ++j) {
            if (matrice_transizioni[i][j] != NULL)
                FreeListAdj(matrice_transizioni[i][j]);
        }
        free(matrice_transizioni[i]);
    }
    free(matrice_transizioni);
    return 0;
}
