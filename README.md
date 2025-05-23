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
### Run Command
```sh
gcc -D_FILE_OFFSET_BITS=64 hexed.c -o hexed `pkg-config --cflags --libs fuse3`
mkdir -p mnt
./hexed mnt

```
## soal_2
> Pada suatu hari, seorang ilmuwan muda menemukan sebuah drive tua yang tertanam di reruntuhan laboratorium robotik. Saat diperiksa, drive tersebut berisi pecahan data dari satu-satunya robot perawat legendaris yang dikenal dengan nama Baymax. Sayangnya, akibat kerusakan sistem selama bertahun-tahun, file utuh Baymax telah terfragmentasi menjadi 14 bagian kecil, masing-masing berukuran 1 kilobyte, dan tersimpan dalam direktori bernama relics. Pecahan tersebut diberi nama berurutan seperti Baymax.jpeg.000, Baymax.jpeg.001, hingga Baymax.jpeg.013. Ilmuwan tersebut kini ingin membangkitkan kembali Baymax ke dalam bentuk digital yang utuh, namun ia tidak ingin merusak file asli yang telah rapuh tersebut. (Author : Icel / wonbunsa)

> **soal_2 HAVEN'T SOLVED, THE CODE IN REPO IS STILL IN MAINTENANCE.**

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
### Maimai Chiho's Areas 💫🌌🌀⛩️
### Problem:

> **Seperti yang kamu tahu, kamu telah mendapatkan pekerjaan di SEGA sebagai chart designer untuk game maimai. [...], dan kamu pun dipromosikan menjadi administrator untuk sistem *in-game* mereka. Di universe maimai, terdapat suatu mekanisme progres yang bernama chiho, yang merupakan bagian dari suatu [area](https://maimai.sega.jp/area/). Terdapat 7 area di maimai saat ini, dan tugasmu sekarang yaitu memastikan 7 area ini berfungsi sebagaimana mestinya.**

> **NOTE: Implementasikan operasi dasar seperti read, write, create, unlink, getattr, readdir untuk setiap sub-soal.**

> **Implementing FUSE mechanism.**

### Struktur Soal:
```
├── chiho/  --> Output fuse_dir/ dari setiap area maimai
│ 	├── starter/  
│  	├── metro/  
│  	├── dragon/ 
│ 	├── blackrose/  
│  	├── heaven/  
│  	└── youth/  
│  
└── fuse_dir/  --> Input dari seluruh area maimai untuk chiho
	├── starter/  --> ekstensi .mai
	├── metro/  --> enkripsi ccc dan ekstensi .ccc
	├── dragon/  --> enkripsi ROT13 dan ekstensi .rot
	├── blackrose/  --> menyimpan data dalam bentuk biner dan ekstensi .bin
	├── heaven/  --> ekstensi AES-256-CBC dan ekstensi .enc
	├── youth/  --> kompresi file gzip dan ekstensi .gz
	└── 7sref/  --> area khusus akses "gateaway" ke area chiho lain 
```
---
### Code's Key Components:
```
#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
[...]
#include <time.h>
#include <zlib.h>
#include <ctype.h>
```
Menjadi *library* dan struktur dasar kebutuhan kode secara keseluruhan 	untuk pengoperasian pemrosesan file (`open`, `close`, `read`, `stat`, `zlib`, dll) serta konfigurasi FUSE yang akan digunakan pada proses.

```
#define STARTER_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/starter"
#define STARTER_EXT ".mai"
#define METRO_DIR   "/home/aslpet/Sisop/modul4/soal_4/chiho/metro"
#define METRO_EXT ".ccc"
#define DRAGON_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/dragon"
#define DRAGON_EXT ".rot"
#define BLACKROSE_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/blackrose"
[...]

#define HEAVEN_KEY "0123456789abcdef0123456789abcdef"
#define YOUTH_DIR "/home/aslpet/Sisop/modul4/soal_4/chiho/skystreet"
#define YOUTH_EXT ".gz"
```
Berfungsi dalam mendefinisikan *directory path* yang akan digunakan pada setiap `/fuse_dir/` dengan `/chiho/` untuk mencocokkan file terhadap area tertentu serta ekstensi *.file* yang ada pada setiap area chiho yang akan memproses file secara khusus, dengan mekanisme utama sebagai berikut:

-   `*_DIR` adalah path direktori asli di sistem file nyata.
    
-   `*_EXT` adalah ekstensi file di setiap area.
    
```
static int sref_getattr(const char *relpath, struct stat *stbuf);
static int sref_readdir(void *buf, fuse_fill_dir_t filler);
static int sref_read(const char *relpath, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
[...]

static int sref_create(const char *relpath, mode_t mode, struct fuse_file_info *fi);
static int sref_unlink(const char *relpath);
```
Berfungsi sebagai handler operasi file untuk direktori virtual / *alias redirector* dari direktori chiho lain ke  `/7sref` (“Prism Chiho”) dalam kode FUSE `maimai_fs.c` dan mempermudah perantara lintas-area pada `/7sref`.

```
static char* add_extension(const char *dir, const char *path, const char *ext) {
    char *res = malloc(strlen(dir) + strlen(path) + strlen(ext) + 3);
    sprintf(res, "%s/%s%s", dir, path, ext);
    return res;
}

void apply_rot13(char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = buf[i];
        [...]
        
    fread(iv, 1, 16, fp);
    fclose(fp);
}
```
Berfungsi sebagai *helper function* yang akan digunakan dalam pemrosesan file chiho dalam sistem, dengan mekanisme utama sebagai berikut:

-   `add_extension`: bantu gabungkan path virtual dengan direktori & ekstensi asli.
    
-   `apply_rot13`: enkripsi/dekripsi ROT13.
	- Huruf alfabet diubah: `a ↔ n`, `b ↔ o`, ... `z ↔ m`
    
	- Non-alfabet diabaikan.
    
-   `generate_iv`: buat inisialisasi vektor (IV) 16-byte dari `/dev/urandom` untuk enkripsi AES-256-CBC.
---
Secara keseluruhan, pada setiap area chiho memiliki fungsi-fungsi umum yang identik dengan satu sama lain dan diimplementasikan di setiap area, yaitu:
|Fungsi FUSE dalam Chiho's System|Tujuan|
|---|---|
|`*_getattr()`|Mendapatkan metadata file (`stat`)|
|`*_readdir()`|Menampilkan isi direktori chiho area (list file)|
|`*_read()`|Membaca isi file dari storage dir. chiho area tertentu ke user|
|`*_write()`|Menulis data dari user ke storage nyata pada dir. chiho area tertentu|
|`*_create()`|Membuat file baru di direktori area|
|`*_unlink()`|Menghapus file dari direktori area|

Lalu, setiap ***chiho's areas*** memiliki fungsi dan implementasi pemrosesan file yang berbeda-beda, yaitu sebagai berikut.
```
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

[...]

int starter_unlink(const char *relpath) {
    char *fpath = add_extension(STARTER_DIR, relpath, STARTER_EXT);
    int res = unlink(fpath);
    free(fpath);
    return res == -1 ? -errno : 0;
}
```
Berfungsi sebagai dasar pemrosesan file pada sistem FUSE `maimai_fs` terutama pada area `starter` dengan menyediakan antarmuka virtual pada `/starter/namafile` dalam sistem chiho, yang sudah mencakup pada kesuluruhan fungsi-fungsi umum FUSE area chiho serta menambahkan ekstensi `.mai` dan menyimpan data file biasa tanpa transformasi ataupun encoding/enkripsi. Secara sederhana, mekanisme utamanya bekerja sebagai berikut:

**1. `starter_getattr(const char *relpath, struct stat *stbuf)`**

- Fungsi ini mencocokkan nama file user (misal `lagu`) menjadi `lagu.mai`
-   Lalu dicek pakai `lstat()` untuk mendapatkan info file nyata
    
**2. `starter_readdir(void *buf, fuse_fill_dir_t filler)`**

-   Saat user ketik `ls /starter`, fungsi ini dipanggil.
-   Dibuka folder asli `/chiho/starter/`
-   Semua file `.mai` diambil, lalu ekstensi `.mai` disembunyikan agar user hanya lihat nama aslinya
    
**3. `starter_read(...)`**

-   Saat user membaca file (`cat /starter/lagu`), fungsi ini dijalankan.
-   File `lagu.mai` dibuka dan dibaca seperti biasa
    
** 4. `starter_write(...)`**

-   Saat user menulis ke file (`echo hello > /starter/catatan`), fungsi ini dipakai.
-   File `catatan.mai` dibuka, lalu isi file disimpan ke dalamnya.
-   Tidak dienkripsi, encoding, atau transformasi lainnya — langsung ditulis seperti biasa

#### 5. `starter_create(...)`

-   Saat user membuat file baru, fungsi ini dijalankan.
-   File dibuat di folder `/chiho/starter/`, nama file ditentukan user dan ekstensi `.mai` ditambahkan otomatis.

#### 6. `starter_unlink(...)`
-   Hapus file `.mai` dari sistem file nyata menggunakan `unlink()`.

```
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
        [...]

int metro_unlink(const char *relpath) {
    char* fpath = add_extension(METRO_DIR, relpath, METRO_EXT);
    int res = unlink(fpath);
    free(fpath);
    return res == -1 ? -errno : 0;
}
```
Berfungsi sebagai dasar pemrosesan file pada sistem FUSE `maimai_fs` terutama pada area `metro` dengan menyediakan akses virtual melalui `/metro/<namafile>`  yang sudah mencakup pada kesuluruhan fungsi-fungsi umum FUSE area chiho dengan ekstensi `.ccc` dan menerapkan enkripsi encoding sederhana per-byte pada proses file dalam direktori `metro area`. Secara sederhana, mekanisme utamanya bekerja sebagai berikut: 
**1. `metro_getattr(const char *relpath, struct stat *stbuf)`**

-   Gabungkan path menjadi `METRO_DIR/namafile.ccc`.
-   Ambil metadata file dengan `lstat()`.

**2. `metro_readdir(void *buf, fuse_fill_dir_t filler)`**

-   Membuka direktori `metro`.
-   Filter file dengan ekstensi `.ccc`.
-   Ekstensi `.ccc` dihilangkan dari output `ls`.

**3. `metro_read(const char *relpath, char *buf, size_t size, off_t offset)`**

-   Buka file `.ccc` → baca data mentah ke buffer sementara.
-   Tiap byte di-_decode_ dengan:
	 -	`buf[i] = temp[i] - (i % 256);` 

**4. `metro_write(const char *relpath, const char *buf, size_t size, off_t offset)`**

-   Buat buffer baru `enc`.
-   Tiap byte di-_encode_:
	- `enc[i] = buf[i] + (i % 256);` 
-   Tulis hasil encoding ke file `.ccc`.
    
**5. `metro_create(...)`**

-   Buat file `.ccc` baru di direktori `metro`.

**6. `metro_unlink(...)`**

-   Hapus file `.ccc` dari `metro`.

```
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

[...]
    
int dragon_unlink(const char *relpath) {
    char *fpath = add_extension(DRAGON_DIR, relpath, DRAGON_EXT);
    int res = unlink(fpath);
    free(fpath);
    return res == -1 ? -errno : 0;
}
```
Berfungsi sebagai dasar pemrosesan file pada sistem FUSE `maimai_fs` terutama pada area `dragon` dengan memberikan antarmuka virtual `/dragon/<namafile>`  yang sudah mencakup pada kesuluruhan fungsi-fungsi umum FUSE area chiho dengan ekstensi `.rot` dan menerapkan enkripsi ROT13 pada proses file dalam direktori `dragon area`. Secara sederhana, mekanisme utamanya bekerja sebagai berikut:

**1. `dragon_getattr(const char *relpath, struct stat *stbuf)`**

-   Gabungkan path → `dragon/namafile.rot`.
-   Ambil metadata file dengan `lstat()`.

**2. `dragon_readdir(void *buf, fuse_fill_dir_t filler)`**

-   Buka direktori `dragon`.
-   Ambil hanya file `.rot`.
-   Strip `.rot` sebelum ditampilkan ke user.

**3. `dragon_read(...)`**

-   Buka dan baca file `.rot` seperti biasa.
-   Setelah membaca, isi buffer di-_decrypt_ dengan `apply_rot13(buf, size)`:   
	 -   ROT13 menggeser huruf 13 langkah dalam alfabet (a↔n, b↔o, ..., z↔m).
    -   Non-alfabet tidak berubah.

**4. `dragon_write(...)`**

-   Data dari user disalin ke buffer, lalu `apply_rot13()` → disimpan ke file.

**5. `dragon_create(...)`**

-   Buat file `.rot` baru di direktori `dragon`.

**6. `dragon_unlink(...)`**

-   Hapus file `.rot` dari sistem file nyata.

```
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

[...]

int blackrose_unlink(const char *relpath) {
    char *fpath = add_extension(BLACKROSE_DIR, relpath, BLACKROSE_EXT);
    int res = unlink(fpath);
    free(fpath);
    return res == -1 ? -errno : 0;
}
```
Berfungsi sebagai dasar pemrosesan file pada sistem FUSE `maimai_fs` terutama pada area `blackrose` dengan memberikan antarmuka virtual `/blackrose/<namafile>`  yang sudah mencakup pada kesuluruhan fungsi-fungsi umum FUSE area chiho dengan ekstensi `.bin` dan menyimpan data file biasa dalam format biner dalam direktori `dragon area`. Secara sederhana, mekanisme utamanya bekerja sebagai berikut:

**1. `blackrose_getattr(const char *relpath, struct stat *stbuf)`**

-   Gabungkan path: `blackrose/<relpath>.bin`.
-   Ambil metadata file via `lstat()`.

**2. `blackrose_readdir(void *buf, fuse_fill_dir_t filler)`**

-   Buka direktori `BLACKROSE_DIR`.
-   Filter file dengan ekstensi `.bin`.
-   Strip ekstensi `.bin` sebelum ditampilkan di `ls`.

**3. `blackrose_read(...)`**

-   Buka file `.bin` → baca isinya dengan `pread()` dengan menyimpan dalam format biner.

**4. `blackrose_write(...)`**

-   Tulis data secara langsung ke file `.bin` dengan `pwrite()` dengan menyimpan dalam format biner.

**5. `blackrose_create(...)`**

-   Buat file `.bin` baru di direktori `blackrose`.

**6. `blackrose_unlink(...)`**

-   Hapus file `.bin` dari sistem file nyata.

```
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

[...]

int heaven_unlink(const char *relpath) {
    char *fpath = add_extension(HEAVEN_DIR, relpath, HEAVEN_EXT);
    int res = unlink(fpath);
    free(fpath);
    return res == -1 ? -errno : 0;
}
```
Berfungsi sebagai dasar pemrosesan file pada sistem FUSE `maimai_fs` terutama pada area `heaven` dengan memberikan antarmuka virtual `/heaven/<namafile>`  yang sudah mencakup pada kesuluruhan fungsi-fungsi umum FUSE area chiho dengan ekstensi `.enc` dan menyimpan data file dengan enkripsi dan AES-256-CBC via OpenSSL CLI, serta menyimpan IV  di awal file dalam direktori `heaven area`. Secara sederhana, mekanisme utamanya bekerja sebagai berikut:

**1. `heaven_getattr(...)`**

-   Gabungkan path → `heaven/namafile.enc`.
-   Ambil metadata dengan `lstat()`.

**2. `heaven_readdir(...)`**

-   Buka direktori `heaven`.
-   Tampilkan hanya file `.enc`, dan hilangkan `.enc` saat ditampilkan.

**3. `heaven_write(...)`**

**Langkah-langkah:**

1.  **Cek offset**: hanya mendukung `offset == 0`, artinya overwrite total.
    
2.  **Generate IV** (16 byte random dari `/dev/urandom`).
    
3.  **Simpan IV ke awal file output `.enc`**.
    
4.  Simpan plaintext sementara ke `/tmp/heaven_in_XXXXXX`.
    
5.  Gunakan `openssl` untuk mengenkripsi dengan AES-256-CBC:

    `openssl enc -aes-256-cbc -K HEAVEN_KEY -iv IV_HEX -in heaven_in -out heaven_out` 
    
7.  Tambahkan ciphertext ke file `.enc` setelah IV.
    
8.  Hapus file sementara.
    

**4. `heaven_read(...)`**

**Langkah-langkah:**

1.  **Cek offset**: hanya mendukung `offset == 0`.
2.  Buka file `.enc` → baca 16 byte pertama sebagai IV.
3.  Sisanya disimpan ke file `/tmp/heaven_ct_XXXXXX` (ciphertext).
4.  Gunakan `openssl enc -d` untuk mendekripsi ke `/tmp/heaven_pt_XXXXXX`.
    
5.  Baca hasil dekripsi ke buffer dan kirim ke user.
    

**5. `heaven_create(...)`**

-   Buat file `.enc` kosong di direktori `heaven`.

**6. `heaven_unlink(...)`**

-   Hapus file `.enc` dari direktori `heaven`.

```
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

[...]

int youth_unlink(const char *relpath) {
    char *fpath = add_extension(YOUTH_DIR, relpath, YOUTH_EXT);
    int res = unlink(fpath);
    free(fpath);
    return res == -1 ? -errno : 0;
}
```
Berfungsi sebagai dasar pemrosesan file pada sistem FUSE `maimai_fs` terutama pada area `skystreet` dengan memberikan antarmuka virtual `/skystreet/<namafile>`  yang sudah mencakup pada kesuluruhan fungsi-fungsi umum FUSE area chiho dengan ekstensi `.gz` dan menyimpan data file kompresi dan dekompresi gzip melalui library `zlib` dalam direktori `skystreet area`. Secara sederhana, mekanisme utamanya bekerja sebagai berikut:

**1. `youth_getattr(...)`**

-   Gabungkan path → `skystreet/<namafile>.gz`.
-   Ambil metadata file dengan `lstat()`.

**2. `youth_readdir(...)`**

-   Buka direktori `skystreet`.
-   Tampilkan hanya file berakhiran `.gz`, lalu strip ekstensi saat ditampilkan ke user.

**3. `youth_read(...)`**

**Langkah-langkah:**

1.  **Cek offset**: hanya mendukung `offset == 0`.
2.  Buka file `.gz` dengan `gzopen()` (mode `"rb"`).
3.  Baca dan dekompresi data ke `buf` menggunakan `gzread()`.
    

**4. `youth_write(...)`**

Langkah-langkah:

1.  **Cek offset**: hanya mendukung `offset == 0`.
2.  Buka file `.gz` dengan `gzopen()` (mode `"wb"`).
3.  Kompresi data dari `buf` ke disk menggunakan `gzwrite()`.

**5. `youth_create(...)`**

-   Buat file `.gz` kosong di direktori `skystreet`.

**6. `youth_unlink(...)`**

-   Hapus file `.gz` dari sistem file nyata.

Sebelum ke `7sref area`, **area `7sref`** harus berada setelah FUSE callbacks karena ia hanya **bekerja sebagai perantara yang membutuhkan fungsi-fungsi utama tersebut untuk berfungsi**, sehingga kode akan memiliki urutan struktural kode lebih modular dan logis. Dengan itu, penggunaan **FUSE Callbacks** akan didahulukan terlebih dahulu disini.
```
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
        
        [...]

static struct fuse_operations maimai_oper = {
    .getattr = maimai_getattr,
    .readdir = maimai_readdir,
    .open    = maimai_open,
    .read    = maimai_read,
    .write   = maimai_write,
    .create  = maimai_create,
    .unlink  = maimai_unlink,
};
```
Dalam program `maimai_fs` sendiri, FUSE Callbacks berfungsi untuk mendistribusikan operasi filesystem (`ls`, `cat`, `echo`, dll) ke handler sesuai area chiho-nya (`starter`, `metro`, dst.) secara otomatis saat melakukan aktivitas operasi filesystem / sebagai *router* untuk semua aktivitas file pada `maimai_fs`. Secara sederhana, FUSE Callbacks dalam `maimai_fs` sendiri bertindak sebagai **jembatan** antara aktivitas user dan logika per-area.
FUSE Callbacks sendiri memiliki struktur sebagai berikut:
|Callback FUSE|Fungsi|Handler di `maimai_fs.c`|
|---|---|---|
|`getattr`|Ambil info file/direktori|`maimai_getattr()`|
|`readdir`|Baca isi direktori (`ls`)|`maimai_readdir()`|
|`read`|Baca isi file|`maimai_read()`|
|`write`|Tulis data ke file|`maimai_write()`|
|`create`|Buat file baru|`maimai_create()`|
|`unlink`|Hapus file|`maimai_unlink()`|
|`open`|Validasi/izin buka file (optional use)|`maimai_open()`|

```
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

[...]

            char combined[512];
            snprintf(combined, sizeof(combined), "%s_%s", areas[i].prefix, stripped);
            filler(buf, combined, NULL, 0, 0);
        }

        closedir(dp);
    }

    return 0;
}
```
Berfungsi sebagai dasar pada *Gateaway* setiap file pada sistem FUSE `maimai_fs` di seluruh area chiho (`starter`, `metro`, `dragon`, dsb) dan bekerja dengan menyediakan direktori virtual `/7sref/<kode_area>_<namafile>` yang *redirect* operasi ke area Chiho lain (`starter`, `metro`, dll.) dan sebagai area spesial yang dapat mengakses semua area lain melalui sistem penamaan khusus setiap area. Secara sederhana, mekanisme utamanya bekerja sebagai berikut:

Mekanisme dasar `7sref area` :
-   File yang diakses di `/7sref/` harus mengikuti format:
    
    `<area>_<namafile>` 
    
    Misalnya:
    
    -   `fuse_dir/starter/test.txt <-> fuse_dir/7sref/starter_test.txt`
        
    -   `fuse_dir/heaven/test.txt <-> fuse_dir/7sref/heaven_test.txt``
        
-   Area ini menjadi **"jendela ke semua area"**.

Dengan mekanisme komponen utama :

**1. `parse_7sref_path(relpath, area_out, file_out)`**

-   Memisahkan path `starter_intro` menjadi:
    
    -   `area = "starter"`
        
    -   `file = "intro"`
        
-   Validasi: harus ada `_` sebagai pemisah.

**2. Fungsi-fungsi `sref_*()`:**
|Fungsi|Tugas|
|---|---|
|`sref_getattr()`|Redirect ke `maimai_getattr()` dengan path `/area/file`|
|`sref_readdir()`|Gabungkan isi dari semua area, beri prefix area_, tampilkan|
|`sref_read()`|Redirect ke `maimai_read("/area/file")`|
|`sref_write()`|Redirect ke `maimai_write("/area/file")`|
|`sref_create()`|Redirect ke `maimai_create("/area/file")`|
|`sref_unlink()`|Redirect ke `maimai_unlink("/area/file")`|

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
