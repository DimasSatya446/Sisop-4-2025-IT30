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
> The Shorekeeper adalah sebuah entitas misterius yang memimpin dan menjaga Black Shores secara keseluruhan. Karena Shorekeeper hanya berada di Black Shores, ia biasanya berjalan - jalan di sekitar Black Shores untuk mencari anomali - anomali yang ada untuk mencegah adanya kekacauan ataupun krisis di Black Shores. Semenjak kemunculan Fallacy of No Return, ia semakin ketat dalam melakukan pencarian anomali - anomali yang ada di Black Shores untuk mencegah hal yang sama terjadi lagi.   
Suatu hari, saat di Tethys' Deep, Shorekeeper menemukan sebuah anomali yang baru diketahui. Anomali ini berupa sebuah teks acak yang kelihatannya tidak memiliki arti. Namun, ia mempunyai ide untuk mencari arti dari teks acak tersebut.
[Author: Haidar / scar / hemorrhager / 恩赫勒夫]

a. Pertama, Shorekeeper akan mengambil beberapa sampel anomali teks dari link berikut. Pastikan file zip terhapus setelah proses unzip.
> on revision
```sh
const char *DOWNLOAD_URL = "https://drive.usercontent.google.com/u/0/uc?id=1hi_GDdP51Kn2JJMw02WmCOxuc3qrXzh5&export=download ";
...
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
```
- Fungsi ini menerima array pointer ke string `(cmd[])` yang berisi command dan argumennya
- `pid_t pid = fork()` - Membuat proses child baru
- Dalam `child process (pid == 0)`:
  - `execvp()` menjalankan command yang diberikan
  - Parameter pertama: nama program `(cmd[0])`
  - Parameter kedua: array argumen program
  - `perror()` akan dieksekusi jika execvp gagal
  - `exit(EXIT_FAILURE)` mengakhiri child process jika gagal
- Dalam parent process:
  - `waitpid()` menunggu child process selesai
  - status menyimpan status exit dari child process
```sh
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

int main(int argc, char* argv[]) {
    ...
    download_then_unzip();         // Panggil fungsinya untuk download > unzip
    ...

    return fuse_main(argc, argv, &hexed_oper, NULL);
}
```
- Pengecekan Direktori:
  - `struct stat st` - Struktur untuk menyimpan informasi file
  - `stat(ANOMALI_DIR, &st)` - Mengecek status direktori anomali
  - `S_ISDIR(st.st_mode)` - Memastikan path adalah direktori
  - Jika direktori sudah ada, fungsi return tanpa melakukan apa-apa
- Download File:
  - Membuat array command wget:
  - `"wget"` - Program downloader
  - `DOWNLOAD_URL` - URL file yang akan didownload
  - `"-O"` - Option untuk output file
  - `ZIP_FILE` - Nama file output ("anomali.zip")
  - NULL - Penanda akhir array
- Ekstrak File:
  - Membuat array command unzip:
  - `"unzip"` - Program ekstraksi
  - `ZIP_FILE` - File yang akan diekstrak
  - `"-d"` - Option untuk direktori tujuan
  - `"."` - Ekstrak ke direktori saat ini
  - `NULL` - Penanda akhir array
- Cleanup:
  - `unlink(ZIP_FILE)` - Menghapus file zip setelah selesai diekstrak
  - Menampilkan pesan selesai

b. Setelah melihat teks - teks yang didapatkan, ia menyadari bahwa format teks tersebut adalah hexadecimal. Dengan informasi tersebut, Shorekeeper mencoba untuk mencoba idenya untuk mencari makna dari teks - teks acak tersebut, yaitu dengan mengubahnya dari string hexadecimal menjadi sebuah file image. Bantulah Shorekeeper dengan membuat kode untuk mengubah string hexadecimal menjadi sebuah gambar ketika file text tersebut dibuka. Lalu, letakkan hasil gambar yang didapat ke dalam directory bernama “image”.
```sh
#define ANOMALI_DIR "anomali"
#define IMAGE_DIR "anomali/image"
```
- Konstanta path direktori:
  - ANOMALI_DIR: Direktori tempat file .txt disimpan.
  - IMAGE_DIR: Direktori hasil konversi gambar.

```
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
```
- Mengubah string hex ke data biner.
- Memastikan panjang genap dan alokasi memori cukup.
- Setiap 2 karakter hex dikonversi ke 1 byte biner.

```
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
```
- Fungsi ini digunakan untuk mendapatkan atribut file/direktori (seperti ukuran, jenis file, permission, dsb).
- `(void)fi;` → Menyembunyikan warning karena parameter tidak digunakan.
- `memset(stbuf, 0, sizeof(struct stat));` → Reset buffer stat agar kosong sebelum diisi.
- Jika path adalah /, maka:
  - Mode: Direktori `(S_IFDIR)` dengan permission `rwxr-xr-x.`
  - Link count: 2 (karena direktori selalu memiliki link ke dirinya sendiri dan .).
- Jika path berakhiran `.txt`, asumsikan itu adalah file regular.
- Mode: Regular file `(S_IFREG)` dengan permission read-only (0444).
- Ukuran file dihitung sebagai setengah panjang file teks, karena tiap 2 karakter hex = 1 byte biner.
- Jika path cocok dengan file/folder nyata di `ANOMALI_DIR`, salin atribut aslinya.
- Jika tidak ditemukan, kembalikan error `-ENOENT`.

```sh
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
```
- Fungsi untuk membaca isi direktori.
- Parameter tidak digunakan, disembunyikan dengan `(void)`.
- `    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);` Setiap direktori harus memiliki entry untuk . (dir sendiri) dan .. (parent).
- Jika path adalah `/`, tampilkan file `1.txt` sampai `7.txt` dan folder `image`.
- Buka direktori fisik `anomali/image/` dan tambahkan semua isinya ke buffer.

```sh
static int hexed_open(const char* path, struct fuse_file_info* fi) {
    if (!strstr(path, ".txt")) return -EINVAL;
    return 0;
}
```
- Memastikan hanya file dengan ekstensi `.txt` yang bisa dibuka.
- Jika bukan `.txt`, kembalikan error -EINVAL (invalid argument).
```sh
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
```
- Hanya izinkan pembacaan file `.txt`.
- `    char* filename = strrchr(path, '/') ? strrchr(path, '/') + 1 : (char*)path + 1;` Ekstrak nama file dari path (misal: /1.txt → 1.txt).
- Buka file asli di `anomali/`.
- Baca isi file ke buffer `hex_buffer`.
- Jika file kosong, kembalikan 0. Gunakan fungsi `hex_to_binary()` untuk konversi.
- `snprintf(image_path, sizeof(image_path), "%s/%s_image_%s.png", IMAGE_DIR, filename, timestamp);` Generate nama file gambar unik menggunakan timestamp.
- Simpan file PNG.
- Tulis log konversi.
- Salin data biner ke buffer FUSE agar bisa dibaca oleh user.
- Bebaskan memori alokasi `binary` dan `timestamp`.
```sh
// --- FUSE Ops ---
static struct fuse_operations hexed_oper = {
    .getattr = hexed_getattr,
    .readdir = hexed_readdir,
    .open = hexed_open,
    .read = hexed_read,
};
```
- Daftar fungsi operasi FUSE yang akan digunakan oleh filesystem virtual.
c. Untuk penamaan file hasil konversi dari string ke image adalah [nama file string]_image_[YYYY-mm-dd]_[HH:MM:SS].
Contoh:
1_image_2025-05-11_18:35:26.png
```sh
//in hexed_read
// Simpan gambar di folder image
    char* timestamp = get_timestamp();
    char image_path[256];
    snprintf(image_path, sizeof(image_path), "%s/%s_image_%s.png", IMAGE_DIR, filename, timestamp);
```
d. Catat setiap konversi yang ada ke dalam sebuah log file bernama conversion.log. Untuk formatnya adalah sebagai berikut.
[YYYY-mm-dd][HH:MM:SS]: Successfully converted hexadecimal text [nama file string] to [nama file image].
```sh
char* get_timestamp() {
    time_t rawtime;
    struct tm* timeinfo;
    char* buffer = malloc(30);
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 30, "%Y-%m-%d_%H:%M:%S", timeinfo);
    return buffer;
}
```
- `time_t`: Tipe data standar C untuk menyimpan waktu (biasanya dalam detik sejak epoch).
- `rawtime`: Variabel untuk menyimpan waktu saat ini.
- `struct tm`: Struktur C yang menyimpan informasi waktu dalam bentuk terurai (tahun, bulan, hari, jam, menit, detik, dst).
- `char* buffer = malloc(30);` Alokasi memori untuk menyimpan string timestamp.
- timeinfo = localtime(&rawtime);
- `strftime(buffer, 30, "%Y-%m-%d_%H:%M:%S", timeinfo);` Fungsi `strftime()` mengonversi waktu dari `timeinfo ke` string sesuai format yang ditentukan.

```sh
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
```
- `char* timestamp = get_timestamp();` Mendapatkan timestamp menggunakan fungsi `get_timestamp()`.
- `FILE* log = fopen(LOG_FILE, "a");` Membuka file log dengan mode append `("a")`
- Jika pembukaan file gagal (misalnya karena izin atau direktori tidak ada):
  - Tampilkan pesan error via `perror`.
  - Bebaskan memori `timestamp`.
  - Keluar dari fungsi.
- `fprintf(log, "[%s]: Successfully converted hexadecimal text %s to %s\n",
        timestamp, input_file, output_file);` Menulis entri log ke file dengan format yang diinginkan.

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
  
---

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
### Revisi soal_1
- Pakai full path, untuk path ke directorynya
  ```sh
  #define ANOMALI_DIR "/home/riverz/PraktikumSisop/Sisop-4-2025-IT30/soal_1/anomali"
  #define IMAGE_DIR "/home/riverz/PraktikumSisop/Sisop-4-2025-IT30/soal_1/anomali/image"
  #define LOG_FILE "/home/riverz/PraktikumSisop/Sisop-4-2025-IT30/soal_1/anomali/conversion.log"
  #define ZIP_FILE "/home/riverz/PraktikumSisop/Sisop-4-2025-IT30/soal_1/anomali.zip"
  ```
- Perubahan logic fungsi untuk mengubah file `.txt` ke `image`
```sh
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
```
- Membuka File Input `FILE *input = fopen(filename_txt, "r");
if (!input) return;`
  - Membuka file teks untuk dibaca
  - Return jika gagal membuka file
- ```
  const char *basename = strrchr(filename_txt, '/');
  basename = (basename) ? basename + 1 : filename_txt;
  char name_only[64] = {0};
  strncpy(name_only, basename, strcspn(basename, "."));
  ```
  - Mengambil nama file dari path lengkap
  - Menghapus ekstensi file
- Membuat Timestamp
  ```
  char date[16], time[16];
  get_timestamp(date, sizeof(date), time, sizeof(time));
  ```
  - Mengambil tanggal dan waktu saat ini untuk nama file
- Membuat Direktori Output
  ```
  struct stat st = {0};
  if (stat(IMAGE_DIR, &st) == -1) {
    #ifdef _WIN32
        _mkdir(IMAGE_DIR);
    #else
        mkdir(IMAGE_DIR, 0755);
    #endif
  }
  ```
  - Mengecek dan membuat direktori image jika belum ada
  - Menggunakan kondisional kompilasi untuk Windows/Unix
- Membuat dan Membuka File Output
  ```
  char output_path[256];
  snprintf(output_path, sizeof(output_path), "%s/%s_image_%s_%s.png", IMAGE_DIR,
  name_only, date, time);
  FILE *output = fopen(output_path, "wb");
  ```
  - Membuat nama file output dengan format: `[nama]_image_[tanggal]_[waktu].png`
  - Membuka file dalam mode binary write
- Proses Konversi
  ```
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
    ```
  - Membaca file input karakter per karakter
  - Mengumpulkan 2 karakter hex
  - Mengkonversi hex ke byte
  - Menulis byte ke file output
- Cleanup dan Logging
  ```
  fclose(input);
  fclose(output);
  log_conversion(basename, output_path, date, time);
    ```
```sh
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
```
- Iterasi file
  ```
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strstr(entry->d_name, ".txt")) {
        char full_path[256];
        snprintf(full_path, sizeof(full_path), "%s/%s", ANOMALI_DIR, entry->d_name);
        convert_hex_to_image(full_path);
    }
  }
    ```
- Membaca setiap entry dalam direktori
- Mencari file dengan ekstensi `.txt`
- Membuat path lengkap untuk setiap file
- Memanggil `convert_hex_to_image()` untuk setiap file `.txt`
- Update `hexed_readdir()` untuk mengisi directory image di mount
  ```sh
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

    return -ENOENT;}
  ```
### Output after revisi
![Screenshot 2025-05-23 212302](https://github.com/user-attachments/assets/ae685431-12fd-4417-9ba7-23cb5e5876a1)
![Screenshot 2025-05-23 212314](https://github.com/user-attachments/assets/03cbc126-f72f-4421-9fb9-d8613fd1f423)
![image](https://github.com/user-attachments/assets/31fa833b-43da-475b-9cbb-156971cffef1)

### Revisi soal_3
- sebelum:
Tidak dapat melihat isi dari it24_host tanpa mount
- Setelah:
![alt text](https://github.com/DimasSatya446/GambarSISOP2/blob/main/Screenshot%202025-05-21%20085037.png)
Dapat melihat isi dari it24_host tanpa mount. Sebelumnya tidak bisa karna salah input
