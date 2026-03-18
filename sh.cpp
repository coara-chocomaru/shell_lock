#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    pid_t ppid = getppid();
    char comm[16] = {0};
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/comm", (int)ppid);
    int fd = open(path, O_RDONLY);
    int is_adbd = 0;
    if (fd >= 0) {
        ssize_t len = read(fd, comm, sizeof(comm)-1);
        close(fd);
        if (len > 0) {
            comm[len] = '\0';
            char *nl = strchr(comm, '\n');
            if (nl) *nl = '\0';
            if (strcmp(comm, "adbd") == 0) {
                is_adbd = 1;
            }
        }
    }

    setenv("TERM", "xterm-256color", 1);
    chdir("/");
    if (is_adbd && !isatty(0)) {
        printf("Error: Non-interactive adb shell commands are blocked for security.\n");
        printf("Use 'adb shell' (interactive) and enter password instead.\n");
        exit(1);
    }

    if (is_adbd && isatty(0)) {
        int skip_password = 0;
        FILE *fp_prop = popen("/system/bin/getprop coara.shell.check", "r");
        if (fp_prop) {
            char val[32] = {0};
            if (fgets(val, sizeof(val), fp_prop)) {
                size_t len = strlen(val);
                if (len > 0 && val[len-1] == '\n') val[len-1] = '\0';
                if (strcmp(val, "0") == 0) skip_password = 1;
            }
            pclose(fp_prop);
        }

        if (!skip_password) {
            printf("login password: ");
            char pass[16] = {0};
            if (fgets(pass, sizeof(pass), stdin) == NULL) {
                printf("\nNo input\n");
                exit(1);
            }
            size_t plen = strlen(pass);
            if (plen > 0 && pass[plen-1] == '\n') pass[plen-1] = '\0';
            if (strcmp(pass, "01234") != 0) {
                printf("\nLogin failed \n");
                exit(1);
            }
        }

        char hostname[64] = "android";
        FILE *fp = popen("/system/bin/getprop ro.product.device", "r");
        if (fp) {
            if (fgets(hostname, sizeof(hostname), fp)) {
                size_t len = strlen(hostname);
                if (len > 0 && hostname[len-1] == '\n') hostname[len-1] = '\0';
            }
            pclose(fp);
        }

        char custom[128];
        snprintf(custom, sizeof(custom), "/data/local/tmp/mkshrc_adbd");
        FILE *f = fopen(custom, "w");
        if (f) {
            fprintf(f,
                "HOSTNAME='%s'\n"
                ". /system/etc/mkshrc\n"
                "PS4='[$EPOCHREALTIME] '\n"
                "PS1='${|\n"
                "\tlocal e=$?\n"
                "\t(( e )) && REPLY+=\"$e|\"\n"
                "\treturn $e\n"
                "}$HOSTNAME:${PWD:-?} $ '\n",
                hostname);
            fclose(f);
            setenv("ENV", custom, 1);
        } else {
            setenv("ENV", "/system/etc/mkshrc", 1);
        }

        char **new_argv = (char**)malloc((argc + 1) * sizeof(char*));
        new_argv[0] = "-sh";
        for(int i = 1; i < argc; i++) new_argv[i] = argv[i];
        new_argv[argc] = NULL;

        execv("/system/bin/xh", new_argv);
        perror("sh exec failed");
        free(new_argv);
        exit(1);
    }

    setenv("ENV", "/system/etc/mkshrc", 1);
    char **new_argv = (char**)malloc((argc + 1) * sizeof(char*));
    new_argv[0] = "sh";
    for(int i = 1; i < argc; i++) new_argv[i] = argv[i];
    new_argv[argc] = NULL;
    execv("/system/bin/xh", new_argv);
    perror("sh exec failed");
    free(new_argv);
    exit(1);
}
