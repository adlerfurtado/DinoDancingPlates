#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PRATOS 8
#define NUM_HASTES 8

//largura e altura da tela em pixels
const int tela_L = 960;
const int tela_A = 540;

//frames por segundoa
const float FPS = 100;

//altura e largura do player
const float player_L = 50;
const float player_H = 100;

//distancia e coordenadas das hastes
const float entre_hastes = 105;
const float largura_hastes = 8;
const float haste0x = 100;

//struct para definir o player
typedef struct
{
    float x; //posição do jogador
    int equilibrando;
    int esq, dir;
    float velocidade;
} player;

//struct para definir o prato (utilizada para estabilidade)
typedef struct
{
    float estabilidade;
} pratos;

//struct para definir a haste (utilizada para equilibrio)
typedef struct
{
    int colorida;
} haste;

//colore a tela de verde
void desenha_tela()
{
    ALLEGRO_COLOR BKG_COLOR = al_map_rgb(0, 51, 25); //pinta a tela de verde
    al_clear_to_color(BKG_COLOR);
}

//função que desenha o score na tela
void conta_score(ALLEGRO_TIMER *timer)
{
    char texto[20];
    int score = al_get_timer_count(timer) / FPS;
    sprintf(texto, "SCORE %d", score); //associa o tempo a uma string que vai ser desenhada

    ALLEGRO_FONT* fonte = al_create_builtin_font(); //função do allegro que cria um ponteiro que aponta para uma fonte embutida na biblioteca
    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255); //o score vai ficar da cor branca

    al_draw_textf(fonte, cor_texto,  //desenha o a string texto com o ponteiro fonte e a variavel cor_texto
                  10, 10, ALLEGRO_ALIGN_LEFT, //coordenadas, ALLEGRO_ALIGN_LEFT serve para alinhar o texto na esquerda(o ponto x,y fica na esquerda do texto)
                  texto);
    al_destroy_font(fonte);
}

//função que inicializa o player e escolhe a cor
void inicializa_player(player *j)
{
    j->x = tela_L / 2; //posiciona o jogador no centro da tela
    j->equilibrando = 0;
    j->esq = 0;
    j->dir = 0;
    j->velocidade = 2;
}

//função que define o desenho e as coordenadas do player
void desenha_player(player j, ALLEGRO_BITMAP *imagem_player)
{
    al_draw_bitmap(imagem_player, //imagem que vai ser utilizada no player
                   j.x - al_get_bitmap_width(imagem_player) / 2, //centraliza a imagem na posição horizontal
                   tela_A - player_H, 0); //posição vertical do desenho
}

//função que define os movimentos do player e atualiza
void movimenta_player(player *j, ALLEGRO_BITMAP *imagem_player, haste *h)
{
    int i = 0;
    if (j->esq == 1) {
        if (j->x - j->velocidade > 0 + al_get_bitmap_width(imagem_player) / 2 ) //impede de sair da borda esquerda da tela
            j->x -= j->velocidade;//subtrai a velocidade de x fazendo andar pixels para a esquerda
            for(i=0;i<NUM_HASTES;i++){
                if(h[i].colorida==1){
                        h[i].colorida=0; //remove o bug da cor dps que mexe
                }
            }
    }
    if (j->dir == 1) {
        if (j->x + j->velocidade < tela_L - al_get_bitmap_width(imagem_player) / 2) //impede de sair da borda direita da tela
            j->x += j->velocidade; //soma a velocidade de x fazendo andar pixels pra direita
            for(i=0;i<NUM_HASTES;i++){
                if(h[i].colorida==1){
                        h[i].colorida=0;
                }
            }
    }
}

//função que define o tempo de inicialização dos pratos
int tempo_inicializacao(int num)
{
    int base[]={27, 19, 10, 5, 7, 12, 17, 25}; //os pratos do meio aparecerão em menos tempo base(mais rapido)
    return base[num]+rand()%6; //soma a base+ um numero aleatorio entre 0 e 5 para ficar mais dinamico
}

//função que desenha os pratos
int desenha_prato(ALLEGRO_TIMER *timer, float tempo_pratos[],pratos *p, ALLEGRO_BITMAP *imagem_final, int jogando,int ranking)
{
    int i = 0;
    for (i = 0; i < NUM_PRATOS; i++)
    {
        if (al_get_timer_count(timer) >= tempo_pratos[i] * 100) //se o timer for maior ou igual ao tempo de inicialização, o prato é desenhado
        {
            float raio = 25; //raio do circulo
            float x = haste0x + (entre_hastes * i) +5; //coordenada x do circulo
            float y = player_H - 25 ; //coordenada y do circulo

            int vermelho = 255; //255 (total de red) dividido por 100 pois o jogo roda em 100 fps para ficar gradual a mudança de cor
            int verde = p[i].estabilidade * 255/100;
            int azul = p[i].estabilidade * 255/100;
            ALLEGRO_COLOR cor_pratos = al_map_rgba(vermelho, verde, azul, 255); //cor dinâmica que muda conforme a estabilidade



            if(p[i].estabilidade>0)
            {
                p[i].estabilidade= p[i].estabilidade - 0.1; //taxa de decremento
                al_draw_filled_circle(x, y, raio, cor_pratos);
            }

            else if(p[i].estabilidade<=0)
            {
                int j = 0;
                al_stop_timer(timer);
                for(j=0; j < 540 - (2 * player_H); j+=10)
                {
                        int y2 = player_H - 25 + j;
                        al_draw_bitmap(imagem_final, 0, 0, 0);
                        conta_score(timer);
                        al_draw_filled_circle(x, y2, raio, cor_pratos);

                        al_flip_display();
                        al_rest(0.1);

                        p[i].estabilidade = 0;
                        ranking = 1;
                }
                return 0;
            }

        }
    }
}

//função que inicializ as hastes
void desenha_hastes(haste *h)
{
    int i = 0;
    for (i = 0; i < NUM_HASTES; i++)
        {
            if(h[i].colorida==0)
            {
                al_draw_filled_rectangle(haste0x + entre_hastes * i, //coordenada x inicial do retangulo
                                         player_H, //coordenada y inicial do retangulo
                                         haste0x + largura_hastes + entre_hastes * i, //coordenada x final do retangulo
                                         tela_A - player_H, //coordenada y final do retangulo
                                         al_map_rgb(153, 76, 0)); //pinta de marrom
            }else if(h[i].colorida==1) //pinta de verde a haste quando ela esta sendo equilibrada
             {
                 al_draw_filled_rectangle(haste0x + entre_hastes * i,
                                          player_H,
                                          haste0x + largura_hastes + entre_hastes * i,
                                          tela_A - player_H,
                                          al_map_rgb(0, 255, 0));
             }
    }
}


//função que anota o recorde em um arquivo .txt e desenha ele na tela ao final do jogo
void recorde(long long int score) {
    int recorde;
    char n_recorde[20] = "NOVO RECORDE:";
    char r_recorde[20] = "RECORDE: ";
    char s_score[20] = "SEU SCORE: ";
    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255);
    ALLEGRO_FONT* fonte = al_create_builtin_font();

    FILE *fp;
    fp = fopen("recorde.txt", "r");
    fscanf(fp, "%d", &recorde);
    fclose(fp);

    if (score >= recorde) {
        fp = fopen("recorde.txt", "w");
        fprintf(fp, "%lld", score);
        fclose(fp);

        al_draw_textf(fonte, cor_texto,
                  tela_L/2, tela_A/2, ALLEGRO_ALIGN_CENTER,
                  n_recorde);

        char score_str[20];
        sprintf(score_str, "%lld", score);
        al_draw_textf(fonte, cor_texto,
                  tela_L/2, tela_A/2 + 32, ALLEGRO_ALIGN_CENTER,
                  score_str);


    } else {
        al_draw_textf(fonte, cor_texto,
                  tela_L/2 - 50 , tela_A/2, ALLEGRO_ALIGN_CENTER,
                  r_recorde);

        char recorde_str[20];
        sprintf(recorde_str, "%d", recorde);
        al_draw_textf(fonte, cor_texto,
                  tela_L/2 - 50 , tela_A/2 + 32, ALLEGRO_ALIGN_CENTER,
                  recorde_str);

        al_draw_textf(fonte, cor_texto,
                  tela_L/2 + 50 , tela_A/2, ALLEGRO_ALIGN_CENTER,
                  s_score);

        char score_str[20];
        sprintf(score_str, "%lld", score);
        al_draw_textf(fonte, cor_texto,
                  tela_L/2 + 50, tela_A/2 + 32, ALLEGRO_ALIGN_CENTER,
                  score_str);


    }
    al_destroy_font(fonte);
}

int main()
{

    ALLEGRO_DISPLAY *tela = NULL; //inicializa ponteiro da tela
    ALLEGRO_TIMER *timer = NULL; //inicializa o ponteiro timer
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL; //inicializa ponteiro fila de eventos
    srand(time(NULL)); //inicializa gerador de números aleatórios


    //inicializações de rotina do allegro
    if (!al_init()) //inicializa o allegro
        {
        fprintf(stderr, "falha ao iniciar allegro\n");
        return -1;
    }

    if (!al_init_primitives_addon()) //inicializa as primitivas do allegro
        {
        fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }

    if (!al_init_image_addon()) //inicializa o módulo de imagens
        {
        fprintf(stderr, "failed to initialize image module!\n");
        return -1;
    }

    if (!al_init_ttf_addon()) //inicializa primitvas de fonte
    {
        fprintf(stderr, "falha ao inicializar o módulo TrueType Font!\n");
        return -1;
    }


    //inicializando as structs
    player j;
    pratos p[NUM_PRATOS];
    haste h[NUM_HASTES];
    inicializa_player(&j);

    //variaveis necessarias
    int jogando = 1;
    int ranking = 0;
    int i = 0;
    for(i=0; i<NUM_HASTES; i++){
            h[i].colorida = 0;
    }
    i=0; //reinicia o i para = 0 após ele ser usado no for


    //cria a tela
    tela = al_create_display(tela_L, tela_A);
    if (!tela)
        {
        fprintf(stderr, "falha ao criar tela!\n");
        return -1;
    }

    //cria um timer de 1.0 a cada frame por segundo
    timer = al_create_timer(1.0 / FPS);
    if (!timer)
        {
        fprintf(stderr, "falha ao criar timer!\n");
        return -1;
    }

    //cria fila de eventos
    fila_eventos = al_create_event_queue();
    if (!fila_eventos)
        {
        fprintf(stderr, "falha ao criar fila de eventos!\n");
        al_destroy_display(tela);
        return -1;
    }

    //instala o teclado
    if (!al_install_keyboard())
        {
        fprintf(stderr, "falha ao instalar teclado!\n");
        return -1;
    }

    //registra na fila os eventos de tela
    al_register_event_source(fila_eventos, al_get_display_event_source(tela));
    //registra na fila os eventos de timer
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    //registra na fila os eventos de teclado
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());


    //associa o tempo de inicialização dos pratos a um array pratos e faz com que a estabilidade inicial de cada prato seja 100;
    float tempo_pratos[NUM_PRATOS];
    for (i = 0; i<NUM_PRATOS; i++)
        {
            tempo_pratos[i] = tempo_inicializacao(i);
            p[i].estabilidade = 100; //estabilidade de cada prato definida em 100
    }

    //cria um ponteiro e associa à imagem do player
    ALLEGRO_BITMAP *imagem_player = NULL;
    imagem_player = al_load_bitmap("dino.png");
    if (!imagem_player)
        {
        fprintf(stderr, "falha ao carregar imagem do player!\n");
        return -1;
    }

    //cria um ponteiro e associa à imagem do final
    ALLEGRO_BITMAP *imagem_final = NULL;
    imagem_final = al_load_bitmap("final.jpg");
    if(!imagem_final)
        {
        fprintf(stderr, "falha ao carregar imagem do final!\n");
        return -1;
    }

    //inicia o timer
    al_start_timer(timer);


    //inicia o loop que roda a fila de eventos
    while (jogando)
        {
        //espera até que ocorra um evento
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        //a cada atualizção do timer
        if (evento.type == ALLEGRO_EVENT_TIMER)
            {

            //starta as funções de desenho e movimentação
            desenha_tela();
            conta_score(timer);
            movimenta_player(&j, imagem_player,h);
            desenha_player(j, imagem_player);
            desenha_hastes(h);
            int fim = desenha_prato(timer, tempo_pratos, p, imagem_final,jogando,ranking); //desenha o prato e associa ele a uma variavel para retornar 0 ou 1
            if (fim == 0) // se a função retornar 0 significa que o prato caiu e o loop playing é encerrado
                {
                ranking = 1;
                jogando = 0;
                al_flip_display();
            }

            //atualiza a tela de acordo com o fps
            al_flip_display();

            //print da contagem de segundos
            if (al_get_timer_count(timer) % (int)FPS == 0)
                {
                printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer) / FPS));
            }
        }

        //apertar A D ou SPACE respectivamente vai pra esquerda, vai pra direita e equilibra
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                if (evento.keyboard.keycode == ALLEGRO_KEY_A)
                {
                    j.esq = 1;
                    j.equilibrando = 0; //se movimentar para com a ação de equilibrar
                }else if (evento.keyboard.keycode == ALLEGRO_KEY_D)
                {
                    j.dir = 1;
                    j.equilibrando = 0; //se movimentar para com a ação de equilibrar
                }else if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE)
                {
                    j.equilibrando = 1;
                    j.esq = 0;
                    j.dir = 0;  //equilibrar faz parar de andar
                }
        }

        //soltar A D ou SPACE para de realizar as ações
        else if (evento.type == ALLEGRO_EVENT_KEY_UP)
            {
                if (evento.keyboard.keycode == ALLEGRO_KEY_A)
                {
                    j.esq = 0;
                }else if (evento.keyboard.keycode == ALLEGRO_KEY_D)
                {
                    j.dir = 0;
                }else if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE)
                {
                    j.equilibrando = 0;
                    for(i=0; i<NUM_HASTES; i++)
                    {
                            h[i].colorida = 0;
                    }
                }
        }

        //se o evento for clicar no X fecha o jogo
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
            jogando = 0; //fecha o loop principal
            ranking = 0; //reforçando que se apertar X nao aparecerá a tela de recorde
        }

        //mecanica para realizar o equilibrio dos pratos se debaixo dele
        for(i=0;i<NUM_PRATOS;i++)
            {
            if (j.equilibrando == 1) //se esta debaixo do prato apertando espaço a estabilidade aumenta e colore as hastes
            {
                    if(j.x >= haste0x + entre_hastes * i - 8  && j.x <= haste0x + largura_hastes + entre_hastes * i ){ //se o player esta entre o inicio e o final das hastes (-8 para ficar mais natural com o dino.png)
                            p[i].estabilidade += 0.7;
                            if(p[i].estabilidade > 100) //nao deixa estabilizar mais que 100
                            {
                                p[i].estabilidade = 100;
                            }
                            h[i].colorida = 1;
                    }
            }
        }

    }

    //inicia tela de recordes após o fim do jogo se ranking = 1
    if(ranking)
    {
        long long int score = al_get_timer_count(timer) / FPS;

        //inicializa outro evento genérico
        ALLEGRO_EVENT evento;

        while(ranking){

                //printa o recorde na tela e atualiza
                recorde(score);
                al_flip_display();

                //se o evento for clicar no X o jogo fecha
                al_wait_for_event(fila_eventos, &evento);

                if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                    {
                        ranking = 0;

                }

        }

    }

    //rotinas de destruição e liberação de memória
    al_destroy_timer(timer);
    al_destroy_display(tela);
    al_destroy_event_queue(fila_eventos);
    al_destroy_bitmap(imagem_player);
    al_destroy_bitmap(imagem_final);


    return 0;
}
