#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "gfx.h"


//construcao
struct s_no{
    int32_t chave:28;
    int32_t bal:2;
    int32_t reservado:2;
    struct s_no *esq;
    struct s_no *dir;
};

struct s_arq_no{
    int32_t chave:28;
    int32_t bal:2;
    uint32_t esq:1;
    uint32_t dir:1;
};

//alocacao do novo no
void nvno(struct s_no **pt, int32_t val){
    (*pt) = (struct s_no*) malloc(sizeof(struct s_no));
    (*pt) -> esq = NULL, (*pt) -> dir = NULL;
    (*pt) -> chave = val, (*pt) -> bal = 0, (*pt) -> reservado = 0;
}
//busca
void busc(struct s_no* raiz, int chave){
    if (raiz->chave == chave){
        printf("VALOR '%d' ENCONTRADO\n\n", chave);
        return;
    }
    if (raiz->chave < chave){
        if (raiz->dir != NULL){
            busc(raiz->dir, chave);
            return;
        }else{
            printf("VALOR '%d' NAO ENCONTRADO\n\n", chave);
            return;
        }
    }
    if(raiz->esq != NULL){
        busc(raiz->esq, chave);
        return;
    }else{
        printf("VALOR '%d' NAO ENCONTRADO\n\n", chave);
        return;
    }
    
}
//printa a arvore em preordem
void most(struct s_no *raiz, unsigned short xi, unsigned short xf, unsigned short y){
    char str[64];
    unsigned short xm;
    if (raiz == NULL){
        return; 
    }
    //printf("%i (%i)  ", raiz->chave, raiz->bal); //pra printar em preordem da forma usual

    gfx_set_color(255,255,255);
    xm=(xi+xf)/2;
    
    snprintf(str,64,"%i",raiz->chave);
    gfx_text(xm, y, str);
    gfx_set_color(0,255,0);
    snprintf(str,64,"%i",raiz->bal);
    gfx_text(xm+10, y-30, str);
    
    if(raiz->esq!=NULL){
        gfx_set_color(0,0,255);
        gfx_line(xm, y, (xi+xm)/2, y+70);
    }    
    if(raiz->dir!=NULL){
        gfx_set_color(0,0,255);
        gfx_line(xm, y, (xf+xm)/2, y+70);
    }
    most(raiz->esq, xi, xm, y+70);
    most(raiz->dir, xm, xf, y+70); 
}
//rebalanceamento caso 1 (com base no algoritmo dos slides)
void caso1(struct s_no** pt, bool* h){
    struct s_no* ptu = (*pt) -> esq;
    if(ptu -> bal == -1) {
        (*pt) -> esq = ptu -> dir;
        ptu -> dir = (*pt);
        (*pt) -> bal = 0;
        (*pt) = ptu;
    } else {
        struct s_no* ptv = ptu -> dir;
        ptu -> dir = ptv -> esq;
        ptv -> esq = ptu;
        (*pt) -> esq = ptv -> dir;
        ptv -> dir = (*pt);

        if(ptv -> bal == -1) (*pt) -> bal = 1;
        else (*pt) -> bal = 0;

        if(ptv -> bal == 1) ptu -> bal = -1;
        else ptu -> bal = 0;
        (*pt) = ptv;
    }
    (*pt) -> bal = 0;
    *h = false;
}
//rebalanceamento caso 2 (com base no algoritmo dos slides)
void caso2(struct s_no** pt, bool *h){
    struct s_no* ptu = (*pt) -> dir;
    if(ptu -> bal == 1) {
        (*pt) -> dir = ptu -> esq;
        ptu -> esq = (*pt);
        (*pt) -> bal = 0;
        (*pt) = ptu;
    } else {
        struct s_no* ptv = ptu -> esq;
        ptu -> esq = ptv -> dir;
        ptv -> dir = ptu;
        (*pt) -> dir = ptv -> esq;
        ptv -> esq = (*pt);

        if(ptv -> bal == 1) (*pt) -> bal = -1;
        else (*pt) -> bal = 0;

        if(ptv -> bal == -1) ptu -> bal = 1;
        else ptu -> bal = 0;
        (*pt) = ptv;
    }
    (*pt) -> bal = 0;
    *h = false;
}
//insercao com base no algoritmo dos slides
void ins(int32_t val, struct s_no** pt, bool* h){
    if(*pt == NULL) {
        nvno(&(*pt), val);
        *h = true;
    } else if(val == (*pt) -> chave) {
        printf("CHAVE JA EXISTENTE\n");
        return ;
    } else {
        if(val < (*pt) -> chave) {
            ins(val, &(*pt) -> esq, h);
            if(*h) {
                switch ((*pt) -> bal) {
                    case 1:;
                        (*pt) -> bal = 0;
                        *h = false;
                        break;
                    case 0:;
                        (*pt) -> bal = -1;
                        break;
                    case -1:;
                        caso1(&(*pt), h);
                        break;
                }
            }
        } else {
            ins(val, &(*pt) -> dir, h);
            if(*h) {
                switch ((*pt) -> bal) {
                    case -1:;
                        (*pt) -> bal = 0;
                        *h = false;
                        break;
                    case 0:;
                        (*pt) -> bal = 1;
                        break;
                    case 1:;
                        caso2(&(*pt), h);
                        break;
                }
            }
        }
    }
}
//rebalanceamento caso 1 (com base no algoritmo de insercao dos slides)
void rcaso1(struct s_no** pt, bool* h){
    struct s_no* ptu = (*pt)->esq;
    if(ptu->bal < 1){
        (*pt)->esq = ptu->dir;
        ptu->dir = (*pt);

        if(ptu->bal == 0){
            ptu->bal = 1;
            *h = false;
        }else{
            ptu->bal = 0;
        }
        (*pt) = ptu;
    }else{
        struct s_no* ptv = ptu->dir;
        ptu->dir = ptv->esq;
        ptv->esq = ptu;
        (*pt)->esq = ptv->dir;
        ptv->dir = (*pt);

        if(ptv->bal == -1) {
            (*pt)->bal = 1;
        }else{
            (*pt)->bal = 0;
        }

        if(ptv->bal == 1) {
            ptu->bal = -1;
        }else{
            ptu->bal = 0;
        }
        (*pt) = ptv;
        ptv->bal = 0;
    }
}
//rebalanceamento caso 2 (com base no algoritmo de insercao dos slides)
void rcaso2(struct s_no** pt, bool *h){
    struct s_no* ptu = (*pt) -> dir;
    
    if(ptu->bal > -1){
		(*pt)->dir = ptu->esq;
		ptu->esq = (*pt);

		if(ptu->bal == 0){
		   ptu->bal = -1;
		   *h = false;
		}else{
		   ptu->bal = 0;
		}
		(*pt)->bal = 0;
		(*pt) = ptu;
	}else{
		struct s_no* ptv = ptu->esq;
		ptu->esq = ptv->dir;
		ptv->dir = ptu;
		(*pt)->dir = ptv->esq;
		ptv->esq = (*pt);

		if (ptv->bal == 1) {
		   (*pt)->bal = -1;
		}else{
		   (*pt)->bal = 0;
		}

		if (ptv->bal == -1) {
		   ptu->bal = 1;
		}else{
		   ptu->bal = 0;
		}
		(*pt) = ptv;
		ptv->bal = 0;
	}
}
//funcao auxiliar pra remocao
void prox(struct s_no **pt, struct s_no **temp, bool *h){
    if ((*temp)->esq == NULL){
        (*pt)->chave = (*temp)->chave;
        *pt = *temp;
        *temp = (*temp)->dir;
        *h = true;
    }else{
        prox(pt, &(*temp)->esq, h);
        if (*h){
            switch((*temp)->bal){
                case -1:;
                    (*temp)->bal = 0;
                    break;
                case 0:;
                    (*temp)->bal = 1;
                    *h = false;
                    break;
                case 1:;
                    rcaso2(temp, h);
                    break;
            }
        }
    }
}
//remocao com base no algoritmo de insercao dos slides
void rem(struct s_no** pt, int32_t val, bool *h){
    struct s_no *temp = NULL;

    if((*pt) != NULL){
        if(val == (*pt)->chave){
            temp = *pt;
            if(((*pt)->esq != NULL) && ((*pt)->dir != NULL)){
                prox(&temp, &temp->dir, h);
                if (*h){
                    switch ((*pt)->bal){
                        case 1:;
                            (*pt)->bal = 0;
                            break;
                        case 0:;
                            (*pt)->bal = -1;
                            *h = false;
                            break;
                        case -1:;
                            rcaso1(pt, h);
                            break;
                    }
                }
            }else{
                if(temp->esq == NULL){
                    *pt = temp->dir;
                    *h = true;
                }else{
                    *pt = temp->esq;
                    *h = true;
                }
            }
            free(temp);
        }else{
            if (val < (*pt)->chave){
                rem(&((*pt)->esq), val, h);
                if (*h){
                    switch ((*pt)->bal){
                        case 1:;
                            rcaso2(pt, h);
                            break;
                        case 0:;
                            (*pt)->bal = 1;
                            *h = false;
                            break;
                        case -1:;
                            (*pt)->bal = 0;
                            break;
                    }
                }
            }else{
                rem(&((*pt)->dir), val, h);
                if (*h){
                    switch ((*pt)->bal) {
                        case 1:;
                            (*pt)->bal = 0;
                            break;
                        case 0:;
                            (*pt)->bal = -1;
                            *h = false;
                            break;
                        case -1:;
                            rcaso1(pt, h);
                            break;
                    }
                }
            }
        }
    }
}
//desalocacao
void desaloc(struct s_no* raiz){ 
    if (raiz == NULL){
        return; 
    }
    desaloc(raiz->esq);
    desaloc(raiz->dir);
    free(raiz);
}
//escrita da arvore em arquivo binario usando a estrutura especificada
void warq(struct s_no* raiz, struct s_arq_no *arqr, FILE* fp_bin){
    if(raiz==NULL){
        return;
    }
    arqr = (struct s_arq_no *)malloc(sizeof(struct s_arq_no));
    arqr->chave=raiz->chave;
    arqr->bal=raiz->bal;
    if(raiz->esq==NULL){
        arqr->esq=0;
    }else{
        arqr->esq=1;
    }
    if(raiz->dir==NULL){
        arqr->dir=0;
    }else{
        arqr->dir=1;
    }
    fwrite(arqr, sizeof(struct s_arq_no), 1, fp_bin);
    warq(raiz->esq, arqr, fp_bin);
    warq(raiz->dir, arqr, fp_bin);
}
//leitura da arvore em arquivo binario usando a estrutura especificada
struct s_no* rarq(struct s_no* no, FILE* fl_bin){
    struct s_arq_no arq;
    fread(&arq, sizeof(arq), 1, fl_bin);
    if(!feof(fl_bin)){
        no = (struct s_no *)malloc(sizeof(struct s_no));
        no->chave = arq.chave;
        no->bal = arq.bal;
        no->reservado = 0;
        if(arq.esq==1){
            no->esq = rarq(no->esq, fl_bin);
        }else{
            no->esq = NULL;
        }
        if(arq.dir==1){
            no->dir = rarq(no->dir, fl_bin);
        }else{
            no->dir = NULL;
        }
    }
    return no;
}

int main(){
    int menu, bus, nv;
    unsigned long lsize;
    char nome[20];
    struct s_no *raiz = NULL;
    struct s_arq_no *arqr = NULL;
    FILE* fp_bin;
    FILE* fl_bin;

    printf("ARVORE AVL\n\n");
    do{
        bool h = false;
        printf("ESCOLHA A OPERACAO\n");
        printf("0- IMPRESSAO NO GFX\n");
        printf("1- BUSCA\n");
        printf("2- INSERCAO\n");
        printf("3- REMOCAO\n");
        printf("4- ESCRITA EM ARQUIVO BINARIO\n");
        printf("5- LEITURA DE ARQUIVO BINARIO (ARVORE ATUAL SERA APAGADA)\n");
        printf("6- FINALIZAR\n");

        scanf("%d", &menu);
        switch(menu){
            case 0:;
                if (raiz == NULL){
                    printf("\nARVORE VAZIA\n\n");
                }else{
                    printf("\n");
                    gfx_init(1366, 768, "ARVORE AVL");
                    gfx_set_font_size(30);
                    gfx_clear();
                    most(raiz, 0, 1366, 70);
                    gfx_paint();
                    sleep(10);
                    gfx_quit();
                    printf("\n\n");                    
                }
                break;
            case 1:;
                if(raiz != NULL){
                    printf("DIGITE O VALOR A SER BUSCADO\n");
                    scanf("%d", &bus);
                    busc(raiz, bus);
                }else{
                    printf("\nARVORE VAZIA\n\n");
                }
                break;
            case 2:;
                h=0;
                printf("DIGITE O VALOR A SER INSERIDO\n");
                scanf("%d", &nv);
                ins(nv, &raiz, &h);
                printf("VALOR '%d' INSERIDO OU JA PRESENTE\n\n", nv);
                break;
            case 3:;
                if (raiz == NULL){
                    printf("ARVORE VAZIA\n\n");
                }else{
                    printf("DIGITE O VALOR A SER REMOVIDO\n");
                    scanf("%d", &bus);
                    rem(&raiz, bus, &h);
                    printf("VALOR REMOVIDO OU NAO ENCONTRADO\n\n");
                }
                break;
            case 4:;
                if (raiz == NULL){
                    printf("ARVORE VAZIA\n\n");
                }else{
                    printf("\nDIGITE O NOME QUE O ARQUIVO BINARIO TERA (COM A EXTENSAO):\n");
                    scanf("%s", nome);
                    fp_bin = fopen(nome, "wb");
                    warq(raiz, arqr, fp_bin);
                    printf("\nARVORE ESCRITA EM ARQUIVO\n\n");
                    fclose(fp_bin);
                }
                break;
            case 5:;
                desaloc(raiz);
                printf("\nDIGITE O NOME DO ARQUIVO (COM A EXTENSAO):\n");
                scanf("%s", nome);
                fl_bin = fopen(nome, "rb");
                if (fl_bin==NULL){
                    printf("ARQUIVO NAO ENCONTRADO\n\n");
                }else{
                    raiz = (struct s_no*)malloc(sizeof(struct s_no));
                    fseek (fl_bin , 0 , SEEK_END);
                    lsize = ftell (fl_bin);
                    rewind (fl_bin);
                    arqr = (struct s_arq_no*)malloc(sizeof(struct s_arq_no)*lsize);
                    if (arqr == NULL){
                        printf("ERRO DE MEMORIA\n");
                    }else{
                        raiz = rarq(raiz, fl_bin);
                        printf("ARQUIVO LIDO\n\n");
                    }
                }
                fclose(fl_bin);
                break;
            case 6:;
                desaloc(raiz);
                printf("\nARVORE DESALOCADA\n\n"); //eu testei pra ver se realmente foi desalocada, isso e so um aviso de confirmacao
                break;
        }
    }while(menu!=6);
    
    return 0;
}
