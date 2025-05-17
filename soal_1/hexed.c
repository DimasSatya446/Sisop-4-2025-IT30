#define FUSE_USE_VERSION 30

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

// Path direktori utama
#define ANOMALI_DIR "anomali"
#define IMAGE_DIR "anomali/image"
#define LOG_FILE "anomali/conversion.log"

// Buat timestamp untuk nama file dan log
char* get_timestamp() {
    time_t rawtime;
    struct tm* timeinfo;
    char* buffer = malloc(30);
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 30, "%Y-%m-%d_%H:%M:%S", timeinfo);
    return buffer;
}

// Simpan log konversi
void log_conversion(const char* input_file, const char* output_file) {
    char* timestamp = get_timestamp();
    FILE* log = fopen(LOG_FILE, "a");
    if (!log) {
        perror("fopen log");
        free(timestamp);
        return;
    }

    fprintf(log, "[%s]: Successfully converted hexadecimal text %s to %s\n",
            timestamp, input_file, output_file);

    fclose(log);
    free(timestamp);
}

// Konversi hex string ke binary
unsigned char* hex_to_binary(const char* hex, size_t* out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) {
        fprintf(stderr, "Invalid hex string length\n");
        return NULL;
    }

    *out_len = len / 2;
    unsigned char* binary = malloc(*out_len);
    if (!binary) {
        perror("malloc");
        return NULL;
    }

    for (size_t i = 0; i < len; i += 2) {
        char byte_str[3] = {hex[i], hex[i + 1], '\0'};
        int val = strtol(byte_str, NULL, 16);
        binary[i / 2] = (unsigned char)val;
    }

    return binary;
}

// Simpan data biner sebagai file .png
void save_binary_to_png(const unsigned char* data, size_t bin_len, const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen png");
        return;
    }

    fwrite(data, 1, bin_len, fp);
    fclose(fp);
}

// Buat direktori jika belum ada
void create_dir_if_not_exists(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        #ifdef _WIN32
            _mkdir(path);
        #else
            mkdir(path, 0777);
        #endif
    }
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

    // Cek apakah adalah file txt
    if (strstr(path, ".txt")) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 0;

        // Baca ukuran asli file txt untuk lebih akurat
        char full_path[256];
        snprintf(full_path, sizeof(full_path), "%s%s.txt", ANOMALI_DIR, path + 1);
        FILE* f = fopen(full_path, "r");
        if (f) {
            fseek(f, 0, SEEK_END);
            stbuf->st_size = ftell(f) / 2; // Hex -> Binary
            fclose(f);
        }

        return 0;
    }

    // Untuk direktori image
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s", ANOMALI_DIR, path);
    if (access(full_path, F_OK) == 0) {
        struct stat s;
        if (stat(full_path, &s) == -1) return -errno;
        *stbuf = s;
        return 0;
    }

    return -ENOENT;
}

static int hexed_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
    (void)offset; (void)fi; (void)flags;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    if (strcmp(path, "/") == 0) {
        for (int i = 1; i <= 7; i++) {
            char name[256];
            snprintf(name, sizeof(name), "%d.txt", i);
            filler(buf, name, NULL, 0);
        }
        filler(buf, "image", NULL, 0);
        return 0;
    }

    // Untuk direktori image
    if (strcmp(path, "/image") == 0) {
        DIR* dir = opendir(IMAGE_DIR);
        if (!dir) return -errno;

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            filler(buf, entry->d_name, NULL, 0);
        }
        closedir(dir);
        return 0;
    }

    return -ENOENT;
}

static int hexed_open(const char* path, struct fuse_file_info* fi) {
    if (!strstr(path, ".txt")) return -EINVAL;
    return 0;
}

static int hexed_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    if (!strstr(path, ".txt")) return -EINVAL;

    char* filename = strrchr(path, '/') ? strrchr(path, '/') + 1 : (char*)path + 1;

    char full_txt_path[256];
    snprintf(full_txt_path, sizeof(full_txt_path), "%s/%s", ANOMALI_DIR, filename);

    FILE* txt_file = fopen(full_txt_path, "r");
    if (!txt_file) return -errno;

    char hex_buffer[10240];
    size_t hex_len = fread(hex_buffer, 1, sizeof(hex_buffer), txt_file);
    fclose(txt_file);

    if (hex_len == 0) return 0;

    size_t bin_len;
    unsigned char* binary = hex_to_binary(hex_buffer, &bin_len);
    if (!binary) return -EINVAL;

    // Simpan gambar di folder image
    char* timestamp = get_timestamp();
    char image_path[256];
    snprintf(image_path, sizeof(image_path), "%s/%s_image_%s.png", IMAGE_DIR, filename, timestamp);

    create_dir_if_not_exists(IMAGE_DIR);
    save_binary_to_png(binary, bin_len, image_path);
    log_conversion(filename, image_path);

    // Salin binary ke buffer fuse (bisa juga gunakan hex_buffer tergantung tujuan)
    size_t len = (offset + size > bin_len) ? bin_len - offset : size;
    memcpy(buf, binary + offset, len);

    free(binary);
    free(timestamp);

    return len;
}

// --- FUSE Ops ---
static struct fuse_operations hexed_oper = {
    .getattr = hexed_getattr,
    .readdir = hexed_readdir,
    .open = hexed_open,
    .read = hexed_read,
};

int main(int argc, char* argv[]) {
    umask(0);
    return fuse_main(argc, argv, &hexed_oper, NULL);
}