#define _XOPEN_SOURCE 700
#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define RELICS_DIR "relics"
#define LOG_FILE "activity.log"
#define FRAGMENT_SIZE 1024
#define MAX_FRAGMENTS 14

size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int download_file(const char *url, const char *output_file) {
    CURL *curl = curl_easy_init();
    if (!curl) return 0;

    FILE *fp = fopen(output_file, "wb");
    if (!fp) {
        curl_easy_cleanup(curl);
        return 0;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);
    fclose(fp);
    
    return (res == CURLE_OK);
}

void split_baymax_file(const char *input_file) {
    FILE *input = fopen(input_file, "rb");
    if (!input) return;

    char buffer[FRAGMENT_SIZE];
    int fragment_count = 0;

    mkdir(RELICS_DIR, 0755);

    while (!feof(input)) {
        size_t bytes_read = fread(buffer, 1, FRAGMENT_SIZE, input);
        if (bytes_read == 0) break;

        char fragment_path[512];
        snprintf(fragment_path, sizeof(fragment_path), "%s/Baymax.jpeg.%03d", 
                RELICS_DIR, fragment_count);

        FILE *output = fopen(fragment_path, "wb");
        if (!output) break;

        fwrite(buffer, 1, bytes_read, output);
        fclose(output);
        fragment_count++;
    }

    fclose(input);
    unlink(input_file);
}

void initialize_relics() {
    struct stat st;
    
    if (stat(RELICS_DIR, &st) == -1) {
        if (mkdir(RELICS_DIR, 0755) == -1) {
            fprintf(stderr, "Failed to create relics directory: %s\n", strerror(errno));
            return;
        }
        
        const char *url = "https://drive.google.com/uc?export=download&id=1MHVhFT57Wa9Zcx69Bv9j5ImHc9rXMH1c";
        const char *temp_file = "Baymax.jpeg.temp";

        printf("Downloading Baymax file...\n");
        if (download_file(url, temp_file)) {
            printf("Splitting into fragments...\n");
            split_baymax_file(temp_file);
            printf("Successfully initialized Baymax fragments in %s directory\n", RELICS_DIR);
        } else {
            fprintf(stderr, "Failed to download Baymax file: %s\n", strerror(errno));
        }
    }
}

// Logging function
void log_activity(const char *action, const char *details) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[26];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    FILE *log = fopen(LOG_FILE, "a");
    if (!log) return;

    // Special handling for COPY operations
    if (strstr(details, " -> ") && strcmp(action, "WRITE") != 0) {
        fprintf(log, "[%s] COPY: %s\n", timestamp, details);
    } else {
        fprintf(log, "[%s] %s: %s\n", timestamp, action, details);
    }
    fclose(log);
}

static int get_file_size(const char *filename) {
    char frag_path[512];
    int total_size = 0;

    for (int i = 0; i < MAX_FRAGMENTS; i++) {
        snprintf(frag_path, sizeof(frag_path), "%s/%s.%03d", RELICS_DIR, filename, i);
        struct stat st;
        if (stat(frag_path, &st) == -1) break;
        total_size += st.st_size;
    }
    return total_size;
}

static int vfs_getattr(const char *path, struct stat *stbuf,
                      struct fuse_file_info *fi) {
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    const char *filename = path + 1;
    char check_path[512];
    snprintf(check_path, sizeof(check_path), "%s/%s.000", RELICS_DIR, filename);

    struct stat temp_stat;
    if (stat(check_path, &temp_stat) == 0) {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size = get_file_size(filename);
        return 0;
    }

    return -ENOENT;
}

// Update readdir signature to match FUSE version 31
static int vfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi,
                      enum fuse_readdir_flags flags) {
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    // Update filler calls to match FUSE 3.x format
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    DIR *dir = opendir(RELICS_DIR);
    if (!dir) return -errno;

    struct dirent *entry;
    char current_file[256] = "";

    while ((entry = readdir(dir))) {
        char *dot = strrchr(entry->d_name, '.');
        if (dot && strcmp(dot+1, "000") == 0) {
            *dot = '\0';  // Remove extension temporarily
            if (strcmp(current_file, entry->d_name) != 0) {
                filler(buf, entry->d_name, NULL, 0, 0);
                strcpy(current_file, entry->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}

static int vfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char base_path[256]; // base path for file checking
    snprintf(base_path, sizeof(base_path), "%s/%s", RELICS_DIR, path + 1); // make the base path

    size_t bytes_read = 0; // bytes read
    int fragment_index = offset / FRAGMENT_SIZE; // fragment index
    off_t fragment_offset = offset % FRAGMENT_SIZE; // offset in fragment

    while (bytes_read < size)  {
        char fragment_path[300];
        snprintf(fragment_path, sizeof(fragment_path), "%s.%03d", base_path, fragment_index); // make fragment path
        
        FILE *fragment = fopen(fragment_path, "rb");
        if (!fragment) break; // fragment doesn't exist

        fseek(fragment, fragment_offset, SEEK_SET); 
        size_t read_size = fread(buf + bytes_read, 1, size - bytes_read, fragment);
        bytes_read += read_size; // update bytes read
        fragment_offset = 0; // reset offset fragment next
        fragment_index++; 
        fclose(fragment);
    }
    return bytes_read; 
}

static int vfs_write(const char *path, const char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
    const char *filename = path + 1;
    size_t bytes_written = 0;
    int fragment_count = 0;
    char fragments_list[2048] = "";
    char fragment_name[128];
    char log_msg[2048];

    while (bytes_written < size) {
        char frag_path[512];
        snprintf(frag_path, sizeof(frag_path), "%s/%s.%03d", RELICS_DIR, filename, fragment_count);

        FILE *f = fopen(frag_path, "wb");
        if (!f) return -errno;

        size_t to_write = size - bytes_written;
        if (to_write > FRAGMENT_SIZE) to_write = FRAGMENT_SIZE;

        size_t written = fwrite(buf + bytes_written, 1, to_write, f);
        fclose(f);

        if (written == 0) return -EIO;

        // Add fragment to list with size check
        snprintf(fragment_name, sizeof(fragment_name), "%s.%03d", filename, fragment_count);
        if (strlen(fragments_list) + strlen(fragment_name) + 2 < sizeof(fragments_list)) {
            if (fragment_count > 0) strcat(fragments_list, ", ");
            strcat(fragments_list, fragment_name);
        }

        bytes_written += written;
        fragment_count++;
    }

    if (snprintf(log_msg, sizeof(log_msg), "%s -> %s", filename, fragments_list) >= sizeof(log_msg)) {
        // Handling
        size_t msg_len = strlen(log_msg);
        if (msg_len > 3) {
            log_msg[msg_len-3] = '.';
            log_msg[msg_len-2] = '.';
            log_msg[msg_len-1] = '.';
        }
    }
    log_activity("WRITE", log_msg);

    return size;
}

static int vfs_unlink(const char *path) {
    const char *filename = path + 1;
    char full_path[512];
    int fragments_deleted = 0;
    
    DIR *dir = opendir(RELICS_DIR);
    if (!dir) return -errno;

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strncmp(entry->d_name, filename, strlen(filename)) == 0 &&
            entry->d_name[strlen(filename)] == '.') {
            
            snprintf(full_path, sizeof(full_path), "%s/%s", RELICS_DIR, entry->d_name);
            if (unlink(full_path) == 0) {
                fragments_deleted++;
            }
        }
    }
    closedir(dir);

    if (fragments_deleted > 0) {
        char log_msg[512];
        snprintf(log_msg, sizeof(log_msg), "%s (fragments: %d)", filename, fragments_deleted);
        log_activity("DELETE", log_msg);
        return 0;
    }

    return -ENOENT;
}

static int vfs_open(const char *path, struct fuse_file_info *fi) {
    char first_fragment[256]; // first fragment name
    snprintf(first_fragment, sizeof(first_fragment), "%s/%s.000", RELICS_DIR, path + 1); // make the first fragment name
    
    if (access(first_fragment, F_OK) == -1) return -ENOENT;

    log_activity("READ", path + 1);
    return 0; 
}

static struct fuse_operations vfs_oper = {
    .getattr    = vfs_getattr,
    .readdir    = vfs_readdir,
    .read       = vfs_read,
    .write      = vfs_write,
    .unlink     = vfs_unlink,
    .open       = vfs_open,
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <mountpoint>\n", argv[0]);
        return 1;
    }

    // inisiasi curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // inisiasi relics(download)
    initialize_relics();
    
    char *new_argv[argc + 3];  
    int new_argc = 0;
    
    new_argv[new_argc++] = argv[0];
    
    // Add FUSE options before mount point
    new_argv[new_argc++] = "-o";
    new_argv[new_argc++] = "default_permissions,allow_other";
    
    for (int i = 1; i < argc; i++) {
        new_argv[new_argc++] = argv[i];
    }
    
    umask(0);
    int ret = fuse_main(new_argc, new_argv, &vfs_oper, NULL);
    
    curl_global_cleanup();
    return ret;
}