/*
#include <unistd.h>

#include <stdio.h>
#include <sys/fcntl.h>


#include "tslib.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
*/

#include <assert.h> 
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <ft2build.h>
#include FT_FREETYPE_H


#ifdef NDEBUG
	#define check_return(expr) expr
#else
	#define check_return(expr) assert(!(expr))
#endif


// Framebuffer
// TODO : check rotation
int    fb_handle;
short* fb_map;
int    fb_width;
int    fb_height;

// Freetype
FT_Library library;
FT_Face    face;
int        ip_font_size = 0;

void draw_red_background()
{
	int* pixel = (int*) fb_map;
	int i = 0;
	
	for( ; i < fb_width * fb_height / 2; i++) {
		*pixel = 0xF800F800;
		pixel++;
	}
}

void draw_text(int offset_x, int offset_y, char* text)
{
	assert(0 <= offset_x && offset_x < fb_width);
	assert(0 <= offset_y && offset_y < fb_height);
	
	while (text[0]) {
		check_return(FT_Load_Char(face,
		                     text[0],
		                     FT_LOAD_RENDER));
		FT_Bitmap bitmap = face->glyph->bitmap;
		
		//int startx = offset_x + face->glyph->bitmap_left;
		int x, y;
		for (y = 0; y < bitmap.rows; y++) {
			short* pixel = fb_map
			               + (offset_y + y - face->glyph->bitmap_top) * fb_width
			               + offset_x + face->glyph->bitmap_left;
			
			for (x = 0; x < bitmap.width; x++) {
				char c = ~bitmap.buffer[y * bitmap.pitch + x] & 0xF8;
				*pixel = c << 8;
				pixel++;
			}
		}
		offset_x += face->glyph->advance.x >> 6;
		text++;
	}
}


int get_text_width(char* text)
{
	int w = 0;
	while (text[0]) {
		check_return(FT_Load_Char(face,
		                     text[0],
		                     FT_LOAD_DEFAULT));
		w += face->glyph->advance.x;
		text++;
	}
	return w >> 6; // Conversion to pixels.
}

void draw_ip(char* ip)
{
	FT_Set_Pixel_Sizes(face, 0, ip_font_size);
	
	draw_text((fb_width - get_text_width(ip)) / 2,
	          (fb_height + ip_font_size) / 2,
	          ip);
}

#define FACE_FOLDER "/sdcard/"

void load_face()
{	
	struct dirent *file;
	char path[sizeof(FACE_FOLDER) + sizeof("concours-999.tff") - 1] = FACE_FOLDER;
	DIR *dir = opendir(path);
	assert(dir != NULL);
	
	while ((file = readdir(dir)) != NULL) {
		if(sscanf(file->d_name, "concours-%i.ttf", &ip_font_size)) {
			assert(0 < ip_font_size && ip_font_size <= 999);
			break;
		}
	}
	strcat(path, (const char*)file->d_name);
	closedir(dir);
	
	check_return(FT_Init_FreeType(&library));
	check_return(FT_New_Face(library,
	                    path,
	                    0,
	                    &face));
}

void draw_pixel(int x, int y, int r, int g, int b)
{
	assert(0 <= x && x < fb_width);
	assert(0 <= y && y < fb_height);
	assert(0 <= r && r < 32);
	assert(0 <= g && g < 64);
	assert(0 <= b && b < 32);
	
	fb_map[x + y * fb_width] = r << 11 | g << 5 | b;
}


#define BUTTON_SIZE 40
void draw_button_up()
{
	int i;
	
	for(i = 1; i <= BUTTON_SIZE; i++) {
		draw_pixel(fb_width - i, fb_height - 5, 0, 0, 0);
		draw_pixel(fb_width - i, fb_height - BUTTON_SIZE, 0, 0, 0);
		draw_pixel(fb_width - 5, fb_height - i, 0, 0, 0);
		draw_pixel(fb_width - BUTTON_SIZE, fb_height - i, 0, 0, 0);
	}
}

void open_fb()
{
	fb_handle = open("/dev/fb0", O_RDWR);
	assert(fb_handle);
	
	struct fb_var_screeninfo info;
	check_return(ioctl(fb_handle, FBIOGET_VSCREENINFO, &info));
	fb_width  = info.xres;
	fb_height = info.yres;
	
	fb_map = (short*) mmap(0,
	                       fb_width * fb_height * 2 /* 16bpp */,
	                       PROT_READ | PROT_WRITE,
	                       MAP_SHARED,
	                       fb_handle,
	                       0);
	assert(fb_map);
}

char* get_ip()
{
	// http://stackoverflow.com/questions/2283494/get-ip-address-of-an-interface-on-linux
	struct ifreq ifr;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(fd != -1);
	
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
	
	check_return(ioctl(fd, SIOCGIFADDR, &ifr));
	close(fd);
	
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

void draw_team()
{
	FT_Set_Pixel_Sizes(face, ip_font_size, 0); // TODO: FT_Set_Pixel_Sizes instead
	draw_text(10, 20, "Caer & Poupine");
}

#include "../GUI-OLD/include/tslib.h"
int main(int argc, char **argv)
{
	open_fb();
	
	draw_red_background();
	
	load_face();
	
	draw_ip(get_ip());
	
	draw_button_up();
	
	draw_team();
	
	///////////
	
	int fd = open("/dev/touchscreen", O_RD);
	
	
	/*/////////
	struct tsdev* tsdev= ts_open( "/dev/touchscreen", 0);
	struct ts_sample samp;
	assert(tsdev);
	//check_return(ts_config(tsdev));	
	
	while(1) {
		int ret = ts_read(tsdev, &samp, 1);
		assert(ret >= 0);
		printf("x=%i, y=%i\n", samp.x, samp.y);
		if(ret == 0)
			usleep(5000);
	}
	// */
	
	return 0;
}

void rectangle(int x, int y, int w, int h, int r, int g, int b) 
{
	w += x;
	h += y;
	// TODO faux réinitialiser x
	for ( ; y < h; y++) {
		for ( ; x < w; x++) {
			#ifdef DEBUG
				set_pixel(a, b, R, G, B);
			#else
				fb_map[x + y * fb_width] = r << 11 | g << 5 | b;
			#endif
		}
	}
}
