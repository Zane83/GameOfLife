#include "../include/GraphicComponent.h"

GraphicComponent::GraphicComponent(){
    initAllegroRoutines();
    initDisplay();
    initEvents();
}

void GraphicComponent::initAllegroRoutines(){

    //Inizializzo tutti i moduli di Allegro (per la componente grafica)

    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    srand(time(NULL));
}

void GraphicComponent::initDisplay(){

    //Creo il display per il disegno

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    al_get_display_mode(0, &displayMode);
    mainDisplay = al_create_display(displayMode.width, displayMode.height);
}

void GraphicComponent::initEvents(){

    //Creo il timer per gestire gli FPS

    mainTimer = al_create_timer(1.0 / FPS);
    eventQueue = al_create_event_queue();

    //Aggiungo i listner alla coda di eventi
    al_register_event_source(eventQueue, al_get_display_event_source(mainDisplay));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(mainTimer));
    al_start_timer(mainTimer);
}

void GraphicComponent::updateScene(short A[DIMV][DIMH], int rows, int cols){

    //Loop del disegno su schermo
    al_wait_for_event(eventQueue, &actualEvent);
    if(actualEvent.type == ALLEGRO_EVENT_TIMER){
        drawScene(A, rows, cols);
    } else if(actualEvent.type == ALLEGRO_EVENT_KEY_DOWN && actualEvent.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
        exit = true;
    } else if(actualEvent.type == ALLEGRO_EVENT_KEY_DOWN && actualEvent.keyboard.keycode == ALLEGRO_KEY_SPACE){
        sending = true;
    }
}

bool GraphicComponent::isSending(){
    if(sending == true){
        sending = false;
        return true;
    }

    return false;
}

bool GraphicComponent::isRunning(){
    return !exit;
}

void GraphicComponent::drawScene(short A[DIMV][DIMH], int rows, int cols){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    srand(time(0));
    for(short i = 0; i < rows; i++)
    {
        for(short j = 0; j < cols; j++)
        {
            if(A[i][j] == 1)
                al_draw_filled_rectangle(j * 4, i * 4, j * 4 + 4, i * 4 + 4, al_map_rgb(rand() % 256, rand() % 256, rand() % 256));
        }
    }
    al_flip_display();
}
