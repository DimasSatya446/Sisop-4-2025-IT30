# Sisop-4-2025-IT30
### by IT30


### Anggota Kelompok

| No | Nama                              | NRP         |
|----|-----------------------------------|------------|
| 1  | Adiwidya Budi Pratama            | 5027241012 |
| 2  | Ahmad Rafi Fadhillah Dwiputra     | 5027241068 |
| 3  | Dimas Satya Andhika              | 5027241032 |

---

## soal_1
a

---

## soal_2
a

---

## soal_3
# ANTINK🚫

## Dockerfile
```c
FROM ubuntu:20.04
```
- Menggunakan image dasar Ubuntu versi 20.04.

- Ini akan menjadi sistem operasi dalam container.

```c
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    fuse libfuse2 libfuse-dev gcc pkg-config

```

- `apt-get update`
→ Memperbarui daftar paket Ubuntu.

- `DEBIAN_FRONTEND=noninteractive`
→ Mencegah dialog interaktif saat instalasi (supaya tidak error saat build).

- `apt-get install -y`
→ Install otomatis tanpa tanya-tanya.

- `fuse, libfuse2, libfuse-dev`
→ Library dan header FUSE.

- `gcc, pkg-config`
→ Compiler C dan alat bantu compile dengan dependency library.

```c
COPY antink.c /root/antink.c
```
- Menyalin file `antink.c` dari host ke direktori `/root` dalam container.

```c
RUN gcc -Wall -D_FILE_OFFSET_BITS=64 `pkg-config fuse --cflags` /root/antink.c -o /root/antink `pkg-config fuse --libs`
```
- `gcc`: compiler C

- `1Wall` → aktifkan semua peringatan compiler

- `D_FILE_OFFSET_BITS=64` → agar program kompatibel dengan file besar (64-bit offset)

- `pkg-config fuse --cflags` → ambil flag compiler dari libfuse

- `/root/antink.c` → source file

- `-o /root/antink` → output binary

- `pkg-config fuse --libs` → ambil flag linker dari libfuse
```c
ENTRYPOINT ["/root/antink"]
```
- Menentukan perintah utama saat container dijalankan.
- `ENTRYPOIN`T ini akan selalu dijalankan dengan argumen dari `docker-compose`.

---

## docker-compose.yml
```c
version: "3.8"
```
- Menentukan versi schema Compose file.
- Versi 3.8 mendukung fitur seperti cap_add, security_opt, dan volumes.

### Service: `antink-server`
```c
services:
  antink-server:
```
- Menentukan service utama yang akan menjalankan antink (filesystem FUSE).

```c
    build: .
```
- Build image dari Dockerfile yang berada di direktori saat ini (`.`).

```c
    container_name: antink-server
```
- Nama container akan menjadi `antink-server`.

```c
    devices:
      - "/dev/fuse:/dev/fuse"
```
- Share device `/dev/fuse` dari host ke container agar FUSE bisa digunakan.

```c
    cap_add:
      - SYS_ADMIN
```
Menambahkan capability `SYS_ADMIN`, wajib untuk mounting filesystem FUSE.

``` c
   security_opt:
      - apparmor:unconfined
```
- Menonaktifkan profil keamanan AppArmor agar container tidak dibatasi saat mengakses kernel features seperti FUSE.

```c
    volumes:
      - ./it24_host:/mnt/it24_host         
      - ./antink_mount:/mnt/antink_mount   
      - ./antink-logs:/mnt/antink-logs:rw  
      - it24log:/var/log     
```              
- `./it24_host:/mnt/it24_host`
→ Direktori di host tempat file sumber berada, dipasang ke dalam container.

- `./antink_mount:/mnt/antink_mount`
→ Direktori tempat mount FUSE akan terjadi, jadi isi FUSE akan terlihat di sini.

- `./antink-logs:/mnt/antink-logs:rw`
→ Direktori log warning. Mode read-write.

- `it24log:/var/log`
→ Volume bernama `it24log`, digunakan untuk menyimpan log aktivitas (akses .log).

```c
    command: ["-f", "/mnt/antink_mount"]
```
- Argumen untuk `ENTRYPOINT` di Dockerfile.
- Artinya container akan menjalankan:
```c
/root/antink -f /mnt/antink_mount
```
- `-f` → run FUSE dalam foreground
- `/mnt/antink_mount` → mount point-nya

### Service: antink-logger
```c
  antink-logger:
    image: busybox
    container_name: antink-logger
```
- Service tambahan untuk membaca log.
- Menggunakan image ringan `busybox`.

```c
    volumes:
      - it24log:/var/log:ro
```
- Mount volume `it24log` sebagai read-only.
- Ini adalah volume yang sama dengan yang dipakai `antink-server`.

```c
    command: ["sh", "-c", "tail -f /var/log/it24.log"]
```
- Perintah yang dijalankan di container logger:
    - Jalankan shell (`sh`)
    - Lalu perintah `tail -f` untuk menampilkan log secara real-time dari file `/var/log/it24.log`.

### Bagian volumes
```c
volumes:
  it24log:
```
- Mendefinisikan volume bernama `it24log` secara eksplisit agar bisa dipakai oleh dua container (`antink-server` & `antink-logger`).

## antink.c
### Variabel Global
```c
static const char *source_dir = "/mnt/it24_host";
static const char *log_path = "/mnt/antink-logs";
static const char *activity_log = "/var/log/it24.log";
```
- `static const char *source_dir = "/mnt/it24_host";`
→ Menunjuk ke direktori asli tempat file disimpan. Semua operasi virtual akan mengarah ke sini.
- `static const char *log_path = "/mnt/antink-logs";`
→ Lokasi file untuk menyimpan log peringatan (file berbahaya).
- `static const char *activity_log = "/var/log/it24.log";`
→ Lokasi file untuk menyimpan log semua aktivitas pengguna (read, readdir, dll).


### Fungsi apply_rot13
```c
void apply_rot13(char *buf, int len) {
    for (int i = 0; i < len; i++) {
        char c = buf[i];
        if ('a' <= c && c <= 'z') buf[i] = 'a' + (c - 'a' + 13) % 26;
        else if ('A' <= c && c <= 'Z') buf[i] = 'A' + (c - 'A' + 13) % 26;
    }
}
```
- Fungsi ini melakukan ROT13, yaitu enkripsi dengan cara menggeser 13 karakter dalam alfabet.
- `for (int i = 0; i < len; i++)`
→ Loop untuk semua karakter dalam buffer `buf`.
- `char c = buf[i];`
→ Ambil satu karakter dari buffer.
- `if ('a' <= c && c <= 'z')`
→ Jika huruf kecil, geser 13 karakter ke depan.
- `buf[i] = 'a' + (c - 'a' + 13) % 26;`
→ Hitung posisi baru dari karakter.
- `else if ('A' <= c && c <= 'Z')`
→ Jika huruf besar, lakukan hal serupa di rentang A-Z.

### Fungsi reverse_name
```c
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
```

1. Pisahkan nama & ekstensi:
```c
const char *dot = strrchr(name, '.');
```
- Cari titik terakhir (`.`) untuk pisahkan nama dan ekstensi.

2. Simpan nama dan ekstensi ke buffer:
- `name_part` → bagian sebelum titik
- `ext_part` → bagian setelah titik

3. Balik ekstensi & simpan ke awal reversed:
```c
for (i = 0; i < ext_len; ++i)
    reversed[i] = ext_part[ext_len - i - 1];
```

4. Tambahkan titik di tengah:
```c
reversed[ext_len] = '.';
```

5. Balik nama & lanjutkan di belakang titik:
```c
for (i = 0; i < name_len; ++i)
    reversed[ext_len + 1 + i] = name_part[name_len - i - 1];
```

6. Tambahkan null terminator:
```c
reversed[ext_len + 1 + name_len] = '\0';
```

### Fungsi is_dangerous
```c
int is_dangerous(const char *name) {
    return strstr(name, "nafis") || strstr(name, "kimcun");
}
```
- `strstr(name, "nafis")` → mengecek apakah substring `"nafis"` muncul dalam `name`.
- `|| strstr(name, "kimcun")` → atau substring `"kimcun"`.
- Jika salah satu ditemukan, fungsi mengembalikan pointer non-NULL → hasilnya `true`.
- Jadi: fungsi ini return true jika nama file berisi “nafis” atau “kimcun”, yang dianggap "berbahaya".

### Fungsi log_warning
```c
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
```
- `fopen(log_path, "a")` → buka file log untuk ditambahkan (`append`).
- `time(NULL)` → ambil waktu sekarang (detik sejak 1970).
- `localtime()` → konversi ke format waktu lokal (jam, tanggal, dst).
- `fprintf(...)` → tulis ke log: waktu sekarang + nama file yang terdeteksi.
- `fclose(log)` → tutup file.

### Fungsi log_access
```c
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
```
- Mirip dengan `log_warning`, tapi mencatat aktivitas biasa (read, readdir, dll).
- Argumen `action` menentukan jenis aktivitas.
- Contoh isi log: `[LOG] 2025-05-19 14:01:00 READDIR: dokumen.txt`

### FUSE Handler antink_getattr
```c
static int antink_getattr(const char *path, struct stat *stbuf) {
    char fullpath[1024];
    sprintf(fullpath, "%s%s", source_dir, path);
    int res = lstat(fullpath, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}
```
- `path` → path virtual dari FUSE.
- `sprintf(fullpath, "%s%s", source_dir, path)`
→ gabungkan direktori sumber dengan path relatif.
- `lstat()` → ambil metadata file asli (bukan symlink).
- `if (res == -1)` → kalau gagal, return error code negatif.
- `return 0` → sukses.

### FUSE Handler antink_readdir
```c
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
```
- `fpath`: path asli direktori.
- `opendir(fpath)`: buka direktori asli.
- `readdir(dp)` → baca setiap entri (file/folder).
- `struct stat st` → inisialisasi stat kosong.
- `st.d_ino, st.d_mode` → set metadata dasar (inode dan tipe file).

Penanganan nama file:
- Jika `.` atau `..` → tetap tampilkan.
- Jika `is_dangerous()` → balik nama file dan log warning.
- Jika tidak → tampilkan nama asli.
- `filler(buf, displayed, &st, 0)` → tambahkan ke daftar isi direktori.
- `log_access("READDIR", de->d_name)` → log akses direktori.

### FUSE Handler antink_read
```c
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
```
- Bangun `fpath`: gabungan `source_dir` + path.
- `open(fpath, O_RDONLY)` → buka file untuk dibaca.
- `pread(...)` → baca isi file ke buffer `buf`, mulai dari `offset`, sepanjang `size` byte.

Jika file `.txt` dan aman:
- `apply_rot13(buf, res)` → enkripsi ROT13 pada isi file sebelum ditampilkan ke user.
- `log_access("READ", path)` → log aktivitas baca.
- `close(fd)` → tutup file.
- `return res` → kembalikan jumlah byte yang berhasil dibaca.

### Struktur fuse_operations
```c
static struct fuse_operations antink_oper = {
    .getattr = antink_getattr,
    .readdir = antink_readdir,
    .read = antink_read,
};
```
Mendaftarkan fungsi-fungsi handler kita ke sistem FUSE:
 - `getattr` → untuk metadata file
 - `readdir` → untuk membaca isi direktori
 - `read` → untuk membaca file

### Fungsi main
```c
int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &antink_oper, NULL);
}
```
- `umask(0)` → supaya permission file tidak dibatasi saat dibuat.
- `fuse_main(...)` → jalankan FUSE dengan handler `antink_oper` dan argumen command-line.


---

## soal_4
a

---

## Revisi
