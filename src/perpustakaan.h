#ifndef PERPUSTAKAAN_H
#define PERPUSTAKAAN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Konstanta 
#define MAX_BUKU 100
#define MAX_ANGGOTA 50
#define MAX_PEMINJAMAN 200
#define MAX_PUSTAKAWAN 10
#define DENDA_PER_HARI 1000
#define DURASI_PINJAM 7
#define MAX_PINJAM_USER 5

// Nama File
#define FILE_BUKU "Buku.txt"
#define FILE_ANGGOTA "Anggota.txt"
#define FILE_PEMINJAMAN "Peminjaman.txt"
#define FILE_PUSTAKAWAN "Pustakawan.txt"
#define FILE_PENGATURAN "Pengaturan.txt"
                                        // === ENUM ===
// Enum untuk kategori buku
typedef enum{
    FIKSI = 1,
    NON_FIKSI,
    KOMIK,
    MAJALAH,
    REFRENSI
} Kategori;

// Enum utuk mengetahui ketersediaan buku
typedef enum{
    TERSEDIA = 1,
    DIPINJAM
} Status;

// Kategori Buku
typedef enum { 
    FIKSI, NON_FIKSI, KOMIK, MAJALAH, REFERENSI 
} Kategori;

// Status Buku
typedef enum { 
    TERSEDIA, DIPINJAM 
} Status;
                                        //=== STRUCK ===
// Struck untuk data buku
typedef struct{
    char id_buku[20];
    char judul_buku[100];
    char penulis[80];
    Kategori kategori;
    Status status;
    int jumlah_dipinjam;
} Buku;

typedef struct {
    char id_peminjaman[30];
    char id_buku[20];
    char npm_user[20];
    char tanggal_pinjam[20];
    char tanggal_pengembalian[20];
    int status_pengembalian;
    int denda;
} Peminjaman;

// Struck untuk anggota
typedef struct {
    char npm_user[20];
    char nama_user[50];
    char alamat[200];
    char telepon[15];
    int pinjaman_aktif;
} Anggota;

typedef struct {
    char id_pustakawan[20];
    char pustakawan[20];
    char waktu_login[20];
    char username[20];
    char password[20];
} pustakawan;
                                        // === PROTOTYPE FUNGSI === //

// === PF PEMINJAMAN === //
int generateIDPeminjaman();
int uraiTanggal(const char *tgl)
double hitungDenda(const char *tglPinjam, const char *tglKembali);
bool validasiAnggota(const char *npm);
bool cekStatusBuku(const char *idBuku);
bool updateStatusBuku(const char *idBuku, const char *statusBaru);
void tanggalHariIni(char *buffer);
void simpanPeminjamanKeFile(Peminjaman p);
void loadPeminjamanDariFile(Peminjaman *arr, int *n);
void tambahRiwayat(Peminjaman p, const char *namaFile);
void pinjamBuku();
void kembalikanBuku();
void tampilkanDaftarPeminjaman();
void cariPeminjamanByNPM();

// === PF BUKU === //
void muatDataBuku(Buku *data, int *jumlah);
void simpanDataBuku(Buku *data, int jumlah);
void tampilkanDaftarBuku();
void tambahBuku();
void hapusBuku();
void editBuku();
void cariBuku();
void cariBukuByJudul(const char *judul);
void cariBukuByPenulis(const char *penulis);
void cariBukuByKategori(Kategori kategori);
int cariIndexBuku(const char *id_buku);
void urutkanBukuBerdasarkanPeminjaman(Buku *data, int jumlah);
void tampilkanRankingBuku();

// === PF UTILITY ===//
void clear_input_buffer(void);
void pause_screen(void);
const char* kategori_string(Kategori k);
const char* status_string(Status s);

// Tanggal
void get_tanggal_sekarang(char* buffer);
void tambah_hari(const char* tanggal, int hari, char* hasil);
int hitung_selisih_hari(const char* day1, const char* day2);

// Pencarian data
int cari_index_buku(const char* id);
int cari_index_anggota(const char* id);
int hitung_pinjaman_aktif(const char* id_anggota);

#endif
