#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

static const char *source_dir = "/mnt/it24_host";
static const char *log_path = "/mnt/antink-logs";
static const char *activity_log = "/var/log/it24.log";

// ROT13 untuk file .txt non-berbahaya
void apply_rot13(char *buf, int len) {
    for (int i = 0; i < len; i++) {
        char c = buf[i];
        if ('a' <= c && c <= 'z') buf[i] = 'a' + (c - 'a' + 13) % 26;
        else if ('A' <= c && c <= 'Z') buf[i] = 'A' + (c - 'A' + 13) % 26;
    }
}

void reverse_name(const char *name, char *reversed) {
    char name_part[512], ext_part[128];
    const char *dot = strrchr(name, '.');

    if (dot) {
        strcpy(ext_part, dot + 1);
        int len = dot - name;
        strncpy(name_part, name, len);
        name_part[len] = '\0';
    } else {
        strcpy(name_part, name);
        ext_part[0] = '\0';
    }

    int ext_len = strlen(ext_part);
    for (int i = 0; i < ext_len; ++i)
        reversed[i] = ext_part[ext_len - i - 1];

    reversed[ext_len] = '.';

    int name_len = strlen(name_part);
    for (int i = 0; i < name_len; ++i)
        reversed[ext_len + 1 + i] = name_part[name_len - i - 1];

    reversed[ext_len + 1 + name_len] = '\0';
}

int is_dangerous(const char *name) {
    return strstr(name, "nafis") || strstr(name, "kimcun");
}

void log_warning(const char *filename) {
    FILE *log = fopen(log_path, "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        fprintf(log, "[WARNING] %04d-%02d-%02d %02d:%02d:%02d: %s terdeteksi\n",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, filename);
        fclose(log);
    }
}

void log_access(const char *action, const char *filename) {
    FILE *log = fopen(activity_log, "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        fprintf(log, "[LOG] %04d-%02d-%02d %02d:%02d:%02d %s: %s\n",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, action, filename);
        fclose(log);
    }
}

static int antink_getattr(const char *path, struct stat *stbuf) {
    char fullpath[1024];
    sprintf(fullpath, "%s%s", source_dir, path);
    int res = lstat(fullpath, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int antink_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info *fi) {
    char fpath[1024];
    sprintf(fpath, "%s%s", source_dir, path);

    DIR *dp = opendir(fpath);
    if (dp == NULL) return -errno;

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        struct stat st = {0};
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        char displayed[512];
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            strcpy(displayed, de->d_name);
        } else if (is_dangerous(de->d_name)) {
            reverse_name(de->d_name, displayed);
            log_warning(de->d_name);
        } else {
            strcpy(displayed, de->d_name);
        }

        log_access("READDIR", de->d_name);
        if (filler(buf, displayed, &st, 0)) break;
    }

    closedir(dp);
    return 0;
}

static int antink_read(const char *path, char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi) {
    char fpath[1024];
    sprintf(fpath, "%s%s", source_dir, path);

    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;

    int res = pread(fd, buf, size, offset);
    if (res == -1) {
        close(fd);
        return -errno;
    }

    if (strstr(path, ".txt") && !is_dangerous(path)) {
        apply_rot13(buf, res);
    }

    log_access("READ", path);
    close(fd);
    return res;
}

static struct fuse_operations antink_oper = {
    .getattr = antink_getattr,
    .readdir = antink_readdir,
    .read = antink_read,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &antink_oper, NULL);
}
