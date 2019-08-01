#ifndef GC_H
#define GC_H
#define DIMV 1000
#define DIMH 1000
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

class GraphicComponent
{
    private:
        ALLEGRO_DISPLAY *mainDisplay;
        ALLEGRO_DISPLAY_MODE displayMode;
        ALLEGRO_BITMAP *buffer;
        ALLEGRO_EVENT actualEvent;
        ALLEGRO_TIMER *mainTimer;
        ALLEGRO_EVENT_QUEUE *eventQueue;
        const int FPS = 30;
        bool redraw, exit = false;
        bool sending = false;

        void initAllegroRoutines();
        void initDisplay();
        void initEvents();
        void drawScene(short A[DIMV][DIMH], int rows, int cols);
        
    public:
        GraphicComponent();
        void updateScene(short A[DIMV][DIMH], int rows, int cols);
        bool isRunning();
        bool isSending();
};

#endif