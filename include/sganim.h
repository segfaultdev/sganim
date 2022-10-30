#ifndef __SGANIM_H__
#define __SGANIM_H__

// multiples of 10 are nice
#ifndef SG_FRAMERATE
#define SG_FRAMERATE 50
#endif

// good default size
#ifndef SG_WIDTH
#define SG_WIDTH 64
#endif
#ifndef SG_HEIGHT
#define SG_HEIGHT 24
#endif

// 64*24=1536, 24*40=960 -> 1536x960 video
#ifndef SG_SCALE_X
#define SG_SCALE_X 24
#endif
#ifndef SG_SCALE_Y
#define SG_SCALE_Y 40
#endif

#include <stdint.h>

typedef struct sg_state_t sg_state_t;
typedef struct sg_entity_t sg_entity_t;

typedef struct sg_frame_t sg_frame_t;
typedef struct sg_scene_t sg_scene_t;

enum {
  sg_linear,
  sg_approach,
  sg_smooth,
};

enum {
  sg_type_text,
  sg_type_group,
};

struct sg_state_t {
  // position and size data
  int x, y, width, height;
  
  // decoration data
  int fore_color, underline;
  
  // background data
  int back_color; // 0 to 15 -> colors, -1 -> transparent
};

struct sg_entity_t {
  int type;
  
  union {
    struct {
      char text_data[1024];
      int text_scale;
    };
    
    struct {
      sg_entity_t **group_childs;
      int group_count;
    };
  };
  
  sg_state_t state, target;
  float lerp;
};

struct sg_frame_t {
  int width, height;
  void *buffer;
};

struct sg_scene_t {
  sg_entity_t root; // root group, automatically generated and all that
  sg_frame_t frame; // preallocated frame
  
  const char *frame_format;
  int frames;
  
  uint8_t *font;
};

void sg_frame_text(sg_scene_t *scene, int chr, int scale, int x, int y, int color); // (USED INTERNALLY) draws a char
void sg_frame_draw(sg_scene_t *scene, sg_entity_t *entity, int x, int y, int mode); // (USED INTERNALLY) draws an entity to a framebuffer
void sg_scene_init(sg_scene_t *scene, const char *frame_format);                    // initializes a new scene, with a given frame file format
void sg_scene_save(sg_scene_t *scene, int mode);                                    // (USED INTERNALLY) saves the current frame
void sg_scene_wait(sg_scene_t *scene, int cents);                                   // waits for some cents, without animating the pending entities
void sg_scene_animate(sg_scene_t *scene, int cents, int smooth, int mode);          // as sg_scene_wait, but lerping to a new state on all entities
void sg_entity_push(sg_entity_t *group, sg_entity_t *entity);                       // pushes an entity to the group
void sg_entity_pop(sg_entity_t *group, sg_entity_t *entity);                        // pops an entity from a group
void sg_entity_lerp(sg_entity_t *entity, float lerp);                               // (USED INTERNALLY) recursively sets the lerp level
void sg_entity_update(sg_entity_t *entity);                                         // (USED INTERNALLY) recursively updates an entity

sg_entity_t sg_entity_text(const char *text, int text_scale, int underline, int x, int y, int width, int height, int fore_color, int back_color);
sg_entity_t sg_entity_group(int x, int y, int width, int height, int back_color);

#endif
