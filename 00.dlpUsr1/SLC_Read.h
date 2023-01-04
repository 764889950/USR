#ifndef __SLC_READ_
#define __SLC_READ_

typedef struct pixel{int x;int y;int bits_per_pix;} pixel_Framebuffer;

void read_Framebuffer(pixel_Framebuffer &temp);
void curr(void);
void curr_picture(void);
void ReadSLC_inf(const char file_dir[]);
int slc_main(void);


#endif