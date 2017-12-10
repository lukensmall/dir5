#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFSIZE 100000

long beforeandafter = 1000;

char *buff;



// Faster strlcpy
// but no return value
// ++variable is faster than variable++
void strmcpy(char *dst,
       const char *src, size_t siz)
{
	if( siz == 0 )
		return;
		
	if( --siz == 0 )
	{
		*dst = '\0';
		return;
	}
	else
	{
		if (( *dst = *src ) == '\0')
			return;
	}

	// Copy as many bytes as will fit 
	while ( --siz )
	{
		if (( *++dst = *++src ) == '\0')
			return;
	}

	// Not enough room in dst, add NUL
	*++dst = '\0';		// NUL-terminate dst
	
}

// strlcat without the return value and its associated calculations and some optimizations
void strmcat(char *dst, const char *src, size_t dsize)
{ 
	const char *odst = dst;
	size_t n = dsize;
 
	/* Find the end of dst and adjust bytes left but don't go past end. */
	if(n != 0 && *dst != '\0')
		while (--n != 0 && *++dst != '\0');
		
	n = dsize - (dst - odst);

	if (n-- == 0)
		return;
	while (*src != '\0')
	{
		if (n != 0)
		{
			*dst++ = *src;
			--n;
		}
		else
			break;
		++src;
	}
	*dst = '\0';
}


ulong filesize(const char* a)
{
	FILE* infile;
	ulong b;
	if(!(infile = fopen(a, "a")))
		return 0;
		
	b = ftell(infile);
	fclose(infile);
	return b;
}

int filesearch( const char* searchstring, const char* a)
{
	FILE* infile;
	ulong fileLength;
	if(!(fileLength = filesize(a)))
		return 0;
	infile = fopen(a, "r");
	long j, k = -1, g = 0, t = 0, localFL;
	ulong before = beforeandafter, after = beforeandafter;
	char *buffer;
	buffer = (char*)malloc(before + 1);
	if (buffer == NULL) perror("malloc");
	int setlength = 0;
	int ssize = strlen(searchstring);
	int setindex[ssize + 1];
	int line = 1;
	
	if(fileLength < BUFFSIZE)
	{
		if(!fileLength)
		{
			free(buffer);
			fclose(infile);
			return 0;
		}
		
		localFL = fileLength;
		fread(buff, 1, fileLength, infile);
		fileLength = 0;
	}
	else
	{
		fileLength -= BUFFSIZE;
		fread(buff, 1, BUFFSIZE, infile);
		localFL = BUFFSIZE;
	}
	
	skip:
		buffer[++k % before] = buff[t];
		if(buff[t] == '\n')
			++line;
		++g;
		for(j = 0; j < setlength; ++j)
		{
			if(buff[t] == searchstring[setindex[j]++])
			{
				if(setindex[j] == ssize)
				{
					printf("*---------------------------*\n");
					printf("%s         line: %d\n\n", a, line);
					
					if(g >= (long)before)
					{
						buffer[before] = '\0';
						k = (k + 1) % before;
						
						printf("%s", buffer + k);
						
						buffer[k] = '\0';
						printf("%s", buffer);
					}
					else
					{
						buffer[g] = '\0';
						printf("%s", buffer);
					}


					if(++t == localFL)
					{
						if(after > BUFFSIZE)
							after = BUFFSIZE;
							
						if(fileLength < after)
						{
							if(!fileLength)
							{
								free(buffer);
								fclose(infile);
								return 0;
							}
							
							localFL = fileLength;
							fread(buff, 1, fileLength, infile);
							fileLength = 0;
						}
						else
						{
							fileLength -= after;
							fread(buff, 1, after, infile);
							localFL = after;
						}
						buff[localFL] = (char)0;
						t = 0;
					}
					
					
					g = 0;
					while(g < (long)after)
					{
						printf("%c", buff[t]);
						++g;
						
						
						if(++t == localFL)
						{
							if(after > BUFFSIZE)
								after = BUFFSIZE;
								
							if(fileLength < after)
							{
								if(!fileLength)
								{
									free(buffer);
									fclose(infile);
									return 0;
								}
								
								localFL = fileLength;
								fread(buff, 1, fileLength, infile);
								fileLength = 0;
							}
							else
							{
								fileLength -= after;
								fread(buff, 1, after, infile);
								localFL = after;
							}
							buff[localFL] = (char)0;
							t = 0;
						}
						
					}
					printf("\n");
					free(buffer);
					fclose(infile);
					return 1;
				}
			}
			else
				setindex[j--] = setindex[--setlength];
		}
		
		if(buff[t] == *searchstring)
			setindex[setlength++] = 1;
			
			

		if(++t == localFL)
		{
			if(fileLength < BUFFSIZE)
			{
				if(!fileLength)
				{
					free(buffer);
					fclose(infile);
					return 0;
				}
				
				localFL = fileLength;
				fread(buff, 1, fileLength, infile);
				fileLength = 0;
			}
			else
			{
				fileLength -= BUFFSIZE;
				fread(buff, 1, BUFFSIZE, infile);
				localFL = BUFFSIZE;
			}
			t = 0;
		}
	goto skip;
	free(buffer);
	return 0;
}

int recur(const char* dir0, const char* file, int level, const char* searchstring)
{
	char *dir, *extended;
	int ret;
	
	dir = (char*)malloc(PATH_MAX + 1);
	if (dir == NULL) perror("malloc");
	
	extended = (char*)malloc(PATH_MAX + 1);
	if (extended == NULL) perror("malloc");

	strmcpy(dir, dir0, PATH_MAX + 1);
	if(level == 1)
	{
		strmcat(dir, file, PATH_MAX + 1);
		strmcat(dir, "/", PATH_MAX + 1);
	}

	DIR *dp;
	struct dirent *ep;

	dp = opendir (dir);
	if(dp != NULL)
	{
		readdir(dp);
		readdir(dp);
		while(!(!(ep = readdir(dp))))
		{
			strmcpy(extended, dir, PATH_MAX + 1);
			strmcat(extended, ep->d_name, PATH_MAX + 1);
			filesearch(searchstring, extended);

			recur(dir, ep->d_name, 1, searchstring);
		}

		(void) closedir (dp);
		ret = 0;
	}
	else
	{
		if(level == 0)
			perror("Couldn\'t open the directory");
		ret = 1;
	}
	
	free(dir);
	free(extended);

	return ret;
}

// argv[1] is a directory, argv[2] is a filename searchstring
int main (int argc, char** argv)
{
	int j, k;
	char searchstring[PATH_MAX + 1];
	char dir[PATH_MAX + 1];

	if(argc <= 2)
	{
		printf("Usage: %s [search-folder] [in-file-search string] optional:[output-length]\n", argv[0]);
		return 0;
	}
	else
		strmcpy(dir, argv[1], PATH_MAX);


	j = -1;
	while(dir[++j]);
	if(dir[j-1] != '/')
		strmcat(dir, "/", PATH_MAX);


	if(argc > 3)
	{
		if(argc > 4)
		{
			perror("Too many arguments");
			return 1;
		}
		j = strlen(argv[3]);
		while(j--)
		{
			if(argv[3][j] < '0' || argv[3][j] > '9')
			{
				perror("Digits only in argument 4\n");
				return 1;
			}
		}
		beforeandafter = 0;
		j = -1;
		k = strlen(argv[3]);
		while( ++j < k )
			beforeandafter = beforeandafter * 10 + (int)(argv[3][j] - '0');
			
		if(beforeandafter == 0)
		{
			perror("Argument 4 must be > 0\n");
			return 1;
		}
	}
	strmcpy(searchstring, argv[2], PATH_MAX);


	buff = (char*)malloc(BUFFSIZE + 1);
	if (buff == NULL) perror("malloc");

	recur(dir, "", 0, searchstring);
	free(buff);

	printf("\n\n%s:\n%s:\n", searchstring, dir);

	return 0;
}
