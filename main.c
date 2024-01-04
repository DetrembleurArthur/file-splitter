#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
	const char *filename = NULL;
	const char *mode = NULL;
	char dynfilename[128] = {0};
	unsigned char *buffer = NULL;
	int splitSize = 0;
	FILE *fp = NULL;
	FILE *fpw = NULL;
	int i = 0;
	size_t size = 0;
	if(argc >= 3)
	{
		mkdir("./fs-build/");
		filename = argv[1];
		mode = argv[2];
		if(argc == 4)
			splitSize = atoi(argv[3]);
		printf("F:%s M:%s SS:%u\n", filename, mode, splitSize);
		if(!strcmp(mode, "split"))
		{
			if(splitSize > 0)
			{
				buffer = calloc(splitSize, sizeof(unsigned char));
				if(buffer)
				{
					fp = fopen(filename, "rb");
					if(fp)
					{
						fprintf(stderr, "%s open\n", filename);
						while((size = fread(buffer, sizeof(unsigned char), splitSize, fp)) > 0)
						{
							fprintf(stderr, "read %lu bytes in %s\n", size, filename);
							i++;
							sprintf(dynfilename, "./fs-build/%s-%d.splt", filename, i);
							fpw = fopen(dynfilename, "wb");
							if(fpw)
							{
								fprintf(stderr, "write %lu bytes in %s\n", size, dynfilename);
								if(fwrite(buffer, size, 1, fpw) != 1)
								{
									fprintf(stderr, "error at writing in %s\n", dynfilename);
									perror("!!!");
									fclose(fpw);
									break;
								}
								fclose(fpw);
							}
							else
							{
								perror("unable to open file in 'wb'");
							}
						}
						fclose(fp);
					}
					else
					{
						perror("unable to open file in 'rb'");
					}
					free(buffer);
					buffer = NULL;
				}
				else
				{
					perror("unable to prepare buffer");
				}
			}
			else
			{
				fprintf(stderr, "split size must be > 0, not %u\n", splitSize);
			}
		}
		else
		{
			sprintf(dynfilename, "./fs-build/%s", filename);
			fpw = fopen(dynfilename, "wb");
			if(fpw)
			{
				do
				{
					i++;
					sprintf(dynfilename, "./fs-build/%s-%d.splt", filename, i);
					fp = fopen(dynfilename, "rb");
					if(fp)
					{
						fseek(fp, 0L, SEEK_END);
						size = ftell(fp);
						fseek(fp, 0L, SEEK_SET);
						fprintf(stderr, "read %lu bytes in %s\n", size, dynfilename);
						buffer = calloc(size, sizeof(char));
						if(buffer)
						{
							if(fread(buffer, size, 1, fp) == 1)
							{
								fwrite(buffer, size, 1, fpw);
							}
							else
							{
								perror("unable to read file");
							}
							free(buffer);
							buffer = NULL;
						}
						else
						{
							perror("unable to prepare buffer");
						}
						fclose(fp);
					}
				} while(fp);
				fclose(fpw);
				fprintf(stderr, "%s rebuild\n", filename);
			}
			else
			{
				perror("unable to open in 'wb'");
			}
		}
	}
	else
	{
		fprintf(stderr, "fs <input filename> split <split size in bytes>\nfs <output filename> build\n");
	}
	return 0;
}