    #define FUSE_USE_VERSION 31

    #include <fuse3/fuse.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <time.h>
    #include <zlib.h>
    #include <ctype.h>

    #define STARTER_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/starter"
    #define STARTER_EXT ".mai"
    #define METRO_DIR   "/home/aslpet/Sisop/modul4/soal_4/chiho/metro"
    #define METRO_EXT ".ccc"
    #define DRAGON_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/dragon"
    #define DRAGON_EXT ".rot"
    #define BLACKROSE_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/blackrose"
    #define BLACKROSE_EXT ".bin"
    #define HEAVEN_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/heaven"
    #define HEAVEN_EXT ".enc"
    #define HEAVEN_KEY "0123456789abcdef0123456789abcdef"
    #define YOUTH_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/skystreet"
    #define YOUTH_EXT ".gz"

    static int sref_getattr(const char *relpath, struct stat *stbuf);
    static int sref_readdir(void *buf, fuse_fill_dir_t filler);
    static int sref_read(const char *relpath, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
    static int sref_write(const char *relpath, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
    static int sref_create(const char *relpath, mode_t mode, struct fuse_file_info *fi);
    static int sref_unlink(const char *relpath);

    // =============================
    // Helper
    // =============================

    static char* add_extension(const char *dir, const char *path, const char *ext) {
        char *res = malloc(strlen(dir) + strlen(path) + strlen(ext) + 3);
        sprintf(res, "%s/%s%s", dir, path, ext);
        return res;
    }

    void apply_rot13(char *buf, size_t len) {
        for (size_t i = 0; i < len; i++) {
            char c = buf[i];
            if ('a' <= c && c <= 'z') buf[i] = 'a' + (c - 'a' + 13) % 26;
            else if ('A' <= c && c <= 'Z') buf[i] = 'A' + (c - 'A' + 13) % 26;
        }
    }

    void generate_iv(unsigned char *iv) {
        FILE *fp = fopen("/dev/urandom", "r");
        fread(iv, 1, 16, fp);
        fclose(fp);
    }

    // =============================
    // Starter Chiho
    // =============================

    int starter_getattr(const char *relpath, struct stat *stbuf) {
        char *fpath = add_extension(STARTER_DIR, relpath, STARTER_EXT);
        int res = lstat(fpath, stbuf);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    int starter_readdir(void *buf, fuse_fill_dir_t filler) {
        DIR *dp;
        struct dirent *de;

        dp = opendir(STARTER_DIR);
        if (dp == NULL) return -errno;

        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        while ((de = readdir(dp)) != NULL) {
            if (strstr(de->d_name, STARTER_EXT)) {
                char fname[256];
                size_t len = strlen(de->d_name) - strlen(STARTER_EXT);
                strncpy(fname, de->d_name, len);
                fname[len] = '\0';
                filler(buf, fname, NULL, 0, 0);
            }
        }

        closedir(dp);
        return 0;
    }

    int starter_read(const char *relpath, char *buf, size_t size, off_t offset) {
        char *fpath = add_extension(STARTER_DIR, relpath, STARTER_EXT);
        int fd = open(fpath, O_RDONLY);
        free(fpath);
        if (fd == -1) return -errno;

        int res = pread(fd, buf, size, offset);
        close(fd);
        return res == -1 ? -errno : res;
    }

    int starter_create(const char *relpath, mode_t mode, struct fuse_file_info *fi) {
        char *fpath = add_extension(STARTER_DIR, relpath, STARTER_EXT);
        int fd = open(fpath, fi->flags | O_CREAT, mode);
        free(fpath);
        if (fd == -1) return -errno;
        close(fd);
        return 0;
    }

    int starter_write(const char *relpath, const char *buf, size_t size, off_t offset) {
        char *fpath = add_extension(STARTER_DIR, relpath, STARTER_EXT);
        int fd = open(fpath, O_WRONLY);
        free(fpath);
        if (fd == -1) return -errno;

        int res = pwrite(fd, buf, size, offset);
        close(fd);
        return res == -1 ? -errno : res;
    }

    int starter_unlink(const char *relpath) {
        char *fpath = add_extension(STARTER_DIR, relpath, STARTER_EXT);
        int res = unlink(fpath);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    // =============================
    // Metro Chiho
    // =============================

    int metro_getattr(const char *relpath, struct stat *stbuf) {
        char* fpath = add_extension(METRO_DIR, relpath, METRO_EXT);
        int res = lstat(fpath, stbuf);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    int metro_readdir(void *buf, fuse_fill_dir_t filler) {
        DIR *dp;
        struct dirent *de;

        dp = opendir(METRO_DIR);
        if (dp == NULL) return -errno;

        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        continue;

        size_t len = strlen(de->d_name);
        size_t ext_len = strlen(METRO_EXT);

        if (len > ext_len && strcmp(de->d_name + len - ext_len, METRO_EXT) == 0) {
        char fname[256];
        strncpy(fname, de->d_name, len - ext_len);
        fname[len - ext_len] = '\0';
        filler(buf, fname, NULL, 0, 0);
        }
    }
        closedir(dp);
        return 0;
    }

    int metro_read(const char *relpath, char *buf, size_t size, off_t offset) {
        char* fpath = add_extension(METRO_DIR, relpath, METRO_EXT);
        int fd = open(fpath, O_RDONLY);
        if (fd == -1) {
            free(fpath);
            return -errno;
        }

        char *temp = malloc(size);
        int res = pread(fd, temp, size, offset);
        close(fd);
        if (res == -1) {
            free(temp);
            free(fpath);
            return -errno;
        }

        for (int i = 0; i < res; i++) {
            buf[i] = temp[i] - (i % 256);
        }

        free(temp);
        free(fpath);
        return res;
    }

    int metro_write(const char *relpath, const char *buf, size_t size, off_t offset) {
        char* fpath = add_extension(METRO_DIR, relpath, METRO_EXT);
        int fd = open(fpath, O_WRONLY | O_CREAT, 0666);
        if (fd == -1) {
            free(fpath);
            return -errno;
        }

        char *enc = malloc(size);
        for (int i = 0; i < size; i++) {
            enc[i] = buf[i] + (i % 256);
        }

        int res = pwrite(fd, enc, size, offset);
        close(fd);
        free(enc);
        free(fpath);
        return res == -1 ? -errno : res;
    }

    int metro_create(const char *relpath, mode_t mode, struct fuse_file_info *fi) {
        char* fpath = add_extension(METRO_DIR, relpath, METRO_EXT);
        int fd = open(fpath, fi->flags | O_CREAT, mode);
        free(fpath);
        if (fd == -1) return -errno;
        close(fd);
        return 0;
    }

    int metro_unlink(const char *relpath) {
        char* fpath = add_extension(METRO_DIR, relpath, METRO_EXT);
        int res = unlink(fpath);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    // =============================
    // Dragon Chiho
    // =============================

    int dragon_getattr(const char *relpath, struct stat *stbuf) {
        char *fpath = add_extension(DRAGON_DIR, relpath, DRAGON_EXT);
        int res = lstat(fpath, stbuf);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    int dragon_readdir(void *buf, fuse_fill_dir_t filler) {
        DIR *dp = opendir(DRAGON_DIR);
        if (!dp) return -errno;

        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        struct dirent *de;
        while ((de = readdir(dp)) != NULL) {
            if (strstr(de->d_name, DRAGON_EXT)) {
                char fname[256];
                size_t len = strlen(de->d_name) - strlen(DRAGON_EXT);
                strncpy(fname, de->d_name, len);
                fname[len] = '\0';
                filler(buf, fname, NULL, 0, 0);
            }
        }

        closedir(dp);
        return 0;
    }

    int dragon_open(const char *relpath, struct fuse_file_info *fi) {
        char *fpath = add_extension(DRAGON_DIR, relpath, DRAGON_EXT);
        int fd = open(fpath, fi->flags);
        free(fpath);
        return fd == -1 ? -errno : 0;
    }

    int dragon_read(const char *relpath, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        (void) fi;
        char *fpath = add_extension(DRAGON_DIR, relpath, DRAGON_EXT);
        int fd = open(fpath, O_RDONLY);
        if (fd == -1) {
            free(fpath);
            return -errno;
        }

        int res = pread(fd, buf, size, offset);
        if (res >= 0) apply_rot13(buf, res);

        close(fd);
        free(fpath);
        return res == -1 ? -errno : res;
    }

    int dragon_write(const char *relpath, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        (void) fi;
        if (offset != 0) return -EOPNOTSUPP;

        char *fpath = add_extension(DRAGON_DIR, relpath, DRAGON_EXT);
        int fd = open(fpath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            free(fpath);
            return -errno;
        }

        char *enc = malloc(size);
        memcpy(enc, buf, size);
        apply_rot13(enc, size);

        int res = write(fd, enc, size);
        free(enc);
        close(fd);
        free(fpath);
        return res == -1 ? -errno : res;
    }

    int dragon_create(const char *relpath, mode_t mode, struct fuse_file_info *fi) {
        char *fpath = add_extension(DRAGON_DIR, relpath, DRAGON_EXT);
        int fd = open(fpath, fi->flags | O_CREAT, mode);
        free(fpath);
        if (fd == -1) return -errno;
        close(fd);
        return 0;
    }

    int dragon_unlink(const char *relpath) {
        char *fpath = add_extension(DRAGON_DIR, relpath, DRAGON_EXT);
        int res = unlink(fpath);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    // =============================
    // Blackrose Chiho
    // =============================

    int blackrose_getattr(const char *relpath, struct stat *stbuf) {
        char *fpath = add_extension(BLACKROSE_DIR, relpath, BLACKROSE_EXT);
        int res = lstat(fpath, stbuf);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    int blackrose_readdir(void *buf, fuse_fill_dir_t filler) {
        DIR *dp = opendir(BLACKROSE_DIR);
        if (!dp) return -errno;

        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        struct dirent *de;
        while ((de = readdir(dp)) != NULL) {
            if (strstr(de->d_name, BLACKROSE_EXT)) {
                char fname[256];
                size_t len = strlen(de->d_name) - strlen(BLACKROSE_EXT);
                strncpy(fname, de->d_name, len);
                fname[len] = '\0';
                filler(buf, fname, NULL, 0, 0);
            }
        }

        closedir(dp);
        return 0;
    }

    int blackrose_open(const char *relpath, struct fuse_file_info *fi) {
        char *fpath = add_extension(BLACKROSE_DIR, relpath, BLACKROSE_EXT);
        int fd = open(fpath, fi->flags);
        free(fpath);
        return fd == -1 ? -errno : 0;
    }

    int blackrose_read(const char *relpath, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        (void) fi;
        char *fpath = add_extension(BLACKROSE_DIR, relpath, BLACKROSE_EXT);
        int fd = open(fpath, O_RDONLY);
        if (fd == -1) {
            free(fpath);
            return -errno;
        }

        int res = pread(fd, buf, size, offset);
        close(fd);
        free(fpath);
        return res == -1 ? -errno : res;
    }

    int blackrose_write(const char *relpath, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        (void) fi;
        char *fpath = add_extension(BLACKROSE_DIR, relpath, BLACKROSE_EXT);
        int fd = open(fpath, O_WRONLY | O_CREAT, 0666);
        if (fd == -1) {
            free(fpath);
            return -errno;
        }

        int res = pwrite(fd, buf, size, offset);
        close(fd);
        free(fpath);
        return res == -1 ? -errno : res;
    }

    int blackrose_create(const char *relpath, mode_t mode, struct fuse_file_info *fi) {
        char *fpath = add_extension(BLACKROSE_DIR, relpath, BLACKROSE_EXT);
        int fd = open(fpath, fi->flags | O_CREAT, mode);
        free(fpath);
        if (fd == -1) return -errno;
        close(fd);
        return 0;
    }

    int blackrose_unlink(const char *relpath) {
        char *fpath = add_extension(BLACKROSE_DIR, relpath, BLACKROSE_EXT);
        int res = unlink(fpath);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    // =============================
    // Heaven Chiho
    // =============================

    int heaven_write(const char *relpath, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        (void) fi;
        if (offset != 0) return -EOPNOTSUPP;

        char *fpath = add_extension(HEAVEN_DIR, relpath, HEAVEN_EXT);

        // Generate IV
        unsigned char iv[16];
        generate_iv(iv);

        // Simpan IV di awal file output
        FILE *fout = fopen(fpath, "wb");
        if (!fout) {
            free(fpath);
            return -errno;
        }
        fwrite(iv, 1, 16, fout);

        // Tulis plaintext ke file sementara
        char tmpin[] = "/tmp/heaven_in_XXXXXX";
        int fdin = mkstemp(tmpin);
        if (fdin == -1) {
            fclose(fout);
            free(fpath);
            return -errno;
        }
        write(fdin, buf, size);
        close(fdin);

        // Hex IV
        char iv_hex[33] = {0};
        for (int i = 0; i < 16; i++) sprintf(iv_hex + i*2, "%02x", iv[i]);

        // Enkripsi dengan openssl
        char tmpout[] = "/tmp/heaven_out_XXXXXX";
        int fdout = mkstemp(tmpout);
        close(fdout);

        char cmd[512];
        snprintf(cmd, sizeof(cmd),
            "openssl enc -aes-256-cbc -K %s -iv %s -in %s -out %s",
            HEAVEN_KEY, iv_hex, tmpin, tmpout);
        int enc_res = system(cmd);
        if (enc_res != 0) {
            unlink(tmpin);
            unlink(tmpout);
            fclose(fout);
            free(fpath);
            return -EIO;
        }

        // Tambahkan ciphertext ke file akhir
        FILE *fin = fopen(tmpout, "rb");
        char buf_out[4096];
        size_t r;
        while ((r = fread(buf_out, 1, sizeof(buf_out), fin)) > 0) {
            fwrite(buf_out, 1, r, fout);
        }

        fclose(fin);
        fclose(fout);
        unlink(tmpin);
        unlink(tmpout);
        free(fpath);
        return size;
    }

    int heaven_read(const char *relpath, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        (void) fi;
        if (offset != 0) return -EOPNOTSUPP;

        char *fpath = add_extension(HEAVEN_DIR, relpath, HEAVEN_EXT);

        FILE *fin = fopen(fpath, "rb");
        if (!fin) {
            free(fpath);
            return -errno;
        }

        // Baca IV (16 byte pertama)
        unsigned char iv[16];
        fread(iv, 1, 16, fin);

        // Simpan ciphertext ke file sementara
        char tmpin[] = "/tmp/heaven_ct_XXXXXX";
        int fdin = mkstemp(tmpin);
        FILE *tmpf = fdopen(fdin, "wb");

        char tmpout[] = "/tmp/heaven_pt_XXXXXX";
        int fdout = mkstemp(tmpout);
        close(fdout);

        char cbuf[4096];
        size_t r;
        while ((r = fread(cbuf, 1, sizeof(cbuf), fin)) > 0) {
            fwrite(cbuf, 1, r, tmpf);
        }
        fclose(tmpf);
        fclose(fin);

        // Hex IV
        char iv_hex[33] = {0};
        for (int i = 0; i < 16; i++) sprintf(iv_hex + i*2, "%02x", iv[i]);

        // Dekripsi dengan openssl
        char cmd[512];
        snprintf(cmd, sizeof(cmd),
            "openssl enc -d -aes-256-cbc -K %s -iv %s -in %s -out %s",
            HEAVEN_KEY, iv_hex, tmpin, tmpout);
        int dec_res = system(cmd);
        if (dec_res != 0) {
            unlink(tmpin);
            unlink(tmpout);
            free(fpath);
            return -EIO;
        }

        // Baca plaintext hasil dekripsi
        FILE *fout = fopen(tmpout, "rb");
        int total = fread(buf, 1, size, fout);
        fclose(fout);

        unlink(tmpin);
        unlink(tmpout);
        free(fpath);
        return total;
    }

    int heaven_getattr(const char *relpath, struct stat *stbuf) {
        char *fpath = add_extension(HEAVEN_DIR, relpath, HEAVEN_EXT);
        int res = lstat(fpath, stbuf);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    int heaven_readdir(void *buf, fuse_fill_dir_t filler) {
        DIR *dp = opendir(HEAVEN_DIR);
        if (!dp) return -errno;

        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        struct dirent *de;
        while ((de = readdir(dp)) != NULL) {
            if (strstr(de->d_name, HEAVEN_EXT)) {
                char fname[256];
                size_t len = strlen(de->d_name) - strlen(HEAVEN_EXT);
                strncpy(fname, de->d_name, len);
                fname[len] = '\0';
                filler(buf, fname, NULL, 0, 0);
            }
        }

        closedir(dp);
        return 0;
    }

    int heaven_create(const char *relpath, mode_t mode, struct fuse_file_info *fi) {
        char *fpath = add_extension(HEAVEN_DIR, relpath, HEAVEN_EXT);
        int fd = open(fpath, fi->flags | O_CREAT, mode);
        free(fpath);
        if (fd == -1) return -errno;
        close(fd);
        return 0;
    }

    int heaven_unlink(const char *relpath) {
        char *fpath = add_extension(HEAVEN_DIR, relpath, HEAVEN_EXT);
        int res = unlink(fpath);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    // =============================
    // Skystreet Chiho
    // =============================

    int youth_write(const char *relpath, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        (void) fi;
        if (offset != 0) return -EOPNOTSUPP;

        char *fpath = add_extension(YOUTH_DIR, relpath, YOUTH_EXT);
        gzFile gz = gzopen(fpath, "wb");
        if (!gz) {
            free(fpath);
            return -errno;
        }

        int written = gzwrite(gz, buf, size);
        gzclose(gz);
        free(fpath);

        return written == 0 ? -EIO : written;
    }

    int youth_read(const char *relpath, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        (void) fi;
        if (offset != 0) return -EOPNOTSUPP;

        char *fpath = add_extension(YOUTH_DIR, relpath, YOUTH_EXT);
        gzFile gz = gzopen(fpath, "rb");
        if (!gz) {
            free(fpath);
            return -errno;
        }

        int read = gzread(gz, buf, size);
        gzclose(gz);
        free(fpath);

        return read == -1 ? -EIO : read;
    }


    int youth_getattr(const char *relpath, struct stat *stbuf) {
        char *fpath = add_extension(YOUTH_DIR, relpath, YOUTH_EXT);
        int res = lstat(fpath, stbuf);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    int youth_readdir(void *buf, fuse_fill_dir_t filler) {
        DIR *dp = opendir(YOUTH_DIR);
        if (!dp) return -errno;

        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        struct dirent *de;
        while ((de = readdir(dp)) != NULL) {
            if (strstr(de->d_name, YOUTH_EXT)) {
                char fname[256];
                size_t len = strlen(de->d_name) - strlen(YOUTH_EXT);
                strncpy(fname, de->d_name, len);
                fname[len] = '\0';
                filler(buf, fname, NULL, 0, 0);
            }
        }

        closedir(dp);
        return 0;
    }

    int youth_create(const char *relpath, mode_t mode, struct fuse_file_info *fi) {
        char *fpath = add_extension(YOUTH_DIR, relpath, YOUTH_EXT);
        int fd = open(fpath, fi->flags | O_CREAT, mode);
        free(fpath);
        if (fd == -1) return -errno;
        close(fd);
        return 0;
    }

    int youth_unlink(const char *relpath) {
        char *fpath = add_extension(YOUTH_DIR, relpath, YOUTH_EXT);
        int res = unlink(fpath);
        free(fpath);
        return res == -1 ? -errno : 0;
    }

    // =============================
    // FUSE Callbacks with Routing
    // =============================

    static int maimai_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
        (void) fi;
        const char *relpath;
        if (strncmp(path, "/starter", 8) == 0 && (path[8] == '/' || path[8] == '\0')) {
            if (path[8] == '\0') {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;
            }
            relpath = path + 9;
            return starter_getattr(relpath, stbuf);
        } else if (strncmp(path, "/metro", 6) == 0 && (path[6] == '/' || path[6] == '\0')) {
            if (path[6] == '\0') {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;
            }
            relpath = path + 7;
            return metro_getattr(relpath, stbuf);
        } else if (strncmp(path, "/dragon", 7) == 0 && (path[7] == '/' || path[7] == '\0')) {
            if (path[7] == '\0') {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;
            }
            relpath = path + 8;
            return dragon_getattr(relpath, stbuf);
        } else if (strncmp(path, "/blackrose", 10) == 0 && (path[10] == '/' || path[10] == '\0')) {
            if (path[10] == '\0') {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;
            }
            relpath = path + 11;
            return blackrose_getattr(relpath, stbuf);
        } else if (strncmp(path, "/heaven", 7) == 0 && (path[7] == '/' || path[7] == '\0')) {
            if (path[7] == '\0') {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;
            }
            relpath = path + 8;
            return heaven_getattr(relpath, stbuf);
        } else if (strncmp(path, "/skystreet", 10) == 0 && (path[10] == '/' || path[10] == '\0')) {
            if (path[10] == '\0') {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;
            }
            relpath = path + 11;
            return youth_getattr(relpath, stbuf);
        } else if (strncmp(path, "/7sref", 6) == 0 && (path[6] == '/' || path[6] == '\0')) {
            if (path[6] == '\0') {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;
            }
            relpath = path + 7;
            return sref_getattr(relpath, stbuf);
        } else if (strcmp(path, "/") == 0 ||
                strcmp(path, "/starter") == 0 ||
                strcmp(path, "/metro") == 0 ||
                strcmp(path, "/dragon") == 0 ||
                strcmp(path, "/blackrose") == 0 ||
                strcmp(path, "/heaven") == 0 ||
                strcmp(path, "/skystreet") == 0 ||
                strcmp(path, "/7sref") == 0) {

        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
        } else if (strcmp(path, "/starter/") == 0 || strcmp(path, "/metro/") == 0 ||
                strcmp(path, "/dragon/") == 0 || strcmp(path, "/blackrose/") == 0 ||
                strcmp(path, "/heaven/") == 0 || strcmp(path, "/skystreet/") == 0 ||
                strcmp(path, "/7sref/") == 0) {
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            return 0;
        }
        return -ENOENT;
    }

    static int maimai_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                            off_t offset, struct fuse_file_info *fi,
                            enum fuse_readdir_flags flags)
    {
        (void) offset;
        (void) fi;
        (void) flags;

        if (strcmp(path, "/") == 0) {
            filler(buf, ".", NULL, 0, 0);
            filler(buf, "..", NULL, 0, 0);
            filler(buf, "starter", NULL, 0, 0);
            filler(buf, "metro", NULL, 0, 0);
            filler(buf, "dragon", NULL, 0, 0);
            filler(buf, "blackrose", NULL, 0, 0);
            filler(buf, "heaven", NULL, 0, 0);
            filler(buf, "skystreet", NULL, 0, 0);
            filler(buf, "7sref", NULL, 0, 0);
            return 0;
        }
        else if (strcmp(path, "/starter") == 0 || strcmp(path, "/starter/") == 0)
            return starter_readdir(buf, filler);
        else if (strcmp(path, "/metro") == 0 || strcmp(path, "/metro/") == 0)
            return metro_readdir(buf, filler);
        else if (strcmp(path, "/dragon") == 0 || strcmp(path, "/dragon/") == 0)
            return dragon_readdir(buf, filler);
        else if (strcmp(path, "/blackrose") == 0 || strcmp(path, "/blackrose/") == 0)
            return blackrose_readdir(buf, filler);
        else if (strcmp(path, "/heaven") == 0 || strcmp(path, "/heaven/") == 0)
            return heaven_readdir(buf, filler);
        else if (strcmp(path, "/skystreet") == 0 || strcmp(path, "/skystreet/") == 0)
            return youth_readdir(buf, filler);
        else if (strcmp(path, "/7sref") == 0 || strcmp(path, "/7sref/") == 0)
            return sref_readdir(buf, filler);

        return -ENOENT;
    }

    static int maimai_open(const char *path, struct fuse_file_info *fi) {
        struct stat st;
        return maimai_getattr(path, &st, fi);
    }

    static int maimai_read(const char *path, char *buf, size_t size, off_t offset,
                        struct fuse_file_info *fi) {
        (void) fi;
        const char *relpath;
        if (strncmp(path, "/starter/", 8) == 0) {
            relpath = path + 9;
            return starter_read(relpath, buf, size, offset);
        } else if (strncmp(path, "/metro/", 7) == 0) {
            relpath = path + 8;
            return metro_read(relpath, buf, size, offset);
        } else if (strncmp(path, "/dragon/", 8) == 0) {
            relpath = path + 9;
            return dragon_read(relpath, buf, size, offset, fi);
        } else if (strncmp(path, "/blackrose/", 11) == 0) {
            relpath = path + 12;
            return blackrose_read(relpath, buf, size, offset, fi);
        } else if (strncmp(path, "/heaven/", 8) == 0) {
            relpath = path + 9;
            return heaven_read(relpath, buf, size, offset, fi);
        } else if (strncmp(path, "/skystreet/", 11) == 0) {
            relpath = path + 12;
            return youth_read(relpath, buf, size, offset, fi);
        } else if (strncmp(path, "/7sref/", 7) == 0) {
            relpath = path + 8;
            return sref_read(relpath, buf, size, offset, fi);
        }
        return -ENOENT;
    }

static int maimai_write(const char *path, const char *buf, size_t size, off_t offset,
                        struct fuse_file_info *fi) {
    const char *relpath;

    if (strncmp(path, "/starter/", 9) == 0) {
        relpath = path + 9;
        return starter_write(relpath, buf, size, offset);
    } else if (strncmp(path, "/metro/", 7) == 0) {
        relpath = path + 7;
        return metro_write(relpath, buf, size, offset);
    } else if (strncmp(path, "/dragon/", 8) == 0) {
        relpath = path + 8;
        return dragon_write(relpath, buf, size, offset, fi);
    } else if (strncmp(path, "/blackrose/", 11) == 0) {
        relpath = path + 11;
        return blackrose_write(relpath, buf, size, offset, fi);
    } else if (strncmp(path, "/heaven/", 8) == 0) {
        relpath = path + 8;
        return heaven_write(relpath, buf, size, offset, fi);
    } else if (strncmp(path, "/skystreet/", 11) == 0) {
        relpath = path + 11;
        return youth_write(relpath, buf, size, offset, fi);
    } else if (strncmp(path, "/7sref/", 7) == 0) {
        relpath = path + 7;
        return sref_write(relpath, buf, size, offset, fi);
    }
    return -ENOENT;
}

static int maimai_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    const char *relpath;

    if (strncmp(path, "/starter/", 9) == 0) {
        relpath = path + 9;
        return starter_create(relpath, mode, fi);
    } else if (strncmp(path, "/metro/", 7) == 0) {
        relpath = path + 7;
        return metro_create(relpath, mode, fi);
    } else if (strncmp(path, "/dragon/", 8) == 0) {
        relpath = path + 8;
        return dragon_create(relpath, mode, fi);
    } else if (strncmp(path, "/blackrose/", 11) == 0) {
        relpath = path + 11;
        return blackrose_create(relpath, mode, fi);
    } else if (strncmp(path, "/heaven/", 8) == 0) {
        relpath = path + 8;
        return heaven_create(relpath, mode, fi);
    } else if (strncmp(path, "/skystreet/", 11) == 0) {
        relpath = path + 11;
        return youth_create(relpath, mode, fi);
    } else if (strncmp(path, "/7sref/", 7) == 0) {
        relpath = path + 7;
        return sref_create(relpath, mode, fi);
    }
    return -ENOENT;
}

static int maimai_unlink(const char *path) {
    const char *relpath;

    if (strncmp(path, "/starter/", 9) == 0) {
        relpath = path + 9;
        return starter_unlink(relpath);
    } else if (strncmp(path, "/metro/", 7) == 0) {
        relpath = path + 7;
        return metro_unlink(relpath);
    } else if (strncmp(path, "/dragon/", 8) == 0) {
        relpath = path + 8;
        return dragon_unlink(relpath);
    } else if (strncmp(path, "/blackrose/", 11) == 0) {
        relpath = path + 11;
        return blackrose_unlink(relpath);
    } else if (strncmp(path, "/heaven/", 8) == 0) {
        relpath = path + 8;
        return heaven_unlink(relpath);
    } else if (strncmp(path, "/skystreet/", 11) == 0) {
        relpath = path + 11;
        return youth_unlink(relpath);
    } else if (strncmp(path, "/7sref/", 7) == 0) {
        relpath = path + 7;
        return sref_unlink(relpath);
    }
    return -ENOENT;
}

    static struct fuse_operations maimai_oper = {
        .getattr = maimai_getattr,
        .readdir = maimai_readdir,
        .open    = maimai_open,
        .read    = maimai_read,
        .write   = maimai_write,
        .create  = maimai_create,
        .unlink  = maimai_unlink,
    };

    // =============================
    // Prism Chiho
    // =============================

    int parse_7sref_path(const char *relpath, char *area_out, char *file_out) {
        const char *underscore = strchr(relpath, '_');
        if (!underscore) return -1;

        size_t area_len = underscore - relpath;
        strncpy(area_out, relpath, area_len);
        area_out[area_len] = '\0';

        strcpy(file_out, underscore + 1);
        return 0;
    }

    int sref_getattr(const char *relpath, struct stat *stbuf) {
        char area[64], file[256];
        if (parse_7sref_path(relpath, area, file) != 0)
            return -ENOENT;

        char redirected[512];
        snprintf(redirected, sizeof(redirected), "/%s/%s", area, file);
        return maimai_getattr(redirected, stbuf, NULL);
    }

    int sref_read(const char *relpath, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        char area[64], file[256];
        if (parse_7sref_path(relpath, area, file) != 0)
            return -ENOENT;

        char redirected[512];
        snprintf(redirected, sizeof(redirected), "/%s/%s", area, file);
        return maimai_read(redirected, buf, size, offset, fi);
    }

    int sref_write(const char *relpath, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        char area[64], file[256];
        if (parse_7sref_path(relpath, area, file) != 0)
            return -ENOENT;

        char redirected[512];
        snprintf(redirected, sizeof(redirected), "/%s/%s", area, file);
        return maimai_write(redirected, buf, size, offset, fi);
    }

    int sref_create(const char *relpath, mode_t mode, struct fuse_file_info *fi) {
        char area[64], file[256];
        if (parse_7sref_path(relpath, area, file) != 0)
            return -ENOENT;

        char redirected[512];
        snprintf(redirected, sizeof(redirected), "/%s/%s", area, file);
        return maimai_create(redirected, mode, fi);
    }

    int sref_unlink(const char *relpath) {
        char area[64], file[256];
        if (parse_7sref_path(relpath, area, file) != 0)
            return -ENOENT;

        char redirected[512];
        snprintf(redirected, sizeof(redirected), "/%s/%s", area, file);
        return maimai_unlink(redirected);
    }

    int sref_open(const char *relpath, struct fuse_file_info *fi) {
        char area[64], file[256];
        if (parse_7sref_path(relpath, area, file) != 0)
            return -ENOENT;

        char redirected[512];
        snprintf(redirected, sizeof(redirected), "/%s/%s", area, file);
        return maimai_open(redirected, fi);
    }

    int sref_readdir(void *buf, fuse_fill_dir_t filler) {
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    struct {
        const char *dir;
        const char *ext;
        const char *prefix;
    } areas[] = {
        {STARTER_DIR, STARTER_EXT, "starter"},
        {METRO_DIR, METRO_EXT, "metro"},
        {DRAGON_DIR, DRAGON_EXT, "dragon"},
        {BLACKROSE_DIR, BLACKROSE_EXT, "blackrose"},
        {HEAVEN_DIR, HEAVEN_EXT, "heaven"},
        {YOUTH_DIR, YOUTH_EXT, "skystreet"},
    };

    for (int i = 0; i < sizeof(areas)/sizeof(areas[0]); i++) {
        DIR *dp = opendir(areas[i].dir);
        if (!dp) continue;

        struct dirent *de;
        while ((de = readdir(dp)) != NULL) {
            const char *name = de->d_name;
            size_t name_len = strlen(name);
            size_t ext_len = strlen(areas[i].ext);

            if (ext_len > 0) {
                if (name_len <= ext_len || strcmp(name + name_len - ext_len, areas[i].ext) != 0)
                    continue;
            }

            char stripped[256];
            if (ext_len > 0) {
                strncpy(stripped, name, name_len - ext_len);
                stripped[name_len - ext_len] = '\0';
            } else {
                strncpy(stripped, name, sizeof(stripped));
                stripped[sizeof(stripped)-1] = '\0';
            }

            char combined[512];
            snprintf(combined, sizeof(combined), "%s_%s", areas[i].prefix, stripped);
            filler(buf, combined, NULL, 0, 0);
        }

        closedir(dp);
    }

    return 0;
}

    int main(int argc, char *argv[]) {
        return fuse_main(argc, argv, &maimai_oper, NULL);
    }
