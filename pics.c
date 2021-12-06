#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct images {
	int c;
	char type[3];
	int n, m;
	int val_max;
	int ***p;
} image;

int ***alloc_array(int n, int m, int c)//alocare matrice cu 3 dimensiuni
{
	int ***a;
	a = (int ***)malloc(c * sizeof(int **));
	for (int k = 0; k < c; k++) {
		a[k] = (int **)malloc(n * sizeof(int *));
		for (int i = 0; i < n; i++)
			a[k][i] = (int *)malloc(m * sizeof(int));
	}
	return a;
}

void free_array(image pic)//eliberare memorie
{
	for (int k = 0; k < pic.c; k++)
		for (int i = 0; i < pic.n; i++)
			free(pic.p[k][i]);
	for (int k = 0; k < pic.c; k++)
		free(pic.p[k]);
	free(pic.p);
}

int minim(int a, int b)
{
	if (a > b)
		return b;
	return a;
}

int round_num(double x)
{
	if(x-(int)x >= 0.5)
		return (int)x + 1;
	return (int) x;
}

image load(FILE *f)//functie incarcare fisier nou
{
	image picture;
	char line[100];
	int ok = 3, loaded[3];
	unsigned char temp_pixel;
	for (int i = 0; i < 3; i++)
		loaded[i] = 0;
	while (ok > 0) {
		fgets(line, 100, f);
		if (line[0] == '#') {
			line[0] = '\0';
			fgets(line, 100, f);
		} else if (line[0] == 'P') {
			line[2] = '\0';
			strcpy(picture.type, line);
			loaded[0] = 1;
		} else if (loaded[1] == 0 && loaded[2] == 0) {
			char *p = strtok(line, " ");
			if (p != NULL && p[strlen(p) - 1] == '\n')
				p[strlen(p) - 1] = '\0';
			picture.m = atoi(p);
			p = strtok(NULL, " ");
			if (p != NULL && p[strlen(p) - 1] == '\n')
				p[strlen(p) - 1] = '\0';
			picture.n = atoi(p);
			loaded[1] = 1;
		} else if(loaded[2] == 0 && loaded[1] == 1) {
			picture.val_max = atoi(line);
		}
		ok--;
	}// citirea matricelor in functie de tip
	if (strcmp(picture.type, "P1") == 0 || strcmp(picture.type, "P2") == 0) {
		picture.c = 1;
		picture.p = (int ***)malloc(1 * sizeof(int **));
		picture.p[0] = (int **)malloc(picture.n * sizeof(int *));
		for (int i = 0; i < picture.n; i++)
			picture.p[0][i] = (int *)malloc(picture.m * sizeof(int));
		for (int i = 0; i < picture.n; i++)
			for (int j = 0; j < picture.m; j++)
				fscanf(f, "%d", &picture.p[0][i][j]);
	} else if (strcmp(picture.type, "P3") == 0) {
		picture.c = 3;
		picture.p = (int ***)malloc(3 * sizeof(int **));
		for (int k = 0; k < 3; k++) {
			picture.p[k] = (int **)malloc(picture.n * sizeof(int *));
			for (int i = 0; i < picture.n; i++)
				picture.p[k][i] = (int *)malloc(picture.m * sizeof(int));
		}
		for (int i = 0; i < picture.n; i++)
			for (int j = 0; j < picture.m; j++)
				for (int k = 0; k < 3; k++)
					fscanf(f, "%d", &picture.p[k][i][j]);
	} else if (strcmp(picture.type, "P4") == 0 || strcmp(picture.type, "P5") == 0) {
		picture.c = 1;
		picture.p = (int ***)malloc(1 * sizeof(int **));
		picture.p[0] = (int **)malloc(picture.n * sizeof(int *));
		for (int i = 0; i < picture.n; i++)
			picture.p[0][i] = (int *)malloc(picture.m * sizeof(int));
		for (int i = 0; i < picture.n; i++)
			for (int j = 0; j < picture.m; j++) {
				fread(&temp_pixel, 1, 1, f);
				picture.p[0][i][j] = temp_pixel;
			}
	} else if (strcmp(picture.type, "P6") == 0) {
		picture.c = 3;
		picture.p = (int ***)malloc(3 * sizeof(int **));
		for (int k = 0; k < 3; k++) {
			picture.p[k] = (int **)malloc(picture.n * sizeof(int *));
			for (int i = 0; i < picture.n; i++)
				picture.p[k][i] = (int *)malloc(picture.m * sizeof(int));
		}
		for (int i = 0; i < picture.n; i++)
			for (int j = 0; j < picture.m; j++)
				for (int k = 0; k < 3; k++) {
					fread(&temp_pixel, 1, 1, f);
					picture.p[k][i][j] = temp_pixel;
				}
	}
	return picture;
}

image sepia(image pic, int x1, int y1, int x2, int y2)
{//aplicare filtru sepia
	image pic1;
	int n_r = 0, n_g = 0, n_b = 0;
	pic1.c = pic.c;
	strcpy(pic1.type, pic.type);
	pic1.n = pic.n;
	pic1.m = pic.m;
	pic1.p = alloc_array(pic1.n, pic1.m, pic1.c);
	for (int i = 0; i < pic1.n; i++)
		for (int j = 0; j < pic1.m; j++)
			for(int k = 0; k < pic1.c; k++)
				pic1.p[k][i][j] = pic.p[k][i][j];
	for (int i = x1; i < x2 + 1; i++)
		for (int j = y1; j < y2 + 1; j++) {
			n_r = n_r + round_num(0.393 * pic.p[0][i][j]);
			n_r = n_r + round_num(0.769 * pic.p[1][i][j]);
			n_r = n_r + round_num(0.189 * pic.p[2][i][j]);
			pic1.p[0][i][j] = minim(n_r, pic.val_max);
		}
	for (int i = x1; i < x2 + 1; i++)
		for (int j = y1; j < y2 + 1; j++) {
			n_g = n_g + round_num(0.349 * pic.p[0][i][j]);
			n_g = n_g + round_num(0.686 * pic.p[1][i][j]);
			n_g = n_g + round_num(0.168 * pic.p[2][i][j]);
			pic1.p[1][i][j] = minim(n_g, pic.val_max);
		}
	for (int i = x1; i < x2 + 1; i++)
		for (int j = y1; j < y2 + 1; j++) {
			n_b = n_b + round_num(0.272 * pic.p[0][i][j]);
			n_b = n_b + round_num(0.534 * pic.p[1][i][j]);
			n_b = n_b + round_num(0.131 * pic.p[2][i][j]);
			pic1.p[2][i][j] = minim(n_b, pic.val_max);
		}
	printf("Sepia filter applied\n");
	free_array(pic);
	return pic1;
}

image grayscale(image pic, int x1, int y1, int x2, int y2)
{// aplicare filtru grayscale
	image pic1;
	pic1.c = pic.c;
	double temp;
	strcpy(pic1.type, pic.type);
	pic1.n = pic.n;
	pic1.m = pic.m;
	pic1.p = alloc_array(pic1.n, pic1.m, pic1.c);
	for (int i = 0; i < pic1.n; i++)
		for (int j = 0; j < pic1.m; j++)
			for(int k = 0; k < pic1.c; k++)
				pic1.p[k][i][j] = pic.p[k][i][j];
	for (int i = y1; i < y2 + 1; i++)
		for (int j = x1; j < x2 + 1; j++)
			for(int k = 0; k < pic1.c; k++) {
				temp = (pic.p[0][i][j] + pic.p[1][i][j] + pic.p[2][i][j]) / 3;
				pic1.p[k][i][j] = temp;
			}
	printf("Grayscale filter applied\n");
	free_array(pic);
	return pic1;
}

image rotate(image pic, int x1, int y1, int x2, int y2)
{
	image pic1;
	if (x1 == 0 && y1 == 0 && x2 == pic.m - 1 && y2 == pic.n - 1) {
		pic1.n = pic.m;// rotire pe intreaga imagine
		pic1.m = pic.n;
		strcpy(pic1.type, pic.type);
		pic1.c = pic.c;
		pic1.p = alloc_array(pic1.n, pic1.m, pic1.c);
		for (int p = 0; p < pic1.c; p++)
			for (int i = 0; i <= y2; i++)
				for (int j = 0; j <= x2; j++)
					pic1.p[p][j][y2 - i] = pic.p[p][i][j];
	} else {// rotire pe selectie
		pic1.n = pic.n;
		pic1.m = pic.m;
		strcpy(pic1.type, pic.type);
		pic1.c = pic.c;
		pic1.p = alloc_array(pic1.n, pic1.m, pic1.c);
		if (!(x1 == 0 && y1 == 0 && x2 == pic.m - 1 && y2 == pic.n - 1)) {
			for (int p = 0; p < pic1.c; p++)
				for (int i = 0; i < pic.n; i++)
					for (int j = 0; j < pic.m; j++)
						pic1.p[p][i][j] = pic.p[p][i][j];
		}
		for (int p = 0; p < pic1.c; p++)
			for (int i = 0; i <= y2 - y1; i++)
				for (int j = 0; j <= x2 - x1; j++)
					pic1.p[p][j + y1][x2 - i] = pic.p[p][i + y1][j + x1];
	}
	free_array(pic);
	return pic1;
}

image crop(image pic, int x1, int y1, int x2, int y2)
{//crop pe selectie
	image pic1;
	pic1.c = pic.c;
	strcpy(pic1.type, pic.type);
	pic1.n = y2 - y1 + 1;
	pic1.m = x2 - x1 + 1;
	pic1.p = alloc_array(pic1.n, pic1.m, pic1.c);
	for (int k = 0; k < pic.c; k++)
		for (int i = y1; i < y2 + 1; i++)
			for (int j = x1; j < x2 + 1; j++) {
				pic1.p[k][i - y1][j - x1] = pic.p[k][i][j];
			}
	printf("Image cropped\n");
	free_array(pic);
	return pic1;
}

void save(image pic, FILE *save, int ascii)
{//salvarea unui fisier in functie de tipul primit
	if (ascii == 1 && (strcmp(pic.type, "P4") == 0))
		strcpy(pic.type, "P1");
	if (ascii == 1 && (strcmp(pic.type, "P5") == 0))
		strcpy(pic.type, "P2");
	if (ascii == 1 && (strcmp(pic.type, "P6") == 0))
		strcpy(pic.type, "P3");
	if (ascii == 0 && (strcmp(pic.type, "P1") == 0))
		strcpy(pic.type, "P4");
	if (ascii == 0 && (strcmp(pic.type, "P2") == 0))
		strcpy(pic.type, "P5");
	if (ascii == 0 && (strcmp(pic.type, "P3") == 0))
		strcpy(pic.type, "P6");
	fprintf(save, "%s\n", pic.type);
	fprintf(save, "%d %d\n", pic.m, pic.n);
	if (strcmp(pic.type, "P2") == 0 || strcmp(pic.type, "P3") == 0 || strcmp(pic.type, "P5") == 0 || strcmp(pic.type, "P6") == 0)
		fprintf(save, "255\n");
	if (ascii == 1) {
		for (int i = 0; i < pic.n; i++) {
			for (int j = 0; j < pic.m; j++)
				for (int k = 0; k < pic.c; k++)
					fprintf(save, "%d ", pic.p[k][i][j]);
			fprintf(save, "\n");
		}
	} else {
		unsigned char temp;
		for (int i = 0; i < pic.n; i++)
			for (int j = 0; j < pic.m; j++) {
				for (int k = 0; k < pic.c; k++) {
					temp = pic.p[k][i][j];
					fwrite(&temp, 1, 1, save);
					//fseek(save, 1, SEEK_CUR);
				}
			}
	}
}

int main(void)
{
	int x1, x2, y1, y2, x1_t, x2_t, y1_t, y2_t, cnt = 0;
	char cmd[100], copy[100];
	image pic;
	pic.p = NULL;
	while(1) {
		fgets(cmd, 100, stdin);// citirea comenzii de la tastatura
		strcpy(copy, cmd);
		char *p = strtok(cmd, " ");
		if (p != NULL && p[strlen(p)-1] == '\n') {
			p[strlen(p) - 1] = '\0';
		}
		if (strcmp(p, "LOAD") == 0) {
			char *path = strtok(NULL, " ");
			if (path != NULL && path[strlen(path)-1] == '\n')
				path[strlen(path)-1] = '\0';
			FILE *f;
			if(cnt == 1)
					free_array(pic);
			f = fopen(path, "r");
			if (!f) {
				printf("Failed to load %s\n", path);
				cnt = 0;
				continue;
			}
			image pic_temp = load(f);
			if (f != NULL && pic_temp.p != NULL) {
				cnt = 1;
			}
			pic = pic_temp;
			fclose(f);
			printf("Loaded %s\n", path);
			x1 = 0;
			y1 = 0;
			x2 = pic.m - 1;
			y2 = pic.n - 1;
		} else if (strcmp(p, "SELECT") == 0) {
			if (cnt == 0) {
				printf("No image loaded\n");
				continue;
			} else {
				char *p1 = strtok(NULL, " ");
				if (p1 != NULL) {
					if (p1[strlen(p1) - 1] == '\n')
						p1[strlen(p1) - 1] = '\0';
					if (strcmp(p1, "ALL") == 0) {
						x1 = 0;
						y1 = 0;
						x2 = pic.m - 1;
						y2 = pic.n - 1;
						printf("Selected ALL\n");
						continue;
					} else {
						int ok = 1;
						for (int i = 0; p1[i] != '\0' && ok == 1; i++)
							if (!((p1[i] >= '0' && p1[i] <= '9') || p1[i] == '-'))
								ok = 0;
						if (ok == 1) {
							x1_t = atoi(p1);
						} else {
							printf("Invalid command\n");
							continue;
						}
						char *p2 = strtok(NULL, " \n");
						if (p2) {
							for (int i = 0; p2[i] != '\0' && ok == 1; i++)
								if (!((p2[i] >= '0' && p2[i] <= '9') || p2[i] == '-'))
									ok = 0;
							if (ok == 1) {
								y1_t = atoi(p2);
							}
						} else {
							printf("Invalid command\n");
							continue;
						}
						char *p3 = strtok(NULL, " \n");
						if(p3) {
							for (int i = 0; p3[i] != '\0' && ok == 1; i++)
								if (!((p3[i] >= '0' && p3[i] <= '9') || p3[i] == '-'))
									ok = 0;
							if (ok == 1) {
								x2_t = atoi(p3);
							}
						} else {
							printf("Invalid command\n");
							continue;
						}
						char *p4 = strtok(NULL, " \n");
						if(p4) {
							for (int i = 0; p4[i] != '\0' && ok == 1; i++)
								if (!((p4[i] >= '0' && p4[i] <= '9') || p4[i] == '-'))
									ok = 0;
							if (ok == 1) {
								y2_t = atoi(p4);
							}
						} else {
							printf("Invalid command\n");
							continue;
						}
						if (x1_t < 0 || x2_t < 0 || y1_t < 0 || y2_t < 0) {
							printf("Invalid set of coordinates\n");
							continue;
						}
						if (x1_t > pic.m || x2_t > pic.m || y1_t > pic.n || y2_t > pic.n) {
							printf("Invalid set of coordinates\n");
							continue;
						}
						x1 = (x1_t < x2_t) ? x1_t : x2_t;
						x2 = (x1_t > x2_t) ? x1_t - 1 : x2_t - 1;
						y1 = (y1_t < y2_t) ? y1_t : y2_t;
						y2 = (y1_t > y2_t) ? y1_t - 1 : y2_t - 1;
						printf("Selected %d %d %d %d\n", x1, y1, x2 + 1, y2 + 1);
					}
				}
			}
		} else if (strcmp(p, "ROTATE") == 0) {
			if (cnt == 0) {
				printf("No image loaded\n");
				continue;
			} else {
				char *ang = strtok(NULL, " ");
				if (ang != NULL && ang[strlen(ang) - 1] == '\n')
					ang[strlen(ang) - 1] = '\0';
				int angle = atoi(ang);
				if ((x2 - x1) != (y2 - y1) && !(x1 == 0 && y1 == 0 && x2 == pic.m - 1 && y2 == pic.n - 1)) {
					printf("The selection must be square\n");
					continue;
				}
				if (angle % 90 != 0) {
					printf("Unsupported rotation angle\n");
					continue;
				}
				printf("Rotated %d\n", angle);
				if (angle < 0)
					angle = angle + 360;
				int nr = angle / 90;
				int all = 0;
				if (x1 == 0 && y1 == 0 && x2 == pic.m - 1 && y2 == pic.n - 1)
					all = 1;
				for (int i = 0; i < nr; i++) {
					pic = rotate(pic, x1, y1, x2, y2);
					if (all == 1) {
						x1 = 0;
						y1 = 0;
						x2 = pic.m - 1;
						y2 = pic.n - 1;
					}
				}
			}
		} else if (strcmp(p, "CROP") == 0) {
			if (cnt == 0) {
				printf("No image loaded\n");
				continue;
			} else {
				pic = crop(pic, x1, y1, x2, y2);
				x1 = 0;
				y1 = 0;
				x2 = pic.m - 1;
				y2 = pic.n - 1;
			}
		} else if (strcmp(p, "SEPIA") == 0) {
			if (cnt == 0) {
				printf("No image loaded\n");
				continue;
			} else {
				if (pic.c == 1) {
					printf("Sepia filter not available\n");
					continue;
				} else
					pic = sepia(pic, x1, y1, x2, y2);
			}
		} else if (strcmp(p, "GRAYSCALE") == 0) {
			if (cnt == 0) {
				printf("No image loaded\n");
				continue;
			} else {
				if (pic.c == 1) {
					printf("Grayscale filter not available\n");
					continue;
				} else
					pic = grayscale(pic, x1, y1, x2, y2);
			}
		} else if (strcmp(p, "SAVE") == 0) {
			if (cnt == 0) {
				printf("No image loaded\n");
				continue;
			} else {
				FILE *s;
				int ascii = 0;
				char *path = strtok(NULL, " ");
				if (path != NULL && path[strlen(path) - 1] == '\n')
					path[strlen(path) - 1] = '\0';
				printf("Saved %s\n", path);
				char *c = strtok(NULL, " ");
				if (c && c[strlen(c) - 1] == '\n')
					c[strlen(c) - 1] = '\0';
				if (c && strcmp(c, "ascii") == 0)
					ascii = 1;
				s = fopen(path, "w");
				if (!s)
					continue;
				save(pic, s, ascii);
				fclose(s);
			}
		} else if (strcmp(p, "EXIT") == 0) {//iesire glorioasa din fisier
			free_array(pic);
			return 0;
		} else
			printf("Invalid command\n");
	}
}
