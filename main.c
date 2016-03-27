#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PI 3.14
#define ARG_NUMB 4

char util_name[FILENAME_MAX];

void print_err(const char *error_msg) {
    fprintf(stderr, "%s: %s\n", util_name, error_msg);
    exit(-1);
}

int main (int argc, char const *argv[]) {

    strcpy(util_name, (char*) basename(argv[0]));
    int number_of_members, set_length;

    if (argc != ARG_NUMB)
        print_err("Wrong number of arguments");

    if ((number_of_members = atoi(argv[1])) == 0)
        print_err("Number of members must be positive");

    if ((set_length = atoi(argv[2])) == 0)
        print_err("Taylor set length must be positive");

    const char *tmp_filename = "/tmp/tmp.txt";
    FILE *tmp_file;

    if ((tmp_file = fopen(tmp_filename, "w+r")) == NULL) {
        fprintf(stderr, "%s: %s: %s\n",
            util_name,
            tmp_filename,
            strerror(errno)
        );
        return -1;
    }

    int i, j, k;
    double x, member;
    pid_t pid;

    for (i = 0; i < number_of_members; i++) {
        x = (2 * PI * i / number_of_members);
        if (x > PI) {
            x = PI - x;
        }
        for (j = 0; j < set_length; j++) {
            pid = fork ();
            if (pid == 0) {
                member = (j % 2) ? -1 : 1;
                for (k = 1; k <= 2 * j + 1; member *= x/k++);
                printf("%d %d %f\n", getpid(), i, member);
                fprintf(tmp_file, "%d %d %f\n", getpid(), i, member);
                exit(0);
            } else if (pid > 0) {
                waitpid(pid);
            } else {
                fprintf(stderr, "%s: %s\n", util_name, strerror(errno));
            }
        }
    }

    char result_filename[FILENAME_MAX];
    FILE *result_file;
    realpath(argv[3], result_filename);

    if ((result_file = fopen(result_filename, "w+")) == NULL) {
        fprintf(stderr, "%s: %s: %s\n",
            util_name,
            result_filename,
            strerror(errno));
        return -1;
    }

    rewind(tmp_file);
    double *members;
    members = malloc(sizeof(double)*number_of_members);

    while (!feof(tmp_file)) {
        fscanf(tmp_file, "%d %d %lf", &pid, &i, &member);
        members[i] += member;
    }

    for (i = 0; i < number_of_members; i++) {
        fprintf(result_file, "y[%d] = %f\n", i, members[i]);
    }

    free(members);
    fclose(tmp_file);
    fclose(result_file);
}
