#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
static volatile sig_atomic_t stopping = 0;
static pid_t child_pid = -1;

static void handle_signal(int sig) {
    if (sig == SIGTERM || sig == SIGINT || sig == SIGQUIT) stopping = 1;
    if (sig == SIGCHLD) { }
}

static void install_handlers(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) < 0 || sigaction(SIGINT, &sa, NULL) < 0 ||
        sigaction(SIGQUIT, &sa, NULL) < 0 || sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sheen-init: sigaction"); exit(1);
    }
}

static void mount_early(const char *source, const char *target, const char *fstype, unsigned long flags) {
    if (mount(source, target, fstype, flags, NULL) < 0 && errno != EBUSY) {
        fprintf(stderr, "sheen-init: mount %s on %s failed: %s\\n", fstype, target, strerror(errno));
    }
}

static void reap_children(void) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) { }
}

static pid_t start_shell(void) {
    pid_t pid = fork();
    if (pid < 0) { perror("sheen-init: fork"); return -1; }
    if (pid == 0) {
        execl("/bin/sh", "sh", "-i", (char *)NULL);
        perror("sheen-init: exec /bin/sh");
        _exit(127);
    }
    return pid;
}

int main(void) {
    if (getpid() != 1) { fprintf(stderr, "sheen-init: must run as PID 1\\n"); return 1; }
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    install_handlers();

    mkdir("/dev", 0755); mkdir("/dev/pts", 0755); mkdir("/proc", 0555); mkdir("/sys", 0555); mkdir("/run", 0755);
    mount_early("proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC);
    mount_early("sysfs", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC);
    mount_early("devtmpfs", "/dev", "devtmpfs", MS_NOSUID);
    mount_early("devpts", "/dev/pts", "devpts", MS_NOSUID | MS_NOEXEC);
    mount_early("tmpfs", "/run", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC);

    printf("\\nSheen OS early userspace\\n");
    printf("PID 1: native sheen-init\\n");
    printf("kernel: ");
    { char buf[128]; FILE *f = fopen("/proc/version", "r"); if (f) { if (fgets(buf, sizeof(buf), f)) fputs(buf, stdout); fclose(f); } else puts("unknown"); }
    printf("\\n");

    while (!stopping) {
        reap_children();
        if (child_pid <= 0) {
            child_pid = start_shell();
            if (child_pid < 0) { struct timespec ts = {1, 0}; nanosleep(&ts, NULL); continue; }
        }
        int status;
        pid_t r = waitpid(child_pid, &status, 0);
        if (r == child_pid) child_pid = -1;
        else if (r < 0 && errno != EINTR) child_pid = -1;
    }

    if (child_pid > 0) { kill(child_pid, SIGTERM); waitpid(child_pid, NULL, 0); }
    reboot(RB_POWER_OFF);
    for (;;) pause();
}
