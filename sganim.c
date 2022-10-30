#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image_write.h>
#include <stb_image.h>
#include <sganim.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const uint32_t sg_colors[] = {
  0x00000000,
  0x005C6068,
  0x00B8B0B0,
  0x00FCFCFC,
  0x00AC381C,
  0x00FC7070,
  0x001428A8,
  0x004848FC,
  0x00008820,
  0x0028F870,
  0x00D02CB8,
  0x00EC74FC,
  0x001C58AC,
  0x0050A8F8,
  0x00E4D43C,
  0x0020ECF8,
};

void sg_frame_text(sg_scene_t *scene, int chr, int scale, int x, int y, int color) {
  sg_frame_t *frame = &(scene->frame);
  if (color < 0) return;
  
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 12; j++) {
      int font_x = j + (chr % 16) * 12;
      int font_y = i + (chr / 16) * 20;
      
      if (scene->font[font_x + font_y * 16 * 12] < 0x80) continue;
      
      for (int delta_y = 0; delta_y < scale; delta_y++) {
        for (int delta_x = 0; delta_x < scale; delta_x++) {
          int pixel_x = x + j * scale + delta_x;
          int pixel_y = y + i * scale + delta_y;
          
          if (pixel_x < frame->width && pixel_y < frame->height) {
            uint32_t *data = (uint32_t *)(frame->buffer + (pixel_x + pixel_y * frame->width) * 3);
            *data = (*data & 0xFF000000) | sg_colors[color];
          }
        }
      }
    }
  }
}

void sg_frame_draw(sg_scene_t *scene, sg_entity_t *entity, int x, int y, int mode) {
  sg_frame_t *frame = &(scene->frame);
  
  x += (entity->state.x + (entity->target.x - entity->state.x) * entity->lerp) * SG_SCALE_X;
  y += (entity->state.y + (entity->target.y - entity->state.y) * entity->lerp) * SG_SCALE_Y;
  
  int width = (entity->state.width + (entity->target.width - entity->state.width) * entity->lerp) * SG_SCALE_X;
  int height = (entity->state.height + (entity->target.height - entity->state.height) * entity->lerp) * SG_SCALE_Y;
  
  float side = (float)(width * width) / (float)(SG_SCALE_X * SG_SCALE_X) + (float)(height * height) / (float)(SG_SCALE_Y * SG_SCALE_Y);
  
  for (int i = 0; i < height; i++) {
    int tile_y = i / SG_SCALE_Y;
    int pixel_y = i + y;
    
    if (pixel_y >= frame->height) break;
    
    if (entity->state.back_color == entity->target.back_color) {
      if (entity->state.back_color < 0) break;
      
      for (int j = 0; j < width; j++) {
        int pixel_x = j + x;
        if (pixel_x >= frame->width) break;
        
        uint32_t *data = (uint32_t *)(frame->buffer + (pixel_x + pixel_y * frame->width) * 3);
        *data = (*data & 0xFF000000) | sg_colors[entity->state.back_color];
      }
    } else {
      for (int j = 0; j < width; j++) {
        int tile_x = j / SG_SCALE_X;
        int pixel_x = j + x;
        
        if (pixel_x >= frame->width) break;
        
        int dist_x = tile_x;
        int dist_y = tile_y;
        
        if (mode == 1 || mode == 2) {
          dist_x = (width / SG_SCALE_X) - 1 - dist_x;
        }
        
        if (mode == 2 || mode == 3) {
          dist_y = (height / SG_SCALE_Y) - 1 - dist_y;
        }
        
        float dist = dist_x * dist_x + dist_y * dist_y;
        float min_dist = side * entity->lerp;
        
        int color = (dist > min_dist) ? entity->state.back_color : entity->target.back_color;
        
        if (color >= 0 && pixel_x < frame->width && pixel_y < frame->height) {
          uint32_t *data = (uint32_t *)(frame->buffer + (pixel_x + pixel_y * frame->width) * 3);
          *data = (*data & 0xFF000000) | sg_colors[color];
        }
      }
    }
  }
  
  if (entity->type == sg_type_text) {
    int length = strlen(entity->text_data);
    
    char word[1024];
    int offset = 0, word_start = 0;
    
    int text_width = width / (SG_SCALE_X * entity->text_scale);
    int delta_x = 0, delta_y = 0;
    
    int limit = length * entity->lerp;
    
    for (int i = 0; i <= length; i++) {
      if (!entity->text_data[i] || entity->text_data[i] == ' ') {
        if (!offset) continue;
        
        if (offset > text_width - delta_x) {
          delta_x = 0;
          delta_y++;
          
          int current = 0;
          
          while (current < offset) {
            int step = offset - current;
            if (step > text_width) step = text_width;
            
            for (int j = 0; j < step; j++) {
              int color = (j + current + word_start <= limit) ? entity->target.fore_color : entity->state.fore_color;
              
              sg_frame_text(scene, word[j + current], (SG_SCALE_X * entity->text_scale) / 12,
                x + delta_x * SG_SCALE_X * entity->text_scale, y + delta_y * SG_SCALE_Y * entity->text_scale, color
              );
              
              delta_x++;
            }
            
            current += step;
            
            if (delta_x >= text_width) {
              delta_x = 0;
              delta_y++;
            }
          }
        } else {
          for (int j = 0; j < offset; j++) {
            int color = (j + word_start <= limit) ? entity->target.fore_color : entity->state.fore_color;
            
            sg_frame_text(scene, word[j], (SG_SCALE_X * entity->text_scale) / 12,
              x + delta_x * SG_SCALE_X * entity->text_scale, y + delta_y * SG_SCALE_Y * entity->text_scale, color
            );
            
            delta_x++;
          }
        }
        
        delta_x++;
        offset = 0;
      } else {
        if (!offset) word_start = i;
        word[offset++] = entity->text_data[i];
      }
    }
  } else if (entity->type == sg_type_group) {
    for (int i = 0; i < entity->group_count; i++) {
      if (!entity->group_childs[i]) continue;
      sg_frame_draw(scene, entity->group_childs[i], x, y, mode);
    }
  }
}

void sg_scene_init(sg_scene_t *scene, const char *frame_format) {
  scene->frame_format = frame_format;
  scene->frames = 0;
  
  scene->frame.width = SG_WIDTH * SG_SCALE_X;
  scene->frame.height = SG_HEIGHT * SG_SCALE_Y;
  
  scene->frame.buffer = malloc(scene->frame.width * scene->frame.height * 3);
  scene->frame.background = calloc(scene->frame.width * scene->frame.height * 3, 1);
  
  scene->root = sg_entity_group(0, 0, SG_WIDTH, SG_HEIGHT, -1);
  
  int x, y, n;
  scene->font = stbi_load("font.png", &x, &y, &n, 1);
}

void sg_scene_save(sg_scene_t *scene, int mode) {
  memcpy(scene->frame.buffer, scene->frame.background, scene->frame.width * scene->frame.height * 3);
  sg_frame_draw(scene, &(scene->root), 0, 0, mode);
  
  char buffer[256];
  sprintf(buffer, scene->frame_format, scene->frames++, "bmp");
  
  stbi_write_bmp(buffer, scene->frame.width, scene->frame.height, 3, scene->frame.buffer);
  printf("saved frame %s\n", buffer);
}

void sg_scene_wait(sg_scene_t *scene, int cents) {
  int frames = (cents * SG_FRAMERATE) / 100;
  
  for (int i = 0; i < frames; i++) {
    sg_scene_save(scene, 0);
  }
}

void sg_scene_animate(sg_scene_t *scene, int cents, int smooth, int mode) {
  int frames = (cents * SG_FRAMERATE) / 100;
  
  for (int i = 0; i < frames; i++) {
    float lerp = (float)(i) / (float)(frames);
    
    if (smooth == sg_approach) {
      const float rate = 0.01f;
      lerp = (1.0f - powf(rate, lerp)) / (1.0f - rate);
    } else if (smooth == sg_smooth) {
      
    }
    
    sg_entity_lerp(&(scene->root), lerp);
    sg_scene_save(scene, mode);
  }
  
  sg_entity_update(&(scene->root));
}

void sg_scene_image(sg_scene_t *scene, const char *image_path, int mode) {
  int width, height, n;
  void *buffer = stbi_load(image_path, &width, &height, &n, 3);
  
  if (mode == sg_image_aspect) {
    if ((float)(width) / (float)(height) > (float)(scene->frame.width) / (float)(scene->frame.height)) {
      int new_height = ((float)(height) / (float)(width)) * (float)(scene->frame.width);
      int start_y = (scene->frame.height - new_height) / 2;
      
      for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < scene->frame.width; x++) {
          int pixel_x = (x * width) / scene->frame.width;
          int pixel_y = (y * height) / new_height;
          
          uint32_t *source_data = (uint32_t *)(buffer + (pixel_x + pixel_y * width) * 3);
          uint32_t *dest_data = (uint32_t *)(scene->frame.background + (x + (y + start_y) * scene->frame.width) * 3);
          
          *dest_data = (*dest_data & 0xFF000000) | (*source_data & 0x00FFFFFF);
        }
      }
    } else {
      int new_width = ((float)(width) / (float)(height)) * (float)(scene->frame.height);
      int start_x = (scene->frame.width - new_width) / 2;
      
      for (int y = 0; y < scene->frame.height; y++) {
        for (int x = 0; x < new_width; x++) {
          int pixel_x = (x * width) / new_width;
          int pixel_y = (y * height) / scene->frame.height;
          
          uint32_t *source_data = (uint32_t *)(buffer + (pixel_x + pixel_y * width) * 3);
          uint32_t *dest_data = (uint32_t *)(scene->frame.background + (x + start_x + y * scene->frame.width) * 3);
          
          *dest_data = (*dest_data & 0xFF000000) | (*source_data & 0x00FFFFFF);
        }
      }
    }
  } else if (mode == sg_image_zoom) {
    if ((float)(width) / (float)(height) > (float)(scene->frame.width) / (float)(scene->frame.height)) {
      int new_width = ((float)(width) / (float)(height)) * (float)(scene->frame.height);
      int start_x = (scene->frame.width - new_width) / 2;
      
      for (int y = 0; y < scene->frame.height; y++) {
        for (int x = 0; x < new_width; x++) {
          if (x + start_x < 0 || x + start_x >= scene->frame.width) continue;
          
          int pixel_x = (x * width) / new_width;
          int pixel_y = (y * height) / scene->frame.height;
          
          uint32_t *source_data = (uint32_t *)(buffer + (pixel_x + pixel_y * width) * 3);
          uint32_t *dest_data = (uint32_t *)(scene->frame.background + (x + start_x + y * scene->frame.width) * 3);
          
          *dest_data = (*dest_data & 0xFF000000) | (*source_data & 0x00FFFFFF);
        }
      }
    } else {
      int new_height = ((float)(height) / (float)(width)) * (float)(scene->frame.width);
      int start_y = (scene->frame.height - new_height) / 2;
      
      for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < scene->frame.width; x++) {
          if (y + start_y < 0 || y + start_y >= scene->frame.height) continue;
          
          int pixel_x = (x * width) / scene->frame.width;
          int pixel_y = (y * height) / new_height;
          
          uint32_t *source_data = (uint32_t *)(buffer + (pixel_x + pixel_y * width) * 3);
          uint32_t *dest_data = (uint32_t *)(scene->frame.background + (x + (y + start_y) * scene->frame.width) * 3);
          
          *dest_data = (*dest_data & 0xFF000000) | (*source_data & 0x00FFFFFF);
        }
      }
    }
  }
  
  stbi_image_free(buffer);
}

void sg_entity_push(sg_entity_t *group, sg_entity_t *entity) {
  for (int i = 0; i < group->group_count; i++) {
    if (!group->group_childs[i]) {
      group->group_childs[i] = entity;
      return;
    }
  }
  
  group->group_childs = realloc(group->group_childs, (group->group_count + 1) * sizeof(sg_entity_t *));
  group->group_childs[group->group_count++] = entity;
}

void sg_entity_pop(sg_entity_t *group, sg_entity_t *entity) {
  for (int i = 0; i < group->group_count; i++) {
    if (group->group_childs[i] == entity) {
      group->group_childs[i] = NULL;
    }
  }
}

void sg_entity_lerp(sg_entity_t *entity, float lerp) {
  if (!entity) return;
  entity->lerp = lerp;
  
  if (entity->type == sg_type_group) {
    for (int i = 0; i < entity->group_count; i++) {
      sg_entity_lerp(entity->group_childs[i], lerp);
    }
  }
}

void sg_entity_update(sg_entity_t *entity) {
  if (!entity) return;
  
  entity->state = entity->target;
  entity->lerp = 0.0f;
  
  if (entity->type == sg_type_group) {
    for (int i = 0; i < entity->group_count; i++) {
      sg_entity_update(entity->group_childs[i]);
    }
  }
}

sg_entity_t sg_entity_text(const char *text, int text_scale, int underline, int x, int y, int width, int height, int fore_color, int back_color) {
  if (width < 0) {
    width = strlen(text) * text_scale;
  }
  
  sg_entity_t entity = (sg_entity_t){
    .type = sg_type_text,
    .text_scale = text_scale,
    .lerp = 0.0f,
  };
  
  strcpy(entity.text_data, text);
  
  entity.state = entity.target = (sg_state_t){
    .x = x,
    .y = y,
    .width = width,
    .height = height,
    .fore_color = fore_color,
    .underline = underline,
    .back_color = back_color,
  };
  
  return entity;
}

sg_entity_t sg_entity_group(int x, int y, int width, int height, int back_color) {
  sg_entity_t entity = (sg_entity_t){
    .type = sg_type_group,
    .group_childs = NULL,
    .group_count = 0,
    .lerp = 0.0f,
  };
  
  entity.state = entity.target = (sg_state_t){
    .x = x,
    .y = y,
    .width = width,
    .height = height,
    .back_color = back_color,
  };
  
  return entity;
}
