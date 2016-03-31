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

double get_x(double index, double number_of_members) {
    double x =  2 * PI * index / number_of_members;
    return (x > PI) ? PI - x : x;
}

FILE *get_tmp_file() {
    const char *tmp_fname = "/tmp/tmp.txt";
    FILE *tmp_file;
    if ((tmp_file = fopen(tmp_fname, "w+r")) == NULL) {
        fprintf(stderr, "%s: %s: %s\n", util_name, tmp_fname, strerror(errno));
        exit(-1);
    }
    return tmp_file;
}

void count_taylor_set_elements(double number_of_members,
    double taylor_set_length,
    FILE* tmp_file) {

    int i, j, k;
    double x, set_element;
    pid_t pid;
    int running_processes = 0;

    for (i = 0; i < number_of_members; i++) {
        x = get_x(i, number_of_members);
        for (j = 0; j < taylor_set_length; j++) {
            if (running_processes == taylor_set_length){
                wait(NULL);
                running_processes--;
            }

            pid = fork ();
            if (pid == 0) {
                set_element = (j % 2) ? -1 : 1;
                for (k = 1; k <= 2 * j + 1; set_element *= x/k++);
                printf("%d %d %f\n", getpid(), i, set_element);
                fprintf(tmp_file, "%d %d %f\n", getpid(), i, set_element);
                exit(0);
            } else if (pid == -1) {
                fprintf(stderr, "%s: %s\n", util_name, strerror(errno));
                exit(-1);
            }
            running_processes++;
        }
    }
    while (wait(NULL) > 0){};
}

FILE *get_result_file(const char *arg_param) {
    char res_fname[FILENAME_MAX];
    realpath(arg_param, res_fname);
    FILE *res_file;

    if ((res_file = fopen(res_fname, "w+")) == NULL) {
        fprintf(stderr, "%s: %s: %s\n", util_name, res_fname, strerror(errno));
        exit(-1);
    }
    return res_file;
}

void print_result(int number_of_members, FILE* tmp_file, FILE* result_file) {
    int i;
    pid_t pid;
    double *members, set_element;
    members = malloc(sizeof(double)*number_of_members);
    rewind(tmp_file);

    while (!feof(tmp_file)) {
        fscanf(tmp_file, "%d %d %lf\n", &pid, &i, &set_element);
        members[i] += set_element;
    }
    members[i] -= set_element; //because last line was read twice

    for (i = 0; i < number_of_members; i++) {
        fprintf(result_file, "y[%d] = %f\n", i, members[i]);
    }
    free(members);
}

int main (int argc, char const *argv[]) {
    strcpy(util_name, (char*) basename(argv[0]));
    int number_of_members, taylor_set_length;

    if (argc != ARG_NUMB)
        print_err("Wrong number of arguments");
    if ((number_of_members = atoi(argv[1])) == 0)
        print_err("Number of members must be positive");
    if ((taylor_set_length = atoi(argv[2])) == 0)
        print_err("Taylor set length must be positive");

    FILE *tmp_file = get_tmp_file();
    count_taylor_set_elements(number_of_members, taylor_set_length, tmp_file);
    FILE *result_file = get_result_file(argv[3]);
    print_result(number_of_members, tmp_file, result_file);

    if (fclose(tmp_file) == -1) {
        fprintf(stderr, "%s: %s\n", util_name, strerror(errno));
    };
    if (fclose(result_file) == -1) {
        fprintf(stderr, "%s: %s\n", util_name, strerror(errno));
    };
}
