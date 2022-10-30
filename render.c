#include <sganim.h>

int main(void) {
  sg_scene_t scene;
  sg_scene_init(&scene, "output/image_%05d.%s");
  
  sg_entity_t box = sg_entity_group(1, 1, 16, 20, 5);
  sg_entity_push(&(scene.root), &box);
  
  sg_entity_t text_in_box = sg_entity_text("According to all known laws of aviation, there is no way a bee should be able to fly. Its wings are too small to get its fat little body off the ground.", 1, 0, 1, 1, 14, 18, 3, -1);
  sg_entity_push(&box, &text_in_box);
  
  sg_scene_wait(&scene, 50);
  
  box.target.width = 32;
  text_in_box.target.width = 30;
  sg_scene_animate(&scene, 100, sg_approach, 0);
  
  sg_scene_wait(&scene, 50);
  
  box.target.back_color = 7;
  sg_scene_animate(&scene, 100, sg_approach, 0);
  
  sg_scene_wait(&scene, 50);
  
  box.target.x = 31;
  sg_scene_animate(&scene, 100, sg_approach, 0);
  
  sg_scene_wait(&scene, 50);
  
  return 0;
}
