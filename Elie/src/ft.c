#include "ft.h"

int FT_init(GUI* gui) {

	if (FT_Init_FreeType(&gui->ft_library)) {
		printf("Error : Failed to initialise FreeType\n");
		return -1;
	}
	
	// Couleur par defaut : blanc
	FT_setColor(gui, 31, 63, 31);
	
	return 0;
	
}

void FT_setColor(GUI* gui, int R, int G, int B) {
	
	gui->ft_color_r = R;
	gui->ft_color_g = G;
	gui->ft_color_b = B;
	
}

int FT_setFont(GUI* gui, char* font) {
	
	gui->ft_font = font;
	int error = FT_New_Face(gui->ft_library, font, 0, &gui->ft_face);
	if (error == FT_Err_Unknown_File_Format) {
		printf("Error : Unsupported font format (%s)\n", font);
		return -1;
	} else if (error) {
		printf("Error : reading font from the file failed (%s)\n", font);
		return -1;
	} else {
		return 0;
	}
}

int FT_setSize(GUI* gui, int size) {
	
	int error = FT_Set_Char_Size(gui->ft_face, /* handle to face object */
		0, /* char_width in 1/64th of points */
		size*64, /* char_height in 1/64th of points */
		100, /* horizontal device resolution */
		100 ); /* vertical device resolution */
	if (error) {
		printf("Error : setting char size failed\n");
		return -1;
	} else {
		return 0;
	}
	
}

int FT_getSizeString(GUI* gui, char* str, int* size) {
	
	int c = 0;
	size[0] = 0; // advance = width
	size[1] = 0; // height
	
	while (str[c] != '\0') {
		
		FT_UInt glyph_index = FT_Get_Char_Index (gui->ft_face, str[c]);
		FT_Load_Glyph (gui->ft_face, glyph_index, FT_LOAD_DEFAULT);
		size[0] += gui->ft_face->glyph->advance.x >> 6;
		c++;
	}
	size[1] = gui->ft_face->glyph->metrics.horiBearingY >> 6;
	
	//~ printf("\tString size : %i×%i (%s)\n", size[0], size[1], str);
	
	return 0;
	
}

int FT_printString(GUI* gui, char* str, int X, int Y) {
	
	if (X == -1 || Y == -1) {
		int size[2];
		FT_getSizeString(gui, str, size);
		if (X == -1) {
			X = (gui->fb_var_info.xres-size[0])/2;
		}
		if (Y == -1) {
			Y = (gui->fb_var_info.yres-size[1])/2+size[1];
		}
	}
	
	FT_Bitmap *bitmap = &gui->ft_face->glyph->bitmap;
	
	int c = 0;
	int x, y, location;
	int advance = 0;
	
	while (str[c] != '\0') {
		
		FT_UInt glyph_index = FT_Get_Char_Index (gui->ft_face, str[c]);
		FT_Load_Glyph (gui->ft_face, glyph_index, FT_LOAD_DEFAULT);
		int error = FT_Render_Glyph (gui->ft_face->glyph, FT_RENDER_MODE_NORMAL);
		if (error) {
			printf("Error rendering glyph (%c)\n", str[c]);
			return -1;
		}
			
		for (y = 0; y < bitmap->rows; y++) {
			for (x = 0; x < bitmap->width; x++) {
				location = ((X + x + advance + gui->ft_face->glyph->bitmap_left)+gui->fb_var_info.xoffset) * (gui->fb_var_info.bits_per_pixel/8)
								+ ((Y + y - gui->ft_face->glyph->bitmap_top)+gui->fb_var_info.yoffset) * gui->fb_fix_info.line_length;

				char grayval = *(bitmap->buffer + (y * bitmap->pitch) + x);
				if (grayval > 64) {
					unsigned short int t = gui->ft_color_r<<11 | gui->ft_color_g << 5 | gui->ft_color_b;
					*((unsigned short int*)(gui->fb_buffer + location)) = t;
				}
			}
		}
		advance += gui->ft_face->glyph->advance.x >> 6;
		c++;
	}
	
	return advance;
	
}
