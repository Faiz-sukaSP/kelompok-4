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

// Struck untuk data buku
typedef struct{
    char id_buku[20];
    char judul_buku[100];
    char penulis[80];
    Kategori kategori;
    Status status;
    int jumlah_dipinjam;
} Buku;

// Struck untuk anggota
typedef struct {

};
