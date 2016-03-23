#include <string.h>
#include <unistd.h>

int num=0;
char namebuf[20];
char prefix[] = "/tmp/tmp";

char *gentemp(void) {
    int length;
    pid_t pid;
    pid=getpid();
    strcpy(namebuf, prefix);
    length = strlen(namebuf);
    itoa(pid, &namebuf[length]);
    strcat(namebuf,".");
    length = strlen(namebuf);
    do{
        itoa(num++,&namebuf[length]);
    } while (access(namebuf, F_OK) != -1);
    return (namebuf);
}

int itoa(int i; char *string) {
    int power, j;
    j = i;
    for (power = 1; j >= 10; j /= 10)
        power *= 10;
    for ( ; power > j; power/=10) {
        *string ++= '0' + i / power;
        i %= power;
    }
    *string = '\0';
}
