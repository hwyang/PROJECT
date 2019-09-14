#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define SCREEN_W 960
#define SCREEN_H 590

bool done;
int state = 0;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;
ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP *startpic = NULL;
ALLEGRO_BITMAP *end_pic = NULL;
ALLEGRO_BITMAP *mid_pic = NULL;
ALLEGRO_BITMAP *mario_win_pic = NULL;
ALLEGRO_BITMAP *luigi_win_pic = NULL;
ALLEGRO_BITMAP *pause = NULL;
ALLEGRO_BITMAP *ball = NULL;
ALLEGRO_BITMAP *image = NULL;
ALLEGRO_BITMAP *image_jump = NULL;
ALLEGRO_BITMAP *image2 = NULL;
ALLEGRO_BITMAP *image2_jump = NULL;
ALLEGRO_BITMAP *back  = NULL;
ALLEGRO_FONT *title = NULL;
ALLEGRO_FONT *subtitle = NULL;

ALLEGRO_SAMPLE *open_music=NULL;
ALLEGRO_SAMPLE *sound = NULL;
ALLEGRO_SAMPLE_INSTANCE *song_instance = NULL;


int score1 = 0, score2 = 0;
double x = 0, y = SCREEN_H - 215;
double x2 = 850, y2 = SCREEN_H - 215;
double ball_x = 150, ball_y = SCREEN_H - 295;
double ball_speed_y = 0, ball_speed_x = 40;
double speed_y = 0,speed_x = 15;
double speed_y2 = 0,speed_x2 = 15;
const double Gravity = 5;
const double Jump_speed = -50;

void abort_game(const char* message)
{
    printf("%s\n", message);
    exit(1);
}

void game_init(void)
{
    if (!al_init())
        abort_game("Failed to initialize allegro");

    if (!al_install_keyboard())
        abort_game("Failed to install keyboard");
    if (!al_install_mouse())
        abort_game("Failed to install mouse");
    timer = al_create_timer(1.0 / 30);
    if (!timer)
        abort_game("Failed to create timer");
    if(!al_install_audio())
        abort_game("Failed to install audio");
    if(!al_init_acodec_addon())
        abort_game("Failed to initialize audio codecs");
    if (!al_reserve_samples(2))
        abort_game("Failed to reserve samples");
    al_set_new_display_flags(ALLEGRO_WINDOWED);
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display)
        abort_game("Failed to create display");
    event_queue = al_create_event_queue();
    if (!event_queue)
        abort_game("Failed to create event queue");

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon();// initialize the ttf (True Type Font) addon
    done = false;
}

void game_destroy(void)
{
    if (timer)
        al_destroy_timer(timer);

    if (display)
        al_destroy_display(display);

    if (event_queue)
        al_destroy_event_queue(event_queue);
    if (startpic)
        al_destroy_bitmap(startpic);
    if (end_pic)
        al_destroy_bitmap(end_pic);
    if (mid_pic)
        al_destroy_bitmap(mid_pic);
    if (mario_win_pic)
        al_destroy_bitmap(mario_win_pic);
    if (luigi_win_pic)
        al_destroy_bitmap(luigi_win_pic);
    if (pause)
        al_destroy_bitmap(pause);
    if (ball)
        al_destroy_bitmap(ball);
    if (image)
        al_destroy_bitmap(image);
    if (image_jump)
        al_destroy_bitmap(image_jump);
    if (image2)
        al_destroy_bitmap(image2);
    if (image2_jump)
        al_destroy_bitmap(image2_jump);
    if (back)
        al_destroy_bitmap(back);
    if (title)
        al_destroy_font(title);
    if (subtitle)
        al_destroy_font(subtitle);
    if (open_music)
        al_destroy_sample(open_music);
    if(sound)
        al_destroy_sample(sound);
    if(song_instance)
        al_destroy_sample_instance(song_instance);
}

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h)
{
    ALLEGRO_BITMAP *resized_bmp, *loaded_bmp, *prev_target;

    resized_bmp = al_create_bitmap(w, h);
    if (!resized_bmp) return NULL;

    loaded_bmp = al_load_bitmap(filename);
    if (!loaded_bmp)
    {
        al_destroy_bitmap(resized_bmp);
        return NULL;
    }

    prev_target = al_get_target_bitmap();
    al_set_target_bitmap(resized_bmp);

    al_draw_scaled_bitmap(loaded_bmp,
                          0, 0,
                          al_get_bitmap_width(loaded_bmp),
                          al_get_bitmap_height(loaded_bmp),
                          0, 0,
                          w, h, 0 );

    al_set_target_bitmap(prev_target);
    al_destroy_bitmap(loaded_bmp);

    return resized_bmp;
}

void game_run(void)
{
    bool redraw = true;

    startpic = load_bitmap_at_size("startingpic.jpg", 960, 590);
    mid_pic = load_bitmap_at_size("midpic.jpg", 960, 590);
    mario_win_pic = load_bitmap_at_size("mario_win.jpg", 960, 590);
    luigi_win_pic = load_bitmap_at_size("Luigi_win.jpg", 960, 590);
    pause = load_bitmap_at_size("pause.jpg", 960, 590);
    ball = load_bitmap_at_size("ball.png", 60, 60);
    image = load_bitmap_at_size("Mario_stand.jpg", 100, 100);
    image_jump = load_bitmap_at_size("mario_jump.png", 100, 100);
    image2 = load_bitmap_at_size("Luigi_stand.png", 100, 100);
    image2_jump = load_bitmap_at_size("Luigi_jump.png", 100, 100);
    back  = load_bitmap_at_size("background.png", 960, 590);
    end_pic = load_bitmap_at_size("endingpic.jpg", 960, 590);
    title = al_load_font("Pokemon.ttf", 80, NULL);
    subtitle = al_load_font("Pokemon.ttf", 40, NULL);
    open_music = al_load_sample("opening.wav");
    sound = al_load_sample("jump_sound.wav");
    song_instance = al_create_sample_instance(open_music);
    al_set_sample_instance_playmode(song_instance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(song_instance, al_get_default_mixer());

/**/al_play_sample_instance(song_instance);

    al_start_timer(timer);

    int net_x[2] = { SCREEN_W/2-2.5 , SCREEN_W/2+2.5 };
    int net_y[2] = { SCREEN_H -320, SCREEN_H-120};
    int jump = 0, jump2 = 0;
    bool L = false, R = false, L2 = false, R2 = false;
    bool touch1 = false, touch2 = false;
    bool skill_speed_up_one = false, skill_speed_up_two = false;
    while (!done) {

        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;

        }
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                done = true;
            }
        }
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
            break;
        }

        if(state == 0 ){
            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    state = 1;
                }
            }
            if (redraw && al_is_event_queue_empty(event_queue)) {
                redraw = false;

                al_draw_bitmap(startpic, 0, 0, 0);
                al_draw_text(title, al_map_rgb(255,165,0), SCREEN_W/2, SCREEN_H/2-100, ALLEGRO_ALIGN_CENTER, "Mario Mushroomball");

                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, "Press Enter to Start");
                al_flip_display();
                al_rest(0.3);

                al_draw_text(subtitle, al_map_rgb(255,127,80), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, "Press Enter to Start");
                al_flip_display();
                al_rest(0.5);
            }
        }

        else if(state == 1){                //main menu

            if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(event.mouse.button == 1 && event.mouse.y >= SCREEN_H-480 && event.mouse.y <= SCREEN_H-440)
                    state = 2;
                else if (event.mouse.button == 1 && event.mouse.y >= SCREEN_H-360 && event.mouse.y <= SCREEN_H-320)
                    state = 3;
                else if (event.mouse.button == 1 && event.mouse.y >= SCREEN_H-240 && event.mouse.y <= SCREEN_H-200)
                    state = 4;
                else if (event.mouse.button == 1 && event.mouse.y >= SCREEN_H-120 && event.mouse.y <= SCREEN_H-80)
                    state = 7;
            }
            if (redraw && al_is_event_queue_empty(event_queue)){
                redraw = false;

                al_draw_bitmap(mid_pic, 0, 0, 0);
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W-20, SCREEN_H-480, ALLEGRO_ALIGN_RIGHT, "2 PLAYER MODE");
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W-20, SCREEN_H-360, ALLEGRO_ALIGN_RIGHT, "PRACTICE MODE");
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W-20, SCREEN_H-240, ALLEGRO_ALIGN_RIGHT, "NEED SOME HELP?");
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W-20, SCREEN_H-120, ALLEGRO_ALIGN_RIGHT, "EXIT");

                al_flip_display();
            }
        }

        else if(state == 2){                //2p mode
            bool score_adjust = false;

            if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(event.mouse.button == 1 && event.mouse.y >= 50 && event.mouse.y <= 90)
                    state = 8;
            }

/**/        //al_play_sample(battle_music, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP, NULL);
            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (event.keyboard.keycode) {

                    case ALLEGRO_KEY_W:
                        if(jump<2){
                        jump++;
                        al_play_sample(sound, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE, NULL);
                        speed_y=Jump_speed;
                        }
                        break;
                    case ALLEGRO_KEY_A:
                        L = true;
                        break;
                    case ALLEGRO_KEY_D:
                        R = true;
                        break;
                    case ALLEGRO_KEY_I:
                        if(jump2<2){
                        jump2++;
                        al_play_sample(sound, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE, NULL);
                        speed_y2=Jump_speed;
                        }
                        break;
                    case ALLEGRO_KEY_J:
                        L2 = true;
                        break;
                    case ALLEGRO_KEY_L:
                        R2 = true;
                        break;
                    case ALLEGRO_KEY_Q:
                        skill_speed_up_one = !skill_speed_up_one;
                        break;
                    case ALLEGRO_KEY_U:
                        skill_speed_up_two = !skill_speed_up_two;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == ALLEGRO_EVENT_KEY_UP) {
                switch (event.keyboard.keycode) {

                    case ALLEGRO_KEY_A:
                        L = false;
                        break;
                    case ALLEGRO_KEY_D:
                        R = false;
                        break;
                    case ALLEGRO_KEY_J:
                        L2 = false;
                        break;
                    case ALLEGRO_KEY_L:
                        R2 = false;
                        break;
                    default:
                        break;
                }
            }

            if (redraw && al_is_event_queue_empty(event_queue)) {
                redraw = false;

                al_draw_bitmap(back, 0, 0, 0);
                al_draw_filled_rectangle(net_x[0],
                                         net_y[0],
                                         net_x[1],
                                         net_y[1],
                                         al_map_rgb( 139, 71, 38));

/*=============================================================*/
                /*CONTROL IMAGE MOVING*/
                if(skill_speed_up_one == true)speed_x = 45;
                if(skill_speed_up_one == false)speed_x = 15;

                if(y+100>=SCREEN_H-120 && speed_y>0){
                    speed_y=-speed_y;
                }
                if(x+100>=SCREEN_W/2-2.5){
                    R=false;
                    x=SCREEN_W/2-105;
                }
                if(x<=0){
                    L=false;
                    x=0;
                }
                if(jump)speed_y+=Gravity;
                y+=speed_y;
                x+=(R-L)*speed_x;
                if(y+100>=SCREEN_H-120){
                    y=SCREEN_H-220;
                    jump=0;
                    speed_y=0;
                }
                /*CONTROL IMAGE2 MOVING*/
                if(skill_speed_up_two == true)speed_x2 = 45;
                if(skill_speed_up_two == false)speed_x2 = 15;

                if(y2+100>=SCREEN_H-120 && speed_y2>0){
                    speed_y2=-speed_y2;
                }
                if(x2+100>=SCREEN_W){
                    R2=false;
                    x2=SCREEN_W-100;
                }
                if(x2<=SCREEN_W/2+15){
                    L2=false;
                    x2=SCREEN_W/2+15;
                }
                if(jump2)speed_y2+=Gravity;
                y2+=speed_y2;
                x2+=(R2-L2)*speed_x2;
                if(y2+100>=SCREEN_H-120){
                    y2=SCREEN_H-220;
                    jump2=0;
                    speed_y2=0;
                }

                /*CONTROL BALL MOVING*/

                if(!(ball_x +60 < x || ball_x > x+100 || ball_y+60 < y+20 || ball_y > y+20)){
                    touch1 = true;
                    ball_speed_y = Jump_speed;
                    touch2 = false;

                    if(jump == 2){      //±þ²y
                        ball_speed_y = Jump_speed*0.3;

                    }
                }
                if(!(ball_x +60 < x2 || ball_x > x2+100 || ball_y+60 < y2+20 || ball_y > y2+20)){
                    touch2 = true;
                    ball_speed_y = Jump_speed;
                    touch1 = false;

                    if(jump == 2){  //±þ²y
                        ball_speed_y = Jump_speed*0.3;
                    }
                }
                if(touch1)ball_speed_y+=Gravity;
                if(touch2)ball_speed_y+=Gravity;
                ball_x+=(touch1-touch2)*ball_speed_x;
                ball_y+=(ball_speed_y);

                if(ball_x+60>=SCREEN_W){
                    //touch1 = false;
                    ball_x = SCREEN_W-60;
                }
                if(ball_x<=0){
                    //touch2 = false;
                    ball_x = 0;
                }

                /*NET COLLISION*/
                if(touch1 == true){
                    if(!(ball_x +60 < SCREEN_W/2-2.5 || ball_x > SCREEN_W/2+2.5 || ball_y+60 < SCREEN_H-320)){

                        ball_x = SCREEN_W/2-70;
                    }
                }
                if(touch2 == true){
                    if(!(ball_x +60 < SCREEN_W/2-2.5 || ball_x > SCREEN_W/2+2.5 || ball_y+60 < SCREEN_H-320)){

                        ball_x = SCREEN_W/2+10;
                    }
                }

                if(ball_y>=SCREEN_H-160){
                    score_adjust = true;
                    touch1=false;
                    touch2=false;
                    ball_speed_y=0;
                }

                /*SCORE*/
                if(score_adjust){
                    if(ball_x >= SCREEN_W/2 && ball_x <=SCREEN_W){
                        score1++;
                        score_adjust = false;

                        x = 0;
                        y = SCREEN_H - 215;
                        al_rest(0.5);
                        x2 = 850;
                        y2 = SCREEN_H - 215;
                        ball_x = SCREEN_W-200;
                        ball_y = SCREEN_H-295;
                    }
                    else if(ball_x >=0 && ball_x <= SCREEN_W/2){
                        score2++;
                        score_adjust = false;

                        x = 0;
                        y = SCREEN_H - 215;
                        al_rest(0.5);
                        x2 = 850;
                        y2 = SCREEN_H - 215;
                        ball_x = 150;
                        ball_y = SCREEN_H-295;
                    }
                }
/*=============================================================*/

                al_draw_text(subtitle, al_map_rgb(178,34,34), SCREEN_W/2, 50, ALLEGRO_ALIGN_CENTER, "PAUSE");
                al_draw_bitmap(ball , ball_x ,ball_y, 0);

                if(y < SCREEN_H-220)al_draw_bitmap(image_jump , x ,y, 0);
                else al_draw_bitmap(image , x ,y, 0);
                if(y2 < SCREEN_H-220)al_draw_bitmap(image2_jump , x2,y2, 0);
                else al_draw_bitmap(image2 , x2 ,y2, 0);

                al_draw_textf(title, al_map_rgb(178,34,34), 40, 20,  ALLEGRO_ALIGN_LEFT, "%d", score1);
                al_draw_textf(title, al_map_rgb(178,34,34), 920, 20,  ALLEGRO_ALIGN_RIGHT, "%d", score2);

                if(score1 == 6 && score2 < 6){
                    state = 6;
                }
                if(score2 == 6 && score1 < 6){
                    state = 5;
                }
                al_flip_display();

            }

        }

        else if(state == 3){                //practice mode
/**/        //al_play_sample(battle_music, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP, NULL);

            if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(event.mouse.button == 1 && event.mouse.y >= 50 && event.mouse.y <= 90)
                    state = 1;
            }


            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (event.keyboard.keycode) {

                    case ALLEGRO_KEY_W:
                        if(jump<1){
                        jump++;
                        al_play_sample(sound, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE, NULL);
                        speed_y=Jump_speed;
                        }
                        break;
                    case ALLEGRO_KEY_A:
                        L = true;
                        break;
                    case ALLEGRO_KEY_D:
                        R = true;
                        break;

                    case ALLEGRO_KEY_Q:
                        skill_speed_up_one = !skill_speed_up_one;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == ALLEGRO_EVENT_KEY_UP) {
                switch (event.keyboard.keycode) {

                    case ALLEGRO_KEY_A:
                        L = false;
                        break;
                    case ALLEGRO_KEY_D:
                        R = false;
                        break;
                    default:
                        break;
                }
            }

            if (redraw && al_is_event_queue_empty(event_queue)) {
                redraw = false;

                al_draw_bitmap(back, 0, 0, 0);
                al_draw_filled_rectangle(net_x[0],
                                         net_y[0],
                                         net_x[1],
                                         net_y[1],
                                         al_map_rgb( 139, 71, 38));

/*=============================================================*/
                /*CONTROL IMAGE MOVING*/
                if(skill_speed_up_one == true)speed_x = 45;
                if(skill_speed_up_one == false)speed_x = 15;

                if(y+100>=SCREEN_H-120 && speed_y>0){
                    speed_y=-speed_y;
                }
                if(x+100>=SCREEN_W/2-2.5){
                    R=false;
                    x=SCREEN_W/2-105;
                }
                if(x<=0){
                    L=false;
                    x=0;
                }
                if(jump)speed_y+=Gravity;
                y+=speed_y;
                x+=(R-L)*speed_x;
                if(y+100>=SCREEN_H-120){
                    y=SCREEN_H-220;
                    jump=0;
                    speed_y=0;
                }
                /*CONTROL IMAGE2 MOVING*/
                x2 = ball_x;
                if(x2+100>=SCREEN_W){

                    x2=SCREEN_W-100;
                }
                if(x2<=SCREEN_W/2+15){

                    x2=SCREEN_W/2+15;
                }


                /*CONTROL BALL MOVING*/

                if(!(ball_x +60 < x || ball_x > x+100 || ball_y+60 < y+20 || ball_y > y+20)){
                    touch1 = true;
                    ball_speed_y = Jump_speed;
                    touch2 = false;
                }
                if(!(ball_x +60 < x2 || ball_x > x2+100 || ball_y+60 < y2+20 || ball_y > y2+20)){
                    touch2 = true;
                    ball_speed_y = Jump_speed;
                    touch1 = false;
                }
                if(touch1)ball_speed_y+=Gravity;
                if(touch2)ball_speed_y+=Gravity;
                ball_x+=(touch1-touch2)*ball_speed_x;
                ball_y+=(ball_speed_y);

                if(ball_x+60>=SCREEN_W){
                    //touch1 = false;
                    ball_x = SCREEN_W-60;
                }
                if(ball_x<=0){
                    //touch2 = false;
                    ball_x = 0;
                }

                /*NET COLLISION*/
                if(touch1 == true){
                    if(!(ball_x +60 < SCREEN_W/2-2.5 || ball_x > SCREEN_W/2+2.5 || ball_y+60 < SCREEN_H-320)){

                        ball_x = SCREEN_W/2-70;
                    }
                }

                if(ball_y+60>=SCREEN_H-100){
                    x = 0;
                    y = SCREEN_H - 215;

                    al_rest(0.5);
                    ball_x = 150;
                    ball_y = SCREEN_H-295;
                    touch1=false;
                    touch2=false;
                    ball_speed_y=0;
                }

/*=============================================================*/

                al_draw_text(subtitle, al_map_rgb(178,34,34), SCREEN_W-30, 50, ALLEGRO_ALIGN_RIGHT, "MENU");
                al_draw_bitmap(ball , ball_x ,ball_y, 0);
                 if(y < SCREEN_H-220)al_draw_bitmap(image_jump , x ,y, 0);
                else al_draw_bitmap(image , x ,y, 0);
                al_draw_bitmap(image2, x2, y2, 0);

                al_flip_display();

            }
        }

        else if(state == 4){                //help
            if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(event.mouse.button == 1 && event.mouse.y >= SCREEN_H-140 && event.mouse.y <= SCREEN_H-100)
                    state = 1;
            }

            if (redraw && al_is_event_queue_empty(event_queue)){
                redraw = false;
                al_draw_bitmap(mid_pic, 0, 0, 0);
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, SCREEN_H-480, ALLEGRO_ALIGN_CENTER, "Use A, D, W to move, Q to speed up Mario!");
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, SCREEN_H-360, ALLEGRO_ALIGN_CENTER, "Use J, L, I to move, U to speed up Luigi!");

                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, SCREEN_H-120, ALLEGRO_ALIGN_CENTER, "BACK TO MAIN MENU");
                al_flip_display();
            }
        }

        else if(state == 5){                //LUIGI WIN
            if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(event.mouse.button == 1 && event.mouse.y >= SCREEN_H-140 && event.mouse.y <= SCREEN_H-100)
                    state = 1;
            }

            if (redraw && al_is_event_queue_empty(event_queue)){
                redraw = false;
                al_draw_bitmap(luigi_win_pic, 0, 0, 0);
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, 150, ALLEGRO_ALIGN_CENTER, "Luigi wins! Congratulation!");
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, SCREEN_H-120, ALLEGRO_ALIGN_CENTER, "BACK TO MAIN MENU");
                al_flip_display();
            }
            score1 = 0;
            score2 = 0;
        }

        else if(state == 6){                //MARIO WIN
            if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(event.mouse.button == 1 && event.mouse.y >= SCREEN_H-140 && event.mouse.y <= SCREEN_H-100)
                    state = 1;
            }

            if (redraw && al_is_event_queue_empty(event_queue)){
                redraw = false;
                al_draw_bitmap(mario_win_pic, 0, 0, 0);
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, 150, ALLEGRO_ALIGN_CENTER, "Mario wins! Congratulation!");
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, SCREEN_H-120, ALLEGRO_ALIGN_CENTER, "BACK TO MAIN MENU");
                al_flip_display();

            }
            score1 = 0;
            score2 = 0;
        }

        else if(state == 7){                //BYE-BYE
            if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(event.mouse.button == 1 && event.mouse.y >= 400 && event.mouse.y <= 440)
                    done = true;
                if(event.mouse.button == 1 && event.mouse.y >= 500 && event.mouse.y <= 540)
                    state = 1;
            }

            if (redraw && al_is_event_queue_empty(event_queue)){
                redraw = false;
                al_draw_bitmap(end_pic, 0, 0, 0);
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, 300, ALLEGRO_ALIGN_CENTER, "Are you sure?");
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, 400, ALLEGRO_ALIGN_CENTER, "YES");
                al_draw_text(subtitle, al_map_rgb(255,165,0), SCREEN_W/2, 500, ALLEGRO_ALIGN_CENTER, "NO");
                al_flip_display();
            }
        }

        else if(state == 8){                   //pause
            if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(event.mouse.button == 1 && event.mouse.y >= 300 && event.mouse.y <= 380)
                    state = 2;
            }

            if (redraw && al_is_event_queue_empty(event_queue)){
                redraw = false;
                al_draw_bitmap(pause, 0, 0, 0);
                al_draw_text(title, al_map_rgb(255,165,0), SCREEN_W/2, 300, ALLEGRO_ALIGN_CENTER, "RESUME");
                al_flip_display();
            }
        }

    }
}

int main(int argc, char* argv[]) {

    game_init();
    game_run();
    game_destroy();

    return 0;
}
