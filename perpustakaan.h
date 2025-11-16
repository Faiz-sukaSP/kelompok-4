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
} pustakawan;
                                        // === PROTOTYPE FUNGSI === //

// PF PEMINJAMAN //
int generateIDPeminjaman();
bool validasiAnggota(const char *npm);
bool cekStatusBuku(const char *idBuku);
void tanggalHariIni(char *buffer);
double hitungDenda(const char *tglPinjam, const char *tglKembali);
bool updateStatusBuku(const char *idBuku, const char *statusBaru);

void simpanPeminjamanKeFile(Peminjaman p);
void loadPeminjamanDariFile(Peminjaman *arr, int *n);
void tambahRiwayat(Peminjaman p, const char *namaFile);

void pinjamBuku();
void kembalikanBuku();
void tampilkanDaftarPeminjaman();
void cariPeminjamanByNPM();

#endif
