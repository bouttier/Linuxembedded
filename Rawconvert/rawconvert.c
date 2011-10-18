#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
	
	int input = open("fb.raw", O_RDONLY);
	printf("i : %i\n", input);
	int output = open("fbbis.raw", O_WRONLY);
	printf("o : %i\n", output);
	
	unsigned short r;
	int i = 0;
	char c;
	while (read(input, (char*)&r, 2) > 0) {
		
		c = (r & 0xF800)>>8;
		write(output, &c, 1);
		c = (r & 0x07E0)>>3;
		write(output, &c, 1);
		c = (r & 0x001F)<<3;
		write(output, &c, 1);
		i++;
		
	}
	printf("%i octets écrit\n", i*3);
	
	close(input);
	close(output);
	
}
