#include "cpp.hint"
#include "game.h"
#include "platform_common.h"

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

internal void
simulate_game(Input* input, float dt) {

}