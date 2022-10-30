#include <sganim.h>

int main(void) {
  sg_scene_t scene;
  sg_scene_init(&scene, "output/image_%05d.%s");
  
  sg_scene_wait(&scene, 50);
  
  sg_scene_image(&scene, "spoon.png", sg_image_zoom);
  scene.root.state.back_color = 0;
  
  sg_scene_animate(&scene, 100, sg_approach, 0);
  
  sg_entity_t text_1 = sg_entity_text("This is Camila", 1, 0, 1, 1, -1, 1, -1, -1);
  sg_entity_push(&(scene.root), &text_1);
  
  text_1.target.back_color = 0;
  sg_scene_animate(&scene, 50, sg_approach, 0);
  
  text_1.target.fore_color = 3;
  sg_scene_animate(&scene, 100, sg_linear, 0);
  
  sg_scene_wait(&scene, 200);
  
  sg_entity_t text_2 = sg_entity_text("She is a spoon", 1, 0, 1, 2, -1, 1, -1, -1);
  sg_entity_push(&(scene.root), &text_2);
  
  text_2.target.back_color = 0;
  sg_scene_animate(&scene, 50, sg_approach, 0);
  
  text_2.target.fore_color = 3;
  sg_scene_animate(&scene, 100, sg_linear, 0);
  
  sg_scene_wait(&scene, 200);
  
  sg_entity_t text_3 = sg_entity_text("Don't be like Camila", 2, 1, 1, 3, -1, 2, -1, -1);
  sg_entity_push(&(scene.root), &text_3);
  
  text_3.target.back_color = 0;
  sg_scene_animate(&scene, 50, sg_approach, 0);
  
  text_3.target.fore_color = 3;
  sg_scene_animate(&scene, 200, sg_linear, 0);
  
  sg_scene_wait(&scene, 150);
  
  return 0;
}
