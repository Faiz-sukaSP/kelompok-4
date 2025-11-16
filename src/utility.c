#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utility.h"

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pause_screen(void) {
    printf("\nTekan Enter untuk melanjutkan...");
    getchar();
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
    int tahun, bulan, tanggal_hari;
    
    sscanf(tanggal, "%d-%d-%d", &tahun, &bulan, &tanggal_hari);
    
    tm_date.tm_year = tahun - 1900;
    tm_date.tm_mon = bulan - 1;
    tm_date.tm_mday = tanggal_hari + hari;
    
    mktime(&tm_date);
    
    strftime(hasil, 20, "%Y-%m-%d", &tm_date);
}

int hitung_selisih_hari(const char* tanggal1, const char* tanggal2) {
    struct tm tm1 = {0}, tm2 = {0};
    int y1, m1, d1, y2, m2, d2;
    
    sscanf(tanggal1, "%d-%d-%d", &y1, &m1, &d1);
    sscanf(tanggal2, "%d-%d-%d", &y2, &m2, &d2);
    
    tm1.tm_year = y1 - 1900;
    tm1.tm_mon = m1 - 1;
    tm1.tm_mday = d1;
    
    tm2.tm_year = y2 - 1900;
    tm2.tm_mon = m2 - 1;
    tm2.tm_mday = d2;
    
    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);
    
    double diff = difftime(time2, time1);
    return (int)(diff / 86400);
}


int cari_index_buku(const char* id_buku) {
    for (int i = 0; i < jumlah_buku; i++) {
        if (strcmp(daftar_buku[i].id_buku, id_buku) == 0) {
            return i;
        }
    }
    return -1;
}

int cari_index_anggota(const char* npm_user) {
    for (int i = 0; i < jumlah_anggota; i++) {
        if (strcmp(daftar_anggota[i].npm_user, npm_user) == 0) {
            return i;
        }
    }
    return -1;
}

int hitung_pinjaman_aktif(const char* npm_user) {
    int count = 0;
    for (int i = 0; i < jumlah_peminjaman; i++) {
        if (strcmp(daftar_peminjaman[i].npm_user, npm_user) == 0 &&
            daftar_peminjaman[i].status_pengembalian == 0) {
            count++;
        }
    }
    return count;
}