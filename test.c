#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// ==================== KONSTANTA ====================
#define MAX_BUKU 100
#define MAX_ANGGOTA 50
#define MAX_PEMINJAMAN 200
#define MAX_PUSTAKAWAN 10
#define DENDA_PER_HARI 1000
#define DURASI_PINJAM_DEFAULT 7
#define MAX_PINJAM_PERUSER 3

#define FILE_BUKU "buku.txt"
#define FILE_ANGGOTA "anggota.txt"
#define FILE_PEMINJAMAN "peminjaman.txt"
#define FILE_PUSTAKAWAN "pustakawan.txt"
#define FILE_PENGATURAN "pengaturan.txt"

// ==================== ENUM ====================
typedef enum {
    FIKSI = 1, 
    NON_FIKSI, 
    KOMIK, 
    MAJALAH, 
    REFERENSI
} Kategori;

typedef enum {
    TERSEDIA = 1, 
    DIPINJAM
} Status;

// ==================== STRUCT ====================
typedef struct {
    char id[20];
    char judul[100];
    char penulis[100];
    Kategori kategori;
    Status status;
    int jumlah_dipinjam;
} Buku;

typedef struct {
    char id[20];
    char nama[100];
    char alamat[200];
    char telepon[20];
    int jumlah_pinjam_aktif;
} Anggota;

typedef struct {
    char id_peminjaman[20];
    char id_buku[20];
    char id_anggota[20];
    char tanggal_pinjam[20];
    char tanggal_kembali[20];
    int status_kembali; // 0=belum, 1=sudah
} Peminjaman;

typedef struct {
    char username[50];
    char password[50];
} Pustakawan;

// ==================== GLOBAL ====================
static Buku daftar_buku[MAX_BUKU];
static Anggota daftar_anggota[MAX_ANGGOTA];
static Peminjaman daftar_peminjaman[MAX_PEMINJAMAN];

static int jumlah_buku = 0;
static int jumlah_anggota = 0;
static int jumlah_peminjaman = 0;
static int durasi_pinjam = DURASI_PINJAM_DEFAULT;

// ==================== UTILITY ====================
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pause_screen(void) {
    printf("\nTekan Enter untuk melanjutkan...");
    getchar();
}

void clear_screen(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

const char* kategori_string(Kategori k) {
    switch(k) {
        case FIKSI: return "Fiksi";
        case NON_FIKSI: return "Non-Fiksi";
        case KOMIK: return "Komik";
        case MAJALAH: return "Majalah";
        case REFERENSI: return "Referensi";
        default: return "Tidak Diketahui";
    }
}

const char* status_string(Status s) {
    return (s == TERSEDIA) ? "Tersedia" : "Dipinjam";
}

void get_tanggal_sekarang(char* buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, 20, "%Y-%m-%d", tm_info);
}

void tambah_hari(const char* tanggal, int hari, char* hasil) {
    struct tm tm_date = {0};
    int y, m, d;
    sscanf(tanggal, "%d-%d-%d", &y, &m, &d);
    tm_date.tm_year = y - 1900;
    tm_date.tm_mon = m - 1;
    tm_date.tm_mday = d + hari;
    mktime(&tm_date);
    strftime(hasil, 20, "%Y-%m-%d", &tm_date);
}

int hitung_selisih_hari(const char* t1, const char* t2) {
    struct tm tm1 = {0}, tm2 = {0};
    int y1, m1, d1, y2, m2, d2;
    sscanf(t1, "%d-%d-%d", &y1, &m1, &d1);
    sscanf(t2, "%d-%d-%d", &y2, &m2, &d2);
    tm1.tm_year = y1 - 1900;
    tm1.tm_mon = m1 - 1;
    tm1.tm_mday = d1;
    tm2.tm_year = y2 - 1900;
    tm2.tm_mon = m2 - 1;
    tm2.tm_mday = d2;
    return (int)(difftime(mktime(&tm2), mktime(&tm1)) / 86400);
}

// ==================== FILE HANDLING ====================
void inisialisasi_file(void) {
    FILE *fp;
    
    // Inisialisasi file buku
    fp = fopen(FILE_BUKU, "a");
    if(fp) fclose(fp);
    
    // Inisialisasi file anggota
    fp = fopen(FILE_ANGGOTA, "a");
    if(fp) fclose(fp);
    
    // Inisialisasi file peminjaman
    fp = fopen(FILE_PEMINJAMAN, "a");
    if(fp) fclose(fp);
    
    // Inisialisasi file pustakawan dengan akun default
    fp = fopen(FILE_PUSTAKAWAN, "r");
    if(!fp) {
        fp = fopen(FILE_PUSTAKAWAN, "w");
        if(fp) {
            fprintf(fp, "admin admin123\n");
            fclose(fp);
        }
    } else {
        fclose(fp);
    }
    
    // Inisialisasi file pengaturan
    fp = fopen(FILE_PENGATURAN, "r");
    if(!fp) {
        fp = fopen(FILE_PENGATURAN, "w");
        if(fp) {
            fprintf(fp, "%d\n", DURASI_PINJAM_DEFAULT);
            fclose(fp);
        }
    } else {
        fclose(fp);
    }
}

void muat_data(void) {
    FILE *fp;
    
    // Muat data buku
    fp = fopen(FILE_BUKU, "r");
    if(fp) {
        jumlah_buku = 0;
        while(fscanf(fp, "%[^|]|%[^|]|%[^|]|%d|%d|%d\n",
                     daftar_buku[jumlah_buku].id,
                     daftar_buku[jumlah_buku].judul,
                     daftar_buku[jumlah_buku].penulis,
                     (int*)&daftar_buku[jumlah_buku].kategori,
                     (int*)&daftar_buku[jumlah_buku].status,
                     &daftar_buku[jumlah_buku].jumlah_dipinjam) == 6) {
            jumlah_buku++;
            if(jumlah_buku >= MAX_BUKU) break;
        }
        fclose(fp);
    }
    
    // Muat data anggota
    fp = fopen(FILE_ANGGOTA, "r");
    if(fp) {
        jumlah_anggota = 0;
        while(fscanf(fp, "%[^|]|%[^|]|%[^|]|%[^|]|%d\n",
                     daftar_anggota[jumlah_anggota].id,
                     daftar_anggota[jumlah_anggota].nama,
                     daftar_anggota[jumlah_anggota].alamat,
                     daftar_anggota[jumlah_anggota].telepon,
                     &daftar_anggota[jumlah_anggota].jumlah_pinjam_aktif) == 5) {
            jumlah_anggota++;
            if(jumlah_anggota >= MAX_ANGGOTA) break;
        }
        fclose(fp);
    }
    
    // Muat data peminjaman
    fp = fopen(FILE_PEMINJAMAN, "r");
    if(fp) {
        jumlah_peminjaman = 0;
        while(fscanf(fp, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%d\n",
                     daftar_peminjaman[jumlah_peminjaman].id_peminjaman,
                     daftar_peminjaman[jumlah_peminjaman].id_buku,
                     daftar_peminjaman[jumlah_peminjaman].id_anggota,
                     daftar_peminjaman[jumlah_peminjaman].tanggal_pinjam,
                     daftar_peminjaman[jumlah_peminjaman].tanggal_kembali,
                     &daftar_peminjaman[jumlah_peminjaman].status_kembali) == 6) {
            jumlah_peminjaman++;
            if(jumlah_peminjaman >= MAX_PEMINJAMAN) break;
        }
        fclose(fp);
    }
    
    // Muat pengaturan
    fp = fopen(FILE_PENGATURAN, "r");
    if(fp) {
        fscanf(fp, "%d", &durasi_pinjam);
        fclose(fp);
    }
}

void simpan_buku(void) {
    FILE *fp = fopen(FILE_BUKU, "w");
    if(fp) {
        for(int i = 0; i < jumlah_buku; i++) {
            fprintf(fp, "%s|%s|%s|%d|%d|%d\n",
                    daftar_buku[i].id,
                    daftar_buku[i].judul,
                    daftar_buku[i].penulis,
                    daftar_buku[i].kategori,
                    daftar_buku[i].status,
                    daftar_buku[i].jumlah_dipinjam);
        }
        fclose(fp);
    }
}

void simpan_anggota(void) {
    FILE *fp = fopen(FILE_ANGGOTA, "w");
    if(fp) {
        for(int i = 0; i < jumlah_anggota; i++) {
            fprintf(fp, "%s|%s|%s|%s|%d\n",
                    daftar_anggota[i].id,
                    daftar_anggota[i].nama,
                    daftar_anggota[i].alamat,
                    daftar_anggota[i].telepon,
                    daftar_anggota[i].jumlah_pinjam_aktif);
        }
        fclose(fp);
    }
}

void simpan_peminjaman(void) {
    FILE *fp = fopen(FILE_PEMINJAMAN, "w");
    if(fp) {
        for(int i = 0; i < jumlah_peminjaman; i++) {
            fprintf(fp, "%s|%s|%s|%s|%s|%d\n",
                    daftar_peminjaman[i].id_peminjaman,
                    daftar_peminjaman[i].id_buku,
                    daftar_peminjaman[i].id_anggota,
                    daftar_peminjaman[i].tanggal_pinjam,
                    daftar_peminjaman[i].tanggal_kembali,
                    daftar_peminjaman[i].status_kembali);
        }
        fclose(fp);
    }
}

void simpan_pengaturan(void) {
    FILE *fp = fopen(FILE_PENGATURAN, "w");
    if(fp) {
        fprintf(fp, "%d\n", durasi_pinjam);
        fclose(fp);
    }
}

// ==================== LOGIN (DIPERBAIKI) ====================
int login_pustakawan(void) {
    Pustakawan p[MAX_PUSTAKAWAN];
    int n = 0;
    char username[50], password[50];
    
    // Muat data pustakawan dari file
    FILE *fp = fopen(FILE_PUSTAKAWAN, "r");
    if(fp) {
        while(fscanf(fp, "%s %s", p[n].username, p[n].password) == 2) {
            n++;
            if(n >= MAX_PUSTAKAWAN) break;
        }
        fclose(fp);
    }
    
    // Input username dan password
    printf("\n==================================\n");
    printf("         LOGIN PUSTAKAWAN         \n");
    printf("==================================\n");
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);
    clear_input_buffer(); // PERBAIKAN: Tambahkan ini
    
    // Validasi login
    for(int i = 0; i < n; i++) {
        if(strcmp(username, p[i].username) == 0 && 
           strcmp(password, p[i].password) == 0) {
            printf("\n✓ Login berhasil! Selamat datang, %s!\n", username);
            pause_screen();
            return 1;
        }
    }
    
    printf("\n✗ Login gagal! Username atau password salah.\n");
    pause_screen();
    return 0;
}

// ==================== CARI INDEX ====================
int cari_index_buku(const char* id) {
    for(int i = 0; i < jumlah_buku; i++) {
        if(strcmp(daftar_buku[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

int cari_index_anggota(const char* id) {
    for(int i = 0; i < jumlah_anggota; i++) {
        if(strcmp(daftar_anggota[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

int hitung_pinjaman_aktif(const char* id_anggota) {
    int count = 0;
    for(int i = 0; i < jumlah_peminjaman; i++) {
        if(strcmp(daftar_peminjaman[i].id_anggota, id_anggota) == 0 && 
           daftar_peminjaman[i].status_kembali == 0) {
            count++;
        }
    }
    return count;
}

// ==================== CRUD BUKU ====================
void tambah_buku(void) {
    if(jumlah_buku >= MAX_BUKU) {
        printf("\n✗ Database buku sudah penuh!\n");
        pause_screen();
        return;
    }
    
    Buku b;
    int k;
    
    printf("\n==================================\n");
    printf("          TAMBAH BUKU             \n");
    printf("==================================\n");
    
    printf("ID Buku: ");
    scanf("%s", b.id);
    clear_input_buffer();
    
    if(cari_index_buku(b.id) != -1) {
        printf("\n✗ ID buku sudah ada!\n");
        pause_screen();
        return;
    }
    
    printf("Judul: ");
    fgets(b.judul, sizeof(b.judul), stdin);
    b.judul[strcspn(b.judul, "\n")] = 0;
    
    printf("Penulis: ");
    fgets(b.penulis, sizeof(b.penulis), stdin);
    b.penulis[strcspn(b.penulis, "\n")] = 0;
    
    printf("\nKategori:\n");
    printf("1. Fiksi\n");
    printf("2. Non-Fiksi\n");
    printf("3. Komik\n");
    printf("4. Majalah\n");
    printf("5. Referensi\n");
    printf("Pilih (1-5): ");
    scanf("%d", &k);
    
    if(k < 1 || k > 5) {
        printf("\n✗ Kategori tidak valid!\n");
        pause_screen();
        return;
    }
    
    b.kategori = (Kategori)k;
    b.status = TERSEDIA;
    b.jumlah_dipinjam = 0;
    
    daftar_buku[jumlah_buku++] = b;
    simpan_buku();
    
    printf("\n✓ Buku berhasil ditambahkan!\n");
    pause_screen();
}

void lihat_daftar_buku(void) {
    if(jumlah_buku == 0) {
        printf("\n✗ Belum ada buku dalam database.\n");
        pause_screen();
        return;
    }
    
    printf("\n==================================\n");
    printf("         DAFTAR BUKU              \n");
    printf("==================================\n");
    printf("%-10s %-30s %-20s %-15s %-10s\n", 
           "ID", "Judul", "Penulis", "Kategori", "Status");
    printf("----------------------------------\n");
    
    for(int i = 0; i < jumlah_buku; i++) {
        printf("%-10s %-30s %-20s %-15s %-10s\n",
               daftar_buku[i].id,
               daftar_buku[i].judul,
               daftar_buku[i].penulis,
               kategori_string(daftar_buku[i].kategori),
               status_string(daftar_buku[i].status));
    }
    
    pause_screen();
}

void hapus_buku(void) {
    char id[20];
    int idx;
    
    printf("\n==================================\n");
    printf("          HAPUS BUKU              \n");
    printf("==================================\n");
    printf("ID Buku: ");
    scanf("%s", id);
    
    idx = cari_index_buku(id);
    if(idx == -1) {
        printf("\n✗ Buku tidak ditemukan!\n");
        pause_screen();
        return;
    }
    
    if(daftar_buku[idx].status == DIPINJAM) {
        printf("\n✗ Buku sedang dipinjam, tidak bisa dihapus!\n");
        pause_screen();
        return;
    }
    
    // Geser array
    for(int i = idx; i < jumlah_buku - 1; i++) {
        daftar_buku[i] = daftar_buku[i + 1];
    }
    
    jumlah_buku--;
    simpan_buku();
    
    printf("\n✓ Buku berhasil dihapus!\n");
    pause_screen();
}

void menu_buku(void) {
    int pilihan;
    do {
        clear_screen();
        printf("\n==================================\n");
        printf("         KELOLA BUKU              \n");
        printf("==================================\n");
        printf("1. Tambah Buku\n");
        printf("2. Hapus Buku\n");
        printf("3. Lihat Daftar Buku\n");
        printf("0. Kembali\n");
        printf("==================================\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);
        
        switch(pilihan) {
            case 1: tambah_buku(); break;
            case 2: hapus_buku(); break;
            case 3: lihat_daftar_buku(); break;
            case 0: break;
            default: 
                printf("\n✗ Pilihan tidak valid!\n");
                pause_screen();
        }
    } while(pilihan != 0);
}

// ==================== CRUD ANGGOTA ====================
void tambah_anggota(void) {
    if(jumlah_anggota >= MAX_ANGGOTA) {
        printf("\n✗ Database anggota sudah penuh!\n");
        pause_screen();
        return;
    }
    
    Anggota a;
    
    printf("\n==================================\n");
    printf("         TAMBAH ANGGOTA           \n");
    printf("==================================\n");
    
    printf("ID: ");
    scanf("%s", a.id);
    clear_input_buffer();
    
    if(cari_index_anggota(a.id) != -1) {
        printf("\n✗ ID anggota sudah ada!\n");
        pause_screen();
        return;
    }
    
    printf("Nama: ");
    fgets(a.nama, sizeof(a.nama), stdin);
    a.nama[strcspn(a.nama, "\n")] = 0;
    
    printf("Alamat: ");
    fgets(a.alamat, sizeof(a.alamat), stdin);
    a.alamat[strcspn(a.alamat, "\n")] = 0;
    
    printf("Telepon: ");
    fgets(a.telepon, sizeof(a.telepon), stdin);
    a.telepon[strcspn(a.telepon, "\n")] = 0;
    
    a.jumlah_pinjam_aktif = 0;
    daftar_anggota[jumlah_anggota++] = a;
    simpan_anggota();
    
    printf("\n✓ Anggota berhasil ditambahkan!\n");
    pause_screen();
}

void lihat_daftar_anggota(void) {
    if(jumlah_anggota == 0) {
        printf("\n✗ Belum ada anggota dalam database.\n");
        pause_screen();
        return;
    }
    
    printf("\n==================================\n");
    printf("        DAFTAR ANGGOTA            \n");
    printf("==================================\n");
    printf("%-10s %-30s %-15s %-10s\n", 
           "ID", "Nama", "Telepon", "Pinjaman");
    printf("----------------------------------\n");
    
    for(int i = 0; i < jumlah_anggota; i++) {
        printf("%-10s %-30s %-15s %-10d\n",
               daftar_anggota[i].id,
               daftar_anggota[i].nama,
               daftar_anggota[i].telepon,
               daftar_anggota[i].jumlah_pinjam_aktif);
    }
    
    pause_screen();
}

void menu_anggota(void) {
    int pilihan;
    do {
        clear_screen();
        printf("\n==================================\n");
        printf("        KELOLA ANGGOTA            \n");
        printf("==================================\n");
        printf("1. Tambah Anggota\n");
        printf("2. Lihat Daftar Anggota\n");
        printf("0. Kembali\n");
        printf("==================================\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);
        
        switch(pilihan) {
            case 1: tambah_anggota(); break;
            case 2: lihat_daftar_anggota(); break;
            case 0: break;
            default: 
                printf("\n✗ Pilihan tidak valid!\n");
                pause_screen();
        }
    } while(pilihan != 0);
}

// ==================== PEMINJAMAN ====================
void pinjam_buku(void) {
    char id_buku[20], id_anggota[20];
    
    printf("\n==================================\n");
    printf("         PINJAM BUKU              \n");
    printf("==================================\n");
    
    printf("ID Buku: ");
    scanf("%s", id_buku);
    printf("ID Anggota: ");
    scanf("%s", id_anggota);
    
    int idx_buku = cari_index_buku(id_buku);
    int idx_anggota = cari_index_anggota(id_anggota);
    
    if(idx_buku == -1 || idx_anggota == -1) {
        printf("\n✗ Buku atau anggota tidak ditemukan!\n");
        pause_screen();
        return;
    }
    
    if(daftar_buku[idx_buku].status == DIPINJAM) {
        printf("\n✗ Buku sedang dipinjam!\n");
        pause_screen();
        return;
    }
    
    if(hitung_pinjaman_aktif(id_anggota) >= MAX_PINJAM_PERUSER) {
        printf("\n✗ Anggota sudah mencapai batas maksimal peminjaman (%d buku)!\n", 
               MAX_PINJAM_PERUSER);
        pause_screen();
        return;
    }
    
    Peminjaman p;
    sprintf(p.id_peminjaman, "P%03d", jumlah_peminjaman + 1);
    strcpy(p.id_buku, id_buku);
    strcpy(p.id_anggota, id_anggota);
    get_tanggal_sekarang(p.tanggal_pinjam);
    tambah_hari(p.tanggal_pinjam, durasi_pinjam, p.tanggal_kembali);
    p.status_kembali = 0;
    
    daftar_peminjaman[jumlah_peminjaman++] = p;
    daftar_buku[idx_buku].status = DIPINJAM;
    daftar_buku[idx_buku].jumlah_dipinjam++;
    daftar_anggota[idx_anggota].jumlah_pinjam_aktif++;
    
    simpan_buku();
    simpan_anggota();
    simpan_peminjaman();
    
    printf("\n✓ Buku berhasil dipinjam!\n");
    printf("ID Peminjaman: %s\n", p.id_peminjaman);
    printf("Tanggal Pinjam: %s\n", p.tanggal_pinjam);
    printf("Tanggal Kembali: %s\n", p.tanggal_kembali);
    pause_screen();
}

void kembalikan_buku(void) {
    char id_peminjaman[20];
    
    printf("\n==================================\n");
    printf("       KEMBALIKAN BUKU            \n");
    printf("==================================\n");
    printf("ID Peminjaman: ");
    scanf("%s", id_peminjaman);
    
    int idx = -1;
    for(int i = 0; i < jumlah_peminjaman; i++) {
        if(strcmp(daftar_peminjaman[i].id_peminjaman, id_peminjaman) == 0) {
            idx = i;
            break;
        }
    }
    
    if(idx == -1) {
        printf("\n✗ Peminjaman tidak ditemukan!\n");
        pause_screen();
        return;
    }
    
    if(daftar_peminjaman[idx].status_kembali == 1) {
        printf("\n✗ Buku sudah dikembalikan sebelumnya!\n");
        pause_screen();
        return;
    }
    
    int idx_buku = cari_index_buku(daftar_peminjaman[idx].id_buku);
    int idx_anggota = cari_index_anggota(daftar_peminjaman[idx].id_anggota);
    
    daftar_peminjaman[idx].status_kembali = 1;
    
    if(idx_buku != -1) {
        daftar_buku[idx_buku].status = TERSEDIA;
    }
    
    if(idx_anggota != -1 && daftar_anggota[idx_anggota].jumlah_pinjam_aktif > 0) {
        daftar_anggota[idx_anggota].jumlah_pinjam_aktif--;
    }
    
    simpan_buku();
    simpan_anggota();
    simpan_peminjaman();
    
    printf("\n✓ Buku berhasil dikembalikan!\n");
    pause_screen();
}

void lihat_peminjaman_aktif(void) {
    int ada = 0;
    
    printf("\n====================================================================\n");
    printf("                      PEMINJAMAN AKTIF                              \n");
    printf("====================================================================\n");
    printf("%-12s %-12s %-12s %-12s %-12s\n", 
           "ID Pinjam", "ID Buku", "ID Anggota", "Tgl Pinjam", "Tgl Kembali");
    printf("--------------------------------------------------------------------\n");
    
    for(int i = 0; i < jumlah_peminjaman; i++) {
        if(daftar_peminjaman[i].status_kembali == 0) {
            printf("%-12s %-12s %-12s %-12s %-12s\n",
                   daftar_peminjaman[i].id_peminjaman,
                   daftar_peminjaman[i].id_buku,
                   daftar_peminjaman[i].id_anggota,
                   daftar_peminjaman[i].tanggal_pinjam,
                   daftar_peminjaman[i].tanggal_kembali);
            ada = 1;
        }
    }
    
    if(!ada) {
        printf("\n✗ Tidak ada peminjaman aktif.\n");
    }
    
    pause_screen();
}

void lihat_semua_peminjaman(void) {
    if(jumlah_peminjaman == 0) {
        printf("\n✗ Belum ada data peminjaman.\n");
        pause_screen();
        return;
    }
    
    printf("\n====================================================================\n");
    printf("                    DAFTAR SEMUA PEMINJAMAN                         \n");
    printf("====================================================================\n");
    printf("%-12s %-12s %-12s %-12s %-12s %-10s\n", 
           "ID Pinjam", "ID Buku", "ID Anggota", "Tgl Pinjam", "Tgl Kembali", "Status");
    printf("--------------------------------------------------------------------\n");
    
    for(int i = 0; i < jumlah_peminjaman; i++) {
        printf("%-12s %-12s %-12s %-12s %-12s %-10s\n",
               daftar_peminjaman[i].id_peminjaman,
               daftar_peminjaman[i].id_buku,
               daftar_peminjaman[i].id_anggota,
               daftar_peminjaman[i].tanggal_pinjam,
               daftar_peminjaman[i].tanggal_kembali,
               daftar_peminjaman[i].status_kembali == 0 ? "Aktif" : "Kembali");
    }
    
    pause_screen();
}

void lihat_peminjaman_per_anggota(void) {
    char id_anggota[20];
    int ada = 0;
    
    printf("\n==================================\n");
    printf("    PEMINJAMAN PER ANGGOTA        \n");
    printf("==================================\n");
    printf("ID Anggota: ");
    scanf("%s", id_anggota);
    
    int idx_anggota = cari_index_anggota(id_anggota);
    if(idx_anggota == -1) {
        printf("\n✗ Anggota tidak ditemukan!\n");
        pause_screen();
        return;
    }
    
    printf("\nAnggota: %s (%s)\n", 
           daftar_anggota[idx_anggota].nama, 
           daftar_anggota[idx_anggota].id);
    printf("====================================================================\n");
    printf("%-12s %-12s %-12s %-12s %-10s\n", 
           "ID Pinjam", "ID Buku", "Tgl Pinjam", "Tgl Kembali", "Status");
    printf("--------------------------------------------------------------------\n");
    
    for(int i = 0; i < jumlah_peminjaman; i++) {
        if(strcmp(daftar_peminjaman[i].id_anggota, id_anggota) == 0) {
            printf("%-12s %-12s %-12s %-12s %-10s\n",
                   daftar_peminjaman[i].id_peminjaman,
                   daftar_peminjaman[i].id_buku,
                   daftar_peminjaman[i].tanggal_pinjam,
                   daftar_peminjaman[i].tanggal_kembali,
                   daftar_peminjaman[i].status_kembali == 0 ? "Aktif" : "Kembali");
            ada = 1;
        }
    }
    
    if(!ada) {
        printf("\n✗ Anggota ini belum pernah meminjam buku.\n");
    }
    
    pause_screen();
}

void lihat_riwayat_buku(void) {
    char id_buku[20];
    int ada = 0;
    
    printf("\n==================================\n");
    printf("      RIWAYAT PEMINJAMAN BUKU     \n");
    printf("==================================\n");
    printf("ID Buku: ");
    scanf("%s", id_buku);
    
    int idx_buku = cari_index_buku(id_buku);
    if(idx_buku == -1) {
        printf("\n✗ Buku tidak ditemukan!\n");
        pause_screen();
        return;
    }
    
    printf("\nBuku: %s - %s\n", 
           daftar_buku[idx_buku].judul, 
           daftar_buku[idx_buku].penulis);
    printf("Total Dipinjam: %d kali\n", daftar_buku[idx_buku].jumlah_dipinjam);
    printf("====================================================================\n");
    printf("%-12s %-12s %-12s %-12s %-10s\n", 
           "ID Pinjam", "ID Anggota", "Tgl Pinjam", "Tgl Kembali", "Status");
    printf("--------------------------------------------------------------------\n");
    
    for(int i = 0; i < jumlah_peminjaman; i++) {
        if(strcmp(daftar_peminjaman[i].id_buku, id_buku) == 0) {
            printf("%-12s %-12s %-12s %-12s %-10s\n",
                   daftar_peminjaman[i].id_peminjaman,
                   daftar_peminjaman[i].id_anggota,
                   daftar_peminjaman[i].tanggal_pinjam,
                   daftar_peminjaman[i].tanggal_kembali,
                   daftar_peminjaman[i].status_kembali == 0 ? "Aktif" : "Kembali");
            ada = 1;
        }
    }
    
    if(!ada) {
        printf("\n✗ Buku ini belum pernah dipinjam.\n");
    }
    
    pause_screen();
}

void detail_peminjaman(void) {
    char id_peminjaman[20];
    
    printf("\n==================================\n");
    printf("      DETAIL PEMINJAMAN           \n");
    printf("==================================\n");
    printf("ID Peminjaman: ");
    scanf("%s", id_peminjaman);
    
    int idx = -1;
    for(int i = 0; i < jumlah_peminjaman; i++) {
        if(strcmp(daftar_peminjaman[i].id_peminjaman, id_peminjaman) == 0) {
            idx = i;
            break;
        }
    }
    
    if(idx == -1) {
        printf("\n✗ Peminjaman tidak ditemukan!\n");
        pause_screen();
        return;
    }
    
    // Ambil data buku
    int idx_buku = cari_index_buku(daftar_peminjaman[idx].id_buku);
    int idx_anggota = cari_index_anggota(daftar_peminjaman[idx].id_anggota);
    
    printf("\n====================================\n");
    printf("ID Peminjaman : %s\n", daftar_peminjaman[idx].id_peminjaman);
    printf("Status        : %s\n", 
           daftar_peminjaman[idx].status_kembali == 0 ? "Aktif" : "Sudah Dikembalikan");
    printf("====================================\n");
    
    if(idx_buku != -1) {
        printf("\n[INFORMASI BUKU]\n");
        printf("ID Buku       : %s\n", daftar_buku[idx_buku].id);
        printf("Judul         : %s\n", daftar_buku[idx_buku].judul);
        printf("Penulis       : %s\n", daftar_buku[idx_buku].penulis);
        printf("Kategori      : %s\n", kategori_string(daftar_buku[idx_buku].kategori));
    }
    
    if(idx_anggota != -1) {
        printf("\n[INFORMASI ANGGOTA]\n");
        printf("ID Anggota    : %s\n", daftar_anggota[idx_anggota].id);
        printf("Nama          : %s\n", daftar_anggota[idx_anggota].nama);
        printf("Telepon       : %s\n", daftar_anggota[idx_anggota].telepon);
    }
    
    printf("\n[INFORMASI WAKTU]\n");
    printf("Tanggal Pinjam: %s\n", daftar_peminjaman[idx].tanggal_pinjam);
    printf("Tanggal Kembali: %s\n", daftar_peminjaman[idx].tanggal_kembali);
    
    // Hitung keterlambatan jika belum dikembalikan
    if(daftar_peminjaman[idx].status_kembali == 0) {
        char tanggal_sekarang[20];
        get_tanggal_sekarang(tanggal_sekarang);
        int selisih = hitung_selisih_hari(daftar_peminjaman[idx].tanggal_kembali, tanggal_sekarang);
        
        if(selisih > 0) {
            int denda = selisih * DENDA_PER_HARI;
            printf("\n[PERINGATAN]\n");
            printf("Keterlambatan : %d hari\n", selisih);
            printf("Denda         : Rp %d\n", denda);
        } else if(selisih == 0) {
            printf("\n[INFO] Hari ini adalah batas pengembalian!\n");
        } else {
            printf("\n[INFO] Masih ada %d hari lagi sebelum batas pengembalian\n", -selisih);
        }
    }
    
    printf("====================================\n");
    pause_screen();
}
// ==================== MAIN PROGRAM ====================
int main(void) {
    inisialisasi_file();
    muat_data();
    
    if (login_pustakawan()) {
        menu_utama();

    } else {
        printf("\n=== Login Gagal! Program Ditutup ===\n");
    }
    
    return 0;
}

// ==================== MENU UTAMA ====================
void menu_utama(void) {
    int pilihan;
    
    do {
        clear_screen();
        printf("\n==================================\n");
        printf("         MENU UTAMA               \n");
        printf("==================================\n");
        printf("1. Kelola Buku\n");
        printf("2. Kelola Anggota\n");
        printf("3. Kelola Peminjaman\n");
        printf("4. Ranking Buku Terpopuler\n");
        printf("5. Statistik Perpustakaan\n");
        printf("6. Laporan Denda\n");
        printf("7. Pengaturan Durasi Peminjaman\n");
        printf("0. Keluar\n");
        printf("==================================\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);
        
        switch (pilihan) {
            case 1: menu_buku(); break;
            case 2: menu_anggota(); break;
            case 3: menu_peminjaman(); break;
            case 4: ranking_buku(); break;
            case 5: statistik_perpustakaan(); break;
            case 6: laporan_denda(); break;
            case 7: kelola_durasi(); break;
            case 0: 
                printf("\nTerima kasih telah menggunakan sistem ini!\n");
                break;
            default: 
                printf("\n✗ Pilihan tidak valid!\n");
                pause_screen();
        }
    } while (pilihan != 0);
}