#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

// Konstanta
const int MAX_BUKU = 100;
const int MAX_ANGGOTA = 50;
const int MAX_PEMINJAMAN = 150;
const int MAX_DENDA = 1000;
const int LAMA_PEMINJAMAN = 7;
const int BANYAK_PINJAMAN_USER = 5;

// File name
#define FILE_BUKU "buku.txt"
#define FILE_ANGGOTA "anggota.txt"
#define FILE_PEMINJAMAN "peminjaman.txt"
#define FILE_PUSTAKAWAN "pustakawan.txt"
#define FILE_PENGATURAN "pengaturan.txt"


// Menentukan Kategori Buku
typedef enum{
    FIKSI = 1, 
    NON_FIKSI, 
    KOMIK, 
    MAJALAH, 
    REFERENSI
} Kategori;

// Menentukan Ketersediaan Buku
typedef enum{
    TERSEDIA = 1,
    DIPINJAM,
} Ketersediaan;
 
// Mendeklarasikan data dan keter
typedef struct {
    char id_buku[20];
    char judul[60];
    char penulis[60];
    Kategori kategori_buku;
    Ketersediaan status;
    int jumlah_dipinjam; 
} Buku;

// Sturct untuk peminjaman buku
typedef struct{
    char id_peminjaman[20];
    char id_buku[20];
    char npm_anggota[20];
    char waktu_peminjaman[20];
    char waktu_pengembalian[20];
    char status_pengembalian;
    int denda;
} Peminjaman; 

// Struct untuk Login Pustakawan
typedef struct {
    char id[20];
    char password[50];
} Pustakawan;

//Struct untuk Anggota 
typedef struct {
    char NPM[30];
    char nama[100];
    char alamat[150];
    char kontak[30];
} Anggota;

// Variabel global
Buku daftar_buku[MAX_BUKU];
Anggota daftar_anggota[MAX_ANGGOTA];
Peminjaman daftar_peminjaman[MAX_PEMINJAMAN];
int jumlah_buku = 0;
int jumlah_anggota = 0;
int jumlah_peminjaman = 0;
int durasi_peminjaman = LAMA_PEMINJAMAN; //Membatasi lama nya waktu peminjaman buku

                                            // Function prototypes

// Inisialisasi dan FILE 
void inisialisasi_file();
void muat_data();
void simpan_buku();
void simpan_anggota();
void simpan_peminjaman();
void simpan_pengaturan();

// Auntentifikasi pustakawan
int login_pustakawan(); 

// Pilihan menu
void menu_utama();
void menu_buku();
void menu_anggota();
void menu_peminjaman();

// CRUD buku
void tambah_buku();
void hapus_buku();
void edit_buku();
void lihat_daftar_buku();
void cari_buku();

// CRUD Anggota
void tambah_anggota();
void lihat_daftar_anggota();
void lihat_riwayat_peminjaman();

// Peminjaman buku
void pinjam_buku();
void kembalikan_buku();

// Laporan dan statis
void ranking_buku();
void kelola_durasi();
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

const char* status_string(Ketersediaan s) {
        switch(s) {
            case TERSEDIA: return "Tersedia";
            case DIPINJAM: return "Dipinjam";
            default: return "Tidak Diketahui";
        }
    }

int  hitung_selisih_hari(const char* tanggal1, const char* tanggal2);

//utility
    void clear_input_buffer(){
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    void pause_screen(){
        printf("\nTekan Enter untuk melanjutkan...");
    clear_input_buffer();
    getchar();
    }

    
// Fungsi main
int main(){

// Bagian 1,2,3 fungsi ke-1,2 & ke-1 & ke-1
    inisialisasi_file();
    muat_data();

        if(login_pustakawan()){
            menu_utama();
        }
        else{
            printf("\n=== Login Gagal!! ===\n");
        }

// Utility
    pause_screen();

// Bagian 7
    const char* kategori_string(Kategori k);
    const char* status_string(Ketersediaan s);
}

