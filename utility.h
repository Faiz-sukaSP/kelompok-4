#ifndef UTILITY_H
#define UTILITY_H

#include "perpustakaan.h"

// Deklarasi fungsi utility
void clear_input_buffer(void);
void pause_screen(void);
const char* kategori_string(Kategori k);
const char* status_string(Status s);
void get_tanggal_sekarang(char* buffer);
void tambah_hari(const char* tanggal, int hari, char* hasil);
int hitung_selisih_hari(const char* tanggal1, const char* tanggal2);
int cari_index_buku(const char* id_buku);
int cari_index_anggota(const char* npm_user);
int hitung_pinjaman_aktif(const char* npm_user);

#endif