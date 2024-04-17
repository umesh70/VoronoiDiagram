//little endian format
//0xAABBGGRR
//RR GG BB AA

#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<assert.h>

#define output_file_path "seeds.ppm"
//size or resolution of the output image
#define WIDTH 800
#define HEIGHT 600
#define SEED_COUNT 10
#define SEED_RADIUS 5
#define BACKGROUND_COLOR 0xFFFFFF
#define SEED_COLOR 0x31363F


typedef struct {
    int x,y;
}Point;

//static memory for holding image
static uint32_t image[HEIGHT][WIDTH];
static Point seeds[SEED_COUNT];
void fill_image(uint32_t color){

    for(size_t y = 0 ;y<HEIGHT;++y){
        for(size_t x = 0; x<WIDTH;++x){
            image[y][x]=color;
        }
    }
}

void SeedRender(int cx, int cy, int radius, uint32_t color){
            //cx , cy  = coordinates of the center of circle or seed
            //x0,y0 = top left corner of the bounding box around the circle
    int x0 = cx-radius;
    int x1 = cx + radius;
            //x1,y1 = bottom right corner of the bounding box around the circle
    int y0 = cy - radius;
    int y1 = cy + radius;


    for(int x = x0 ; x<=x1; ++x ){
        if ( 0 <= x && x<WIDTH){
            for(int y = y0 ; y<=y1; ++y){
               if( 0 <= y && y<HEIGHT){
                    int dx = cx-x;
                    int dy = cy-y;
                    if (dx*dx + dy*dy <= radius*radius){
                        image[y][x] = color;
                    }
                }
            }
        }
    }
}
//ppm format has a metadata header and binary pixel representation
//ppm format is usefull for generating images but not for storing imagem,
//since it is a raw format and does not provide any compression techniques 
void ppm_image(const char *filepath){
    FILE *f = fopen(filepath,"wb");
    if (f == NULL){
        fprintf(stderr,"Error:could not write into file %s %s\n",filepath,strerror(errno));
        exit(1);
    }
    //header of ppm image
    fprintf(f,"P6\n%d %d 255\n",WIDTH,HEIGHT);

    for (size_t y = 0 ; y<HEIGHT;++y){
        for (size_t x= 0 ;x<WIDTH;++x){
            //extract RGB from a pixel at y,x position
            uint32_t pixel = image[y][x];
            uint8_t bytes[3] = {
                (pixel&0x0000FF)>>8*0,
                (pixel&0x00FF00)>>8*1,
                (pixel&0xFF0000)>>8*2,
            };
            fwrite(bytes,sizeof(bytes),1,f);
            assert(!ferror(f));
        }
        
    }
    int ret = fclose(f);
    assert(ret == 0);
}

void generate_Seeds(){
    for (size_t i = 0 ;i<SEED_COUNT;++i){
        seeds[i].x = rand()%WIDTH;
        seeds[i].y = rand()%HEIGHT;
    }
}
int main(void){
    fill_image(BACKGROUND_COLOR);
    generate_Seeds();
    for (size_t i = 0 ;i<SEED_COUNT;++i){
        SeedRender(seeds[i].x,seeds[i].y,SEED_RADIUS,SEED_COLOR);
    }
    
    ppm_image(output_file_path);
    return 0;
}
