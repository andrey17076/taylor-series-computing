#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


int main (int argc, char const *argv[]) {
    char util_name[FILENAME_MAX];
    int tmp_file;
    pid_t pid;

    strcpy(util_name, (char*) basename(argv[0]));

    if (argc != 3) {
        fprintf(stderr, "%s: Wrong number of arguments\n", util_name);
        return -1;
    }

    if (argv[1] < 0 || argv[2] < 0) {
        fprintf(stderr,"%s: Options must be positive\n", util_name);
        return -1;
    }

    if ((tmp_file = open("/tmp/tmp.txt", O_RDWR | O_CREAT | O_TRUNC, 0777)) == 0) {
        fprintf(stderr, "%s: /tmp/tmp.txt: %s\n", util_name, strerror(errno));
        return -1;
    }

    int number_of_members = atoi(argv[1]);
    int set_length = atoi(argv[2]);
    int i, j, k;

    for (i = 0; i < number_of_members; i++) {
        double sum = 0;
        double x = 6.28 * i / number_of_members;
        for (j = 0; j < set_length; j++) {
            pid = fork ();
            if (pid == 0) {
                double member = (j % 2) ? -1 : 1;
                for (k = 1; k <= 2 * j + 1; k++)
                    member *= x/k;
                char buffer[64];
                sprintf(buffer, "%d %d %f\n", getpid(), i, member);
                printf("%s\n", buffer);
                write(tmp_file, buffer, 64);
                exit(0);
            }
        }
    }


    close(tmp_file);
}
