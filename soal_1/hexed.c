#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>

// Path direktori utama
#define ANOMALI_DIR "/home/riverz/PraktikumSisop/Sisop-4-2025-IT30/soal_1/anomali"
#define IMAGE_DIR "/home/riverz/PraktikumSisop/Sisop-4-2025-IT30/soal_1/anomali/image"
#define LOG_FILE "/home/riverz/PraktikumSisop/Sisop-4-2025-IT30/soal_1/anomali/conversion.log"
#define ZIP_FILE "/home/riverz/PraktikumSisop/Sisop-4-2025-IT30/soal_1/anomali.zip"

const char *DOWNLOAD_URL = "https://drive.usercontent.google.com/u/0/uc?id=1hi_GDdP51Kn2JJMw02WmCOxuc3qrXzh5&export=download ";

void run_command(const char *cmd[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(cmd[0], (char *const *)cmd);  // Casting diperlukan karena execvp butuh char *const[]
        perror("execvp gagal");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

void download_then_unzip() {
    struct stat st;

    if (stat(ANOMALI_DIR, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("Direktori %s sudah ada, skip download.\n", ANOMALI_DIR);
        return;
    }

    printf("Downloading ZIP...\n");

    const char *wget_cmd[] = {"wget", DOWNLOAD_URL, "-O", ZIP_FILE, NULL};
    run_command(wget_cmd);

    const char *unzip_cmd[] = {"unzip", ZIP_FILE, "-d", ".", NULL};
    run_command(unzip_cmd);

    unlink(ZIP_FILE);

    printf("Download dan ekstraksi selesai.\n");
}

// Buat timestamp untuk nama file dan log
void get_timestamp(char *date_str, size_t size1, char *time_str, size_t size2) {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date_str, size1, "%Y-%m-%d", timeinfo);
    strftime(time_str, size2, "%H:%M:%S", timeinfo);
}

// Simpan log konversi
void log_conversion(const char* input_file, const char* output_file, const char* date, const char* time) {
    FILE* log = fopen(LOG_FILE, "a");
    if (!log) return;

    fprintf(log, "[%s][%s]: Successfully converted hexadecimal text %s to %s\n",
            date, time, input_file, output_file);

    fclose(log);
}

// Konversi satu file .txt ke .png
void convert_hex_to_image(const char *filename_txt) {
    FILE *input = fopen(filename_txt, "r");
    if (!input) return;

    const char *basename = strrchr(filename_txt, '/');
    basename = (basename) ? basename + 1 : filename_txt;

    char name_only[64] = {0};
    strncpy(name_only, basename, strcspn(basename, "."));

    char date[16], time[16];
    get_timestamp(date, sizeof(date), time, sizeof(time));

    // Buat direktori image
    struct stat st = {0};
    if (stat(IMAGE_DIR, &st) == -1) {
        #ifdef _WIN32
            _mkdir(IMAGE_DIR);
        #else
            mkdir(IMAGE_DIR, 0755);
        #endif
    }

    char output_path[256];
    snprintf(output_path, sizeof(output_path), "%s/%s_image_%s_%s.png", IMAGE_DIR, name_only, date, time);
    FILE *output = fopen(output_path, "wb");
    if (!output) {
        fclose(input);
        return;
    }

    char hex[3];
    int c, count = 0;
    while ((c = fgetc(input)) != EOF) {
        if (isxdigit(c)) {
            hex[count++] = c;
            if (count == 2) {
                hex[2] = '\0';
                unsigned char byte = strtol(hex, NULL, 16);
                fwrite(&byte, 1, 1, output);
                count = 0;
            }
        }
    }

    fclose(input);
    fclose(output);

    log_conversion(basename, output_path, date, time);
}

// Proses semua file txt di anomali/
void process_all_files() {
    DIR *dir = opendir(ANOMALI_DIR);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".txt")) {
            char full_path[256];
            snprintf(full_path, sizeof(full_path), "%s/%s", ANOMALI_DIR, entry->d_name);
            convert_hex_to_image(full_path);
        }
    }
    closedir(dir);
}

// --- FUSE Operations ---
static int hexed_getattr(const char* path, struct stat* stbuf, struct fuse_file_info *fi) {
    (void)fi;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s", ANOMALI_DIR, path);

    // Cek apakah adalah file txt
    if (strstr(path, ".txt")) {
        char img_path[256] = {0};
        const char *filename = strrchr(path, '/') ? strrchr(path, '/') + 1 : path;
        char name_only[64] = {0};
        strncpy(name_only, filename, strcspn(filename, "."));
        DIR *img_dir = opendir(IMAGE_DIR);
        if (img_dir) {
            struct dirent *de;
            while ((de = readdir(img_dir)) != NULL) {
                if (strstr(de->d_name, name_only) && strstr(de->d_name, ".png")) {
                    snprintf(img_path, sizeof(img_path), "%s/%s", IMAGE_DIR, de->d_name);
                    break;
                }
            }
            closedir(img_dir);
        }

        if (strlen(img_path) > 0) {
            return lstat(img_path, stbuf);
        } else {
            return lstat(full_path, stbuf);
        }
    }

    if (access(full_path, F_OK) == 0) {
        return lstat(full_path, stbuf);
    }

    return -ENOENT;
}

static int hexed_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi,
                         enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    if (strcmp(path, "/") == 0) {
        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        DIR *dir = opendir(ANOMALI_DIR);
        if (!dir)
            return -errno;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            // Abaikan direktori 'image' agar tidak muncul di root
            if (strcmp(entry->d_name, "image") == 0)
                continue;

            filler(buf, entry->d_name, NULL, 0, 0);
        }
        closedir(dir);

        filler(buf, "image", NULL, 0, 0); // Tambahkan image sebagai virtual dir
        return 0;
    }

    if (strcmp(path, "/image") == 0) {
        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        DIR *img_dir = opendir(IMAGE_DIR);
        if (!img_dir)
            return -errno;

        struct dirent *entry;
        while ((entry = readdir(img_dir)) != NULL) {
            filler(buf, entry->d_name, NULL, 0, 0);
        }
        closedir(img_dir);
        return 0;
    }

    return -ENOENT;
}

static int hexed_open(const char* path, struct fuse_file_info* fi) {
    if (!strstr(path, ".txt")) return -EINVAL;

    char img_path[256] = {0};
    const char *filename = strrchr(path, '/') ? strrchr(path, '/') + 1 : path;
    char name_only[64] = {0};
    strncpy(name_only, filename, strcspn(filename, "."));
    DIR *img_dir = opendir(IMAGE_DIR);
    if (img_dir) {
        struct dirent *de;
        while ((de = readdir(img_dir)) != NULL) {
            if (strstr(de->d_name, name_only) && strstr(de->d_name, ".png")) {
                snprintf(img_path, sizeof(img_path), "%s/%s", IMAGE_DIR, de->d_name);
                break;
            }
        }
        closedir(img_dir);
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s", ANOMALI_DIR, path);

    const char *target_path = strlen(img_path) > 0 ? img_path : full_path;

    int res = open(target_path, fi->flags);
    if (res == -1)
        return -errno;
    close(res);
    return 0;
}

static int hexed_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    (void)fi;

    char img_path[256] = {0};
    const char *filename = strrchr(path, '/') ? strrchr(path, '/') + 1 : path;
    char name_only[64] = {0};
    strncpy(name_only, filename, strcspn(filename, "."));
    DIR *img_dir = opendir(IMAGE_DIR);
    if (img_dir) {
        struct dirent *de;
        while ((de = readdir(img_dir)) != NULL) {
            if (strstr(de->d_name, name_only) && strstr(de->d_name, ".png")) {
                snprintf(img_path, sizeof(img_path), "%s/%s", IMAGE_DIR, de->d_name);
                break;
            }
        }
        closedir(img_dir);
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s", ANOMALI_DIR, path);

    const char *target_path = strlen(img_path) > 0 ? img_path : full_path;

    int fd = open(target_path, O_RDONLY);
    if (fd == -1)
        return -errno;

    int res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

// --- FUSE Ops ---
static struct fuse_operations hexed_oper = {
    .getattr = hexed_getattr,
    .readdir = hexed_readdir,
    .open = hexed_open,
    .read = hexed_read,
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <mountpoint>\n", argv[0]);
        return 1;
    }

    umask(0);

    // Langkah-langkah otomatis
    download_then_unzip();         // Step 1: Download ZIP
    process_all_files();          // Step 2: Convert all .txt to .png

    return fuse_main(argc, argv, &hexed_oper, NULL);
}
