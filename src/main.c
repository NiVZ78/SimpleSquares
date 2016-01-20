#include <pebble.h>
  
// pointer to main window
static Window *s_main_window;

// pointer to main window layer
static Layer *s_main_window_layer;

// pointer to the layer we will draw on
static Layer *s_canvas_layer;

// co-ordinate variable which will hold the value of the center of the screen (so content will be centered on all Pebble watches)
GPoint s_main_window_center;

// Array to describe the 3x5 digits
// 1 will draw a square
// 0 will leave blank space
char *digits[10] = {
              "111101101101111", 
              "001001001001001",
              "111001111100111",
              "111001111001111",
              "101101111001001",
              "111100111001111",
              "111100111101111",
              "111001001001001",
              "111101111101111",
              "111101111001111"};

// function to draw one of our digits on the graphics context at a given co-ordinate
static void draw_digit(GContext *ctx, int digit, int x, int y){

  // set the fill colour
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  // loop through the rows
  for (int i=0; i<5; i++){
    // loop through th columns
    for (int j=0; j<3; j++){
      
       // if the array character is a 1 then draw a square
       if (digits[digit][(i*3)+j] == '1'){
         graphics_fill_rect(ctx, GRect((j*15)+x,(i*15)+y, 14, 14), 0, GCornerNone);
       }   
      
    }
  }
  
}


static void canvas_update_proc(Layer *this_layer, GContext *ctx) {

  // Get the current time
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  // Get the 2 digit hours
  // divide by 10 to get the first digit
  draw_digit(ctx, t->tm_hour/10, s_main_window_center.x - 52, s_main_window_center.y - 82);
  // get the remainder of dividing by 10 to get the second digit
  draw_digit(ctx, t->tm_hour%10, s_main_window_center.x + 8, s_main_window_center.y -82);
  
  // 2 digit minutes
  // divide by 10 to get the first digit
  draw_digit(ctx, t->tm_min/10, s_main_window_center.x - 52, s_main_window_center.y + 8);
  // get the remainder of dividing by 10 to get the second digit
  draw_digit(ctx, t->tm_min%10, s_main_window_center.x + 8, s_main_window_center.y + 8);
   
}


// Tick handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
   
  // Tell our layer it needs to redraw
  layer_mark_dirty(s_canvas_layer);
  
}



static void main_window_load(Window *window) {
  
  // get the main window layer
  s_main_window_layer = window_get_root_layer(s_main_window);
  
  // Get the boundaries of the main layer
  GRect window_bounds = layer_get_bounds(s_main_window_layer);
  
  // Get the center co-ordinates
  s_main_window_center = grect_center_point(&window_bounds);

  // Create the layer we will draw on
  s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  
  // Add the layer to our main window layer
  layer_add_child(s_main_window_layer, s_canvas_layer);

  // Set the update procedure for our layer
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  // Subscribe to the timer tick service
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
}


static void main_window_unload(Window *window) {
  
  // Unsubscribe from event services
  tick_timer_service_unsubscribe();
  
  // Destroy Layer
  layer_destroy(s_canvas_layer);
  
}


static void init(void) {
  
  // Create the main window
  s_main_window = window_create();
  
  // set the background colour
  window_set_background_color(s_main_window, GColorBlack);
  
  // set the window load and unload handlers
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  // show the window on screen
  window_stack_push(s_main_window, true);
  
}


static void deinit(void) {
  
  // Destroy the main window
  window_destroy(s_main_window);
  
}


int main(void) {
  
  init();
  app_event_loop();
  deinit();
  
}