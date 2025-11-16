#ifndef PERPUSTAKAAN_H
#define PERPUSTAKAAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>  // Tambahkan ini untuk tipe bool

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
typedef enum {
    FIKSI = 1,
    NON_FIKSI,
    KOMIK,
    MAJALAH,
    REFERENSI  // ✅ Diperbaiki dari REFRENSI
} Kategori;

// Enum untuk mengetahui ketersediaan buku
typedef enum {
    TERSEDIA = 1,
    DIPINJAM
} Status;

// === STRUCT ===
// Struct untuk data buku
typedef struct {
    char id_buku[20];
    char judul_buku[100];
    char penulis[80];
    Kategori kategori;
    Status status;
    int jumlah_dipinjam;
} Buku;

// Struct untuk peminjaman
typedef struct {
    char id_peminjaman[30];
    char id_buku[20];
    char npm_user[20];
    char tanggal_pinjam[20];
    char tanggal_pengembalian[20];
    int status_pengembalian;
    int denda;
} Peminjaman;

// Struct untuk anggota
typedef struct {
    char npm_user[20];
    char nama_user[50];
    char alamat[200];
    char telepon[15];
    int pinjaman_aktif;
} Anggota;

// Struct untuk pustakawan
typedef struct {
    char id_pustakawan[20];
    char pustakawan[20];
    char waktu_login[20];
    char username[20];
    char password[20];
} Pustakawan;  // ✅ Huruf kapital untuk konsistensi

// === VARIABEL GLOBAL ===
extern Buku daftar_buku[MAX_BUKU];
extern int jumlah_buku;

extern Anggota daftar_anggota[MAX_ANGGOTA];
extern int jumlah_anggota;

extern Peminjaman daftar_peminjaman[MAX_PEMINJAMAN];
extern int jumlah_peminjaman;

// === PROTOTYPE FUNGSI === 

// PF PEMINJAMAN
int generateIDPeminjaman();
int uraiTanggal(const char *tgl);  // ✅ Tambahkan semicolon
double hitungDenda(const char *tglPinjam, const char *tglKembali);  // ✅ Tambahkan semicolon
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

// === PF BUKU ===
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

#endif