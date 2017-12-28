#include <stdint.h>
#include <bitbox.h>
#include <stdlib.h> // rand
#include <string.h>

#define CLEAR_COLOR 0
#define GROUND_Y 273
#define MAX_PIPES 4
#define PERCENTAGE_SPEED 5
#define STATE_DISABLED 0
#define STATE_ENABLED 1
#define STATE_ENABLEDANDCOUNTED 2
#define GAME_STOP 0
#define GAME_RUN 1
#define GAME_PAUSED 2
#define GAME_OVER 3
#define BIRDY_STARTX 300
#define BIRDY_STARTY 150

#define SCORE_POSX 296
#define SCORE_POSY 10

volatile int x,y, snd_vol;

int speedY = 1;
int speedX = 5;

//Variables for test
int linesCount = 0;
int framesCount = 0;
int frCnt_NewPipe = 0;
int flip = 0;
short score_u = 0;
short score_d = 0;
short score_c = 0;
short game_State = 0;
int g_score = 0;
int next_SpeedUp = 5;
int delayToRestart = 0;
int show_GO = 0;


//uint16_t lineToDraw[640];

const uint16_t TRANSPARENT_COLOR=RGB(255,0,255); 

typedef uint16_t Sprite[][3];

typedef struct{
    int x,y;
    int w,h;
    int idxBlit;    
    short actualFrame;
    short state;
    short flip;  
    Sprite *sprite;  
} ACTOR;

typedef struct{
    int x,y;
    int w,h;
    int idxBlit;        
    short flip;    
} STAGE;

int back_2_index;
extern int back_2_w;
extern int back_2_h;
extern Sprite back_2_sprite;

/*Starring*/

int back_1_index;
extern int back_1_w;
extern int back_1_h;
extern Sprite back_1_sprite;

extern int bird_1_w;
extern int bird_1_h;

extern int bird_1_shad_w;
extern int bird_1_shad_h;

extern Sprite bird_1_sprite;
extern Sprite bird_2_sprite;
extern Sprite bird_3_sprite;

int bird_1_shad_index;
int bird_2_shad_index;
int bird_3_shad_index;
extern Sprite bird_1_shad_sprite;
extern Sprite bird_2_shad_sprite;
extern Sprite bird_3_shad_sprite;

/*Numbers */
extern int n_zero_w;
extern int n_zero_h;

int num_w;
int num_h;

int score_u_index = 0;
int score_d_index = 0;
int score_c_index = 0;

extern Sprite n_zero_sprite;
extern Sprite n_one_sprite;
extern Sprite n_two_sprite;
extern Sprite n_three_sprite;
extern Sprite n_four_sprite;
extern Sprite n_five_sprite;
extern Sprite n_six_sprite;
extern Sprite n_seven_sprite;
extern Sprite n_eight_sprite;
extern Sprite n_nine_sprite;

/*tubes */

extern int tube_up_short_h; //tube seeing up
extern int tube_up_short_w;
extern Sprite tube_up_short_sprite;

extern int tube_up_med_h;
extern int tube_up_med_w;
extern Sprite tube_up_med_sprite;

extern int tube_up_long_h;
extern int tube_up_long_w;
extern Sprite tube_up_long_sprite;

extern int tube_down_short_h;
extern int tube_down_short_w;
extern Sprite tube_down_short_sprite;

extern int tube_down_med_h;
extern int tube_down_med_w;
extern Sprite tube_down_med_sprite;

extern int tube_down_long_h;
extern int tube_down_long_w;
extern Sprite tube_down_long_sprite;

extern int tube_up_short_shad_h;
extern int tube_up_short_shad_w;
extern Sprite tube_up_short_shad_sprite;

extern int default_tube_shad_h;
extern int default_tube_shad_w;
extern Sprite default_tube_shad_sprite;

int start_1_index = 0;
extern int start_1_h;
extern int start_1_w;
extern Sprite start_1_sprite;

int go_1_index = 0;
extern int go_1_h;
extern int go_1_w;
extern Sprite go_1_sprite;

int game_over_1_index = 0;
extern int game_over_1_h;
extern int game_over_1_w;
extern Sprite game_over_1_sprite;

ACTOR birdy;
ACTOR tube_u[MAX_PIPES];
ACTOR tube_u_shad[MAX_PIPES];
ACTOR tube_d[MAX_PIPES];
ACTOR tube_d_shad[MAX_PIPES];

void initialize_Actors()
{    
   back_2_index = 0;
   
   birdy.x = 300;
   birdy.y = 150;
   birdy.h = bird_1_h;
   birdy.w = bird_1_w;
   birdy.flip = 0;
   birdy.idxBlit = 0;  
   
   num_w = n_zero_w;
   num_h = n_zero_h;
   
}

// standard AABB collision check
int collide(ACTOR *aa, ACTOR *ab)
{
    if (aa->x+aa->w < ab->x ||
        aa->y+aa->h < ab->y ||
        aa->x > ab->x+ab->w ||
        aa->y > ab->y+ab->h) {
        return 0;
    } else {
    	return 1;
    }
}

void check_BirdyCollide(){
    for(int t_idx = 0; t_idx < MAX_PIPES; t_idx++){
        if(tube_u[t_idx].state != STATE_DISABLED){
            if(collide(&birdy,&tube_u[t_idx])) {
                //birdy.x -=10
                game_State = GAME_STOP;                
                birdy.x = BIRDY_STARTX;
                birdy.y = BIRDY_STARTY;
                //g_score = 0;
            }
                
        }
        if(tube_d[t_idx].state != STATE_DISABLED){
            if(collide(&birdy,&tube_d[t_idx])) {
                game_State = GAME_STOP;                
                birdy.x = BIRDY_STARTX;
                birdy.y = BIRDY_STARTY;
                //g_score = 0;
            }
        }
   }      
}

void clear_Line(){
    //for(int idx = 0; idx < 640; idx++) lineToDraw[idx] = CLEAR_COLOR;
    for(int idx = 0; idx < 640; idx++) draw_buffer[idx] = CLEAR_COLOR;
}

void remove_Pipe(int idxPipe){
    tube_u[idxPipe].state = STATE_DISABLED;
    tube_d[idxPipe].state = STATE_DISABLED;        
}

void new_Pipe(int idxPipe){
    int r = rand() % 3;
    
    switch(r){
        case 0:
        tube_u[idxPipe].w = tube_up_short_w;
        tube_u[idxPipe].h = tube_up_short_h;
        tube_d[idxPipe].w = tube_down_long_w;
        tube_d[idxPipe].h = tube_down_long_h;
        tube_u_shad[idxPipe].w = tube_up_short_shad_w;
        tube_u_shad[idxPipe].h = tube_up_short_shad_h;
        tube_u[idxPipe].actualFrame = 0;
        tube_d[idxPipe].actualFrame = 2;
        tube_u_shad[idxPipe].actualFrame = 0;
        break;
        case 1:
        tube_u[idxPipe].w = tube_up_med_w;
        tube_u[idxPipe].h = tube_up_med_h;
        tube_d[idxPipe].w = tube_down_med_w;
        tube_d[idxPipe].h = tube_down_med_h;
        tube_u_shad[idxPipe].w = default_tube_shad_w;
        tube_u_shad[idxPipe].h = default_tube_shad_h;
        
        tube_u[idxPipe].actualFrame = 1;
        tube_d[idxPipe].actualFrame = 1;
        tube_u_shad[idxPipe].actualFrame = 0;
        break;
        case 2:
        tube_u[idxPipe].w = tube_up_long_w;
        tube_u[idxPipe].h = tube_up_long_h;
        tube_d[idxPipe].w = tube_down_short_w;
        tube_d[idxPipe].h = tube_down_short_h;        
        tube_u_shad[idxPipe].w = default_tube_shad_w;
        tube_u_shad[idxPipe].h = default_tube_shad_h;
        
        tube_u[idxPipe].actualFrame = 2;
        tube_d[idxPipe].actualFrame = 0;        
        tube_u_shad[idxPipe].actualFrame = 0;
        break;
        
    }    
    tube_u[idxPipe].x = VGA_H_PIXELS;// - tube_u[idxPipe].w;
    tube_u[idxPipe].y = GROUND_Y - tube_u[idxPipe].h;
    tube_u[idxPipe].state = STATE_ENABLED;    
    
    tube_d[idxPipe].x = VGA_H_PIXELS;// - tube_d[idxPipe].w;
    tube_d[idxPipe].y = 0;
    tube_d[idxPipe].state = STATE_ENABLED;   
    
    tube_u_shad[idxPipe].x = VGA_H_PIXELS;// - tube_u_shad[idxPipe].w;
    tube_u_shad[idxPipe].y = 0;
    tube_u_shad[idxPipe].state = STATE_ENABLED;   

}

void fill_Pipes(){
    for(int t_idx = 0; t_idx < MAX_PIPES; t_idx++){
        if(tube_u[t_idx].state == STATE_DISABLED) {
            new_Pipe(t_idx);
            return;
        }
    }
        
}

void move_Pipes(){
    for(int t_idx = 0; t_idx < MAX_PIPES; t_idx++){
        if(tube_u[t_idx].state != STATE_DISABLED){            
            tube_u[t_idx].x-= 2;
            if((tube_u[t_idx].x + tube_u[t_idx].w) < 0) remove_Pipe(t_idx);
        }
        if(tube_d[t_idx].state != STATE_DISABLED){            
            tube_d[t_idx].x-= 2;
            //if((tube_d[t_idx].x + tube_d[t_idx].w) < 0) remove_Pipe(idx);
        }
   }    
}

void count_Pipes(){    
    for(int t_idx = 0; t_idx < MAX_PIPES; t_idx++){
        if(tube_u[t_idx].state == STATE_ENABLED){                        
            if((tube_u[t_idx].x + tube_u[t_idx].w) < birdy.x)
            { 
                tube_u[t_idx].state = STATE_ENABLEDANDCOUNTED;
                g_score++;
            }
        }        
    }    
    score_u = g_score % 10;   
    score_d = g_score / 10;    
}


void game_init() {
    initialize_Actors();
    back_2_index = 0; 
    
    for(int idx = 0; idx < MAX_PIPES; idx++) 
    {
        tube_u[idx].state = STATE_DISABLED;
        tube_d[idx].state = STATE_DISABLED;        
    }   
    
    //fill_Pipes();
    
}

void game_frame()
{
    if (GAMEPAD_PRESSED(0,up)) birdy.y -= 2;
    if (GAMEPAD_PRESSED(0,down)){
         if(birdy.y+birdy.h < GROUND_Y) birdy.y += 2;
     }
    if (GAMEPAD_PRESSED(0,left)) 
    {
        birdy.flip = 1;
        birdy.x -= 2;
    }
        
    if (GAMEPAD_PRESSED(0,right)) 
    {
        birdy.flip = 0;
        birdy.x += 2;
    }
    
    if (GAMEPAD_PRESSED(0,start)){
        if(game_State == GAME_STOP || game_State == GAME_PAUSED)
        {
            game_State = GAME_RUN;   
            g_score = 0;  
            show_GO = 0;   
            delayToRestart = 200;
        }
        //else if(game_State == GAME_RUN) game_State = GAME_PAUSED;
    
    }    
    
    if(framesCount < 5) framesCount++;
    else {        
        framesCount = 0;                
        if(birdy.actualFrame == 0) birdy.actualFrame = 1;
        else if(birdy.actualFrame == 1) birdy.actualFrame = 2;
        else if(birdy.actualFrame == 2) birdy.actualFrame = 0;                
        count_Pipes();        
    }     
    
    if(game_State == GAME_STOP){
        for(int idx = 0; idx < MAX_PIPES; idx++) 
        {
            remove_Pipe(idx);
        }
        delayToRestart--;
        if(delayToRestart < 1){
            
        }
    }
    
    if(game_State == GAME_RUN){
    if(show_GO < 50) show_GO++;
    
    move_Pipes(); 
    
    check_BirdyCollide();    
    
   /* for(int t_idx = 0; t_idx < MAX_PIPES; t_idx++){
        if(tube_u[t_idx].state != STATE_DISABLED){
            switch(tube_u[t_idx].actualFrame){
                case 0:
                    tube_u[t_idx].sprite = tube_up_short_sprite;                         
                break;                
                case 1:
                    tube_u[t_idx].sprite = tube_up_med_sprite;                         
                break;
                case 2:
                    tube_u[t_idx].sprite = tube_up_long_sprite; 
                break;                
            }
        }
    }*/
    
    if(frCnt_NewPipe < 100  ) frCnt_NewPipe++;
    else
    {
        frCnt_NewPipe = 0;
        fill_Pipes();          
    }
    }  
} 

int blit_TT(Sprite sprite, int x, int y, int w, int h, int flip, int blit_index){
    if(flip) x += w;
    
    if(vga_line > y && vga_line <= y + h)
    {
        for(int drawX = 0; drawX < w; )
        {            
             uint16_t c1 = sprite[blit_index][0]; //color pix 1
             uint16_t c2 = sprite[blit_index][1]; //color pix 2
             int nb = sprite[blit_index][2];// cantidad de repeticiones                                                  
                    
             for (int i=0;i<nb;i++)
		     {                  
                 //if((x+drawX)>0){ 
                 if(flip)
                 {
                     if(c1 != TRANSPARENT_COLOR) draw_buffer[x-drawX]=c1;
                 }else
                 {
                     if(c1 != TRANSPARENT_COLOR) draw_buffer[x+drawX]=c1;
                 }
                 //}
                 drawX++;
                 
                 //if((x+drawX)>0){ 
                 if(flip)
                 {
                     if(c2 != TRANSPARENT_COLOR) draw_buffer[x-drawX]=c2;
                 }
                 else 
                 {   
                     if(c1 != TRANSPARENT_COLOR) draw_buffer[x+drawX]=c1;
                 }
                 //}
                 drawX++;
             }
             blit_index += 1;
        }         
    }
    return blit_index;
}


void graph_vsync(void) 
{
    // Do all on same line
	//if (vga_line != VGA_V_PIXELS+3) return;
    back_1_index = 0;
    back_2_index = 0;               
    birdy.idxBlit = 0;    
    bird_1_shad_index = 0;
    bird_2_shad_index = 0;
    bird_3_shad_index = 0;
    score_u_index = 0;
    score_d_index = 0;
    score_c_index = 0;
    start_1_index = 0;
    game_over_1_index = 0;
    go_1_index = 0;
       
    for(int t_idx = 0; t_idx < MAX_PIPES; t_idx++){
        if(tube_u[t_idx].state != STATE_DISABLED){
            tube_u[t_idx].idxBlit = 0;
            tube_d[t_idx].idxBlit = 0;
            tube_u_shad[t_idx].idxBlit = 0;
        }        
    }      
}

void update_Score(){      
    
    switch(score_u){
        case 0:
            score_u_index = blit_TT(n_zero_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 1:
            score_u_index = blit_TT(n_one_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 2:
            score_u_index = blit_TT(n_two_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 3:
            score_u_index = blit_TT(n_three_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 4:
            score_u_index = blit_TT(n_four_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 5:
            score_u_index = blit_TT(n_five_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 6:
            score_u_index = blit_TT(n_six_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 7:
            score_u_index = blit_TT(n_seven_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 8:
            score_u_index = blit_TT(n_eight_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
        case 9:
            score_u_index = blit_TT(n_nine_sprite, SCORE_POSX + num_w, SCORE_POSY, num_w, num_h, 0, score_u_index);            
        break;
    }
    
    switch(score_d){
        case 0:
            score_d_index = blit_TT(n_zero_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 1:
            score_d_index = blit_TT(n_one_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 2:
            score_d_index = blit_TT(n_two_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 3:
            score_d_index = blit_TT(n_three_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 4:
            score_d_index = blit_TT(n_four_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 5:
            score_d_index = blit_TT(n_five_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 6:
            score_d_index = blit_TT(n_six_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 7:
            score_d_index = blit_TT(n_seven_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 8:
            score_d_index = blit_TT(n_eight_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
        case 9:
            score_d_index = blit_TT(n_nine_sprite, SCORE_POSX, SCORE_POSY, num_w, num_h, 0, score_d_index);            
        break;
    }
}

void graph_line()
{
    //clear_Line();    
    back_1_index = blit_TT(back_1_sprite,0,0,back_1_w, back_1_h,0,back_1_index);    
    
    if(game_State == GAME_STOP && delayToRestart < 1){
        start_1_index = blit_TT(start_1_sprite,188,80,start_1_w,start_1_h,0,start_1_index);
    }else if(game_State == GAME_STOP && delayToRestart > 1){
        game_over_1_index = blit_TT(game_over_1_sprite,268,80,game_over_1_w,game_over_1_h,0,game_over_1_index);
    }
    if(game_State == GAME_RUN && show_GO < 50){
        go_1_index = blit_TT(go_1_sprite,284,80,go_1_w,go_1_h,0,go_1_index);
    }
    
    
    for(int t_idx = 0; t_idx < MAX_PIPES; t_idx++){
        if(tube_u[t_idx].state != STATE_DISABLED){
            //tube_u[t_idx].idxBlit = blit_TT(tube_u[t_idx].sprite,tube_u[t_idx].x,tube_u[t_idx].y,
            //                            tube_u[t_idx].w, tube_u[t_idx].h,0,tube_u[t_idx].idxBlit);   
            switch(tube_u[t_idx].actualFrame){
                case 0:
                tube_u[t_idx].idxBlit = blit_TT(tube_up_short_sprite,tube_u[t_idx].x,tube_u[t_idx].y,
                                        tube_u[t_idx].w, tube_u[t_idx].h,0,tube_u[t_idx].idxBlit);                
                tube_u_shad[t_idx].idxBlit = blit_TT(tube_up_short_shad_sprite,tube_u[t_idx].x,448,
                                        tube_u_shad[t_idx].w, tube_u_shad[t_idx].h,0,tube_u_shad[t_idx].idxBlit);
                break;                
                case 1:
                tube_u[t_idx].idxBlit = blit_TT(tube_up_med_sprite,tube_u[t_idx].x,tube_u[t_idx].y,
                                        tube_u[t_idx].w, tube_u[t_idx].h,0,tube_u[t_idx].idxBlit);                
                tube_u_shad[t_idx].idxBlit = blit_TT(default_tube_shad_sprite,tube_u[t_idx].x,448,
                                        tube_u_shad[t_idx].w, tube_u_shad[t_idx].h,0,tube_u_shad[t_idx].idxBlit);
                break;
                case 2:
                tube_u[t_idx].idxBlit = blit_TT(tube_up_long_sprite,tube_u[t_idx].x,tube_u[t_idx].y,
                                        tube_u[t_idx].w, tube_u[t_idx].h,0,tube_u[t_idx].idxBlit);                
                tube_u_shad[t_idx].idxBlit = blit_TT(default_tube_shad_sprite,tube_u[t_idx].x,448,
                                        tube_u_shad[t_idx].w, tube_u_shad[t_idx].h,0,tube_u_shad[t_idx].idxBlit);
                break;
                
            }
            
        }
        if(tube_d[t_idx].state != STATE_DISABLED){
            switch(tube_d[t_idx].actualFrame){
                case 0:
                tube_d[t_idx].idxBlit = blit_TT(tube_down_short_sprite,tube_d[t_idx].x,tube_d[t_idx].y,
                                        tube_d[t_idx].w, tube_d[t_idx].h,0,tube_d[t_idx].idxBlit);                
                            
                break;                
                case 1:
                tube_d[t_idx].idxBlit = blit_TT(tube_down_med_sprite,tube_d[t_idx].x,tube_d[t_idx].y,
                                        tube_d[t_idx].w, tube_d[t_idx].h,0,tube_d[t_idx].idxBlit);
                break;
                case 2:
                tube_d[t_idx].idxBlit = blit_TT(tube_down_long_sprite,tube_d[t_idx].x,tube_d[t_idx].y,
                                        tube_d[t_idx].w, tube_d[t_idx].h,0,tube_d[t_idx].idxBlit);
                break;
                
            }          
        }
    }
    
    switch(birdy.actualFrame){
        case 0:
            birdy.idxBlit = blit_TT(bird_1_sprite,birdy.x, birdy.y, birdy.w, birdy.h,birdy.flip,birdy.idxBlit);    
            bird_3_shad_index = blit_TT(bird_3_shad_sprite, birdy.x,450,bird_1_shad_w,bird_1_shad_h,birdy.flip,bird_3_shad_index);
        break;    
        case 1:
            birdy.idxBlit = blit_TT(bird_2_sprite,birdy.x, birdy.y, birdy.w, birdy.h,birdy.flip,birdy.idxBlit);    
            bird_2_shad_index = blit_TT(bird_2_shad_sprite, birdy.x,450,bird_1_shad_w,bird_1_shad_h,birdy.flip,bird_2_shad_index);
        break; 
        case 2:
            birdy.idxBlit = blit_TT(bird_3_sprite,birdy.x, birdy.y, birdy.w, birdy.h,birdy.flip,birdy.idxBlit);                
            bird_1_shad_index = blit_TT(bird_1_shad_sprite, birdy.x,450,bird_1_shad_w,bird_1_shad_h,birdy.flip,bird_1_shad_index);
        break; 
    }    
    update_Score();        
}

void game_snd_buffer(uint16_t *buffer, int len) 
/* generates a 1kHz sound alternatibng between left & right */
{
	for (int i=0;i<len;i++)
	{
		if (i&100000) // square each 32 samples 
		{
            //color++;		
		}
	}
}


