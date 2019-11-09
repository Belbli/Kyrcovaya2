#include "FilesFuntions.h"

void saveProgress(int availableLvls) {
		FILE *file = fopen("progress.txt", "w+");
		if (file != NULL) {
			fprintf(file, "%d", availableLvls);
			fclose(file);
		}
	}

int readFile(char *path) {
	int output = -1;
	FILE *file = fopen(path, "r");
	if (file != NULL) {
		while (fscanf(file, "%d", &output)) {
			fclose(file);
			return output;
		}
	}
	return output;
}
