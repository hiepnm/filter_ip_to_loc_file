/*
 * main.cpp
 *
 *  Created on: May 9, 2014
 *      Author: hiepnm
 */


#include <map>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
struct location_t {
	int regnum;
	char regid[3];
	char regname[57];
};
typedef struct location_t location;
std::map<std::string, location> loc_map;

int collect_data_old_file(const char* file_name, char** sz_error) {
	sz_error = NULL;
	FILE* fptr = fopen(file_name, "r");
	if (!fptr) {
//		*sz_error = (char*)malloc(strlen(strerror(errno)) + 1);
//		strcpy(*sz_error, strerror(errno));
		return -1;
	}
	char line_buffer[BUFSIZ]; /* BUFSIZ is defined if you include stdio.h */
	while (fgets(line_buffer, sizeof(line_buffer), fptr)) {
		location* l = (location*)malloc(sizeof(location));
		sscanf(line_buffer, "%c%c\t%d\t%s", &l->regid[0], &l->regid[1], &l->regnum, l->regname);
		l->regid[2] = '\0';
		loc_map.insert(std::pair<std::string, location>(std::string(l->regid), *l));
	}
	fclose(fptr);
	return 0;
}
static int count = 101;
int collect_data_new_file(const char* file_name, char** sz_error) {
	sz_error = NULL;
	FILE* fptr = fopen(file_name, "r");
	if (!fptr) {
		*sz_error = (char*)malloc(strlen(strerror(errno)) + 1);
		strcpy(*sz_error, strerror(errno));
		return -1;
	}
	char line_buffer[BUFSIZ]; /* BUFSIZ is defined if you include stdio.h */
	char name[57];
	int l1,l2,l3,l4,u1,u2,u3,u4;
	unsigned int lower, upper;
	while (fgets(line_buffer, sizeof(line_buffer), fptr)) {
		char regid[3];
		sscanf(line_buffer, "%d.%d.%d.%d,%d.%d.%d.%d,%d,%d,%c%c,%[0-9a-zA-Z ,\"]", &l1,&l2,&l3,&l4, &u1,&u2,&u3,&u4, &lower, &upper, &regid[0], &regid[1], name);
		regid[2]='\0';
		std::string key(regid);
		if (loc_map.find(key) == loc_map.end()) {
			location* l = (location*)malloc(sizeof(location));
			count++;
			l->regnum = count;
			strcpy(l->regid, regid);
			strcpy(l->regname, name);
			loc_map.insert(std::pair<std::string, location> (key, *l));
		} else {
		}
	}
	fclose(fptr);
	return 0;
}
int flush_data(const char* file_name, char** sz_error) {
	sz_error = NULL;
	FILE* fptr = fopen(file_name, "w");
	if (!fptr) {
		*sz_error = (char*)malloc(strlen(strerror(errno)) + 1);
		strcpy(*sz_error, strerror(errno));
		return -1;
	}
	std::map<std::string, location>::iterator it;
	for(it = loc_map.begin(); it != loc_map.end(); it++) {
		fprintf(fptr, "%c%c,%d,%s\n", it->second.regid[0], it->second.regid[1], it->second.regnum, it->second.regname);
	}
	fclose(fptr);
	return 0;
}
/**
 * argv[1]: old file: %c%c\t%d\t%s
 * argv[2]: file has new data
 * argv[3]: new file
 */
int main(int argc, char **argv) {
	//doc argv[1] => insert vao map
	//doc argv[2] => insert vao map
	//ghi ra argv[3]
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <old_file> <new_file_data> <flush_file> \n", argv[0]);
		return -1;
	}

	char* sz_error = NULL;
	int rc = collect_data_old_file(argv[1], &sz_error);
	if (rc == -1 && sz_error != NULL) {
		fprintf(stderr, "%s", sz_error);
		free(sz_error);
	}

	char* sz_error2 = NULL;
	rc = collect_data_new_file(argv[2], &sz_error2);
	if (rc == -1 && sz_error2 != NULL) {
		fprintf(stderr, "%s", sz_error2);
		free(sz_error2);
		return -1;
	}

	char* sz_error3 = NULL;
	rc = flush_data(argv[3], &sz_error3);
	if (rc == -1 && sz_error3 != NULL) {
		fprintf(stderr, "%s", sz_error3);
		free(sz_error3);
		return -1;
	}
}


