#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "perpustakaan.h"

// Variabel global
Anggota daftarAnggota[MAX_ANGGOTA];
int jumlahAnggota = 0;

//load data anggota dari file
void loadAnggota() {
    FILE *file = fopen(FILE_ANGGOTA, "r"); // buka file anggota
    if (!file) return; //jika tidak ditemukan file, keluar

    jumlahAnggota = 0;
    while (fscanf(file, "%[^;];%[^;];%[^;];%[^;];%d\n",
        daftarAnggota[jumlahAnggota].npm_user,
        daftarAnggota[jumlahAnggota].nama_user,
        daftarAnggota[jumlahAnggota].alamat,
        daftarAnggota[jumlahAnggota].telepon,
        &daftarAnggota[jumlahAnggota].pinjaman_aktif
    ) != EOF)
    {
        jumlahAnggota++;
    }

    fclose(file);
}

//simpan data ke file anggota
void simpanAnggota() {
    FILE *file = fopen(FILE_ANGGOTA, "w");// buka file untuk ditulis
    if (!file) return;

    for (int i = 0; i < jumlahAnggota; i++) {
        fprintf(file, "%s;%s;%s;%s;%d\n",
            daftarAnggota[i].npm_user,
            daftarAnggota[i].nama_user,
            daftarAnggota[i].alamat,
            daftarAnggota[i].telepon,
            daftarAnggota[i].pinjaman_aktif
        );
    }

    fclose(file);
}

//tambah anggota baru
void tambahAnggota() {
    if (jumlahAnggota >= MAX_ANGGOTA) {
        printf("Tidak bisa menambah anggota, kapasitas penuh!\n");
        return;
    }

    Anggota a;

    printf("Masukkan NPM     : ");
    scanf(" %19s", a.npm_user);

    printf("Masukkan Nama    : ");
    scanf(" %[^\n]", a.nama_user);

    printf("Masukkan Alamat  : ");
    scanf(" %[^\n]", a.alamat);

    printf("Masukkan Telepon : ");
    scanf(" %14s", a.telepon);

    a.pinjaman_aktif = 0;

    daftarAnggota[jumlahAnggota++] = a;
    simpanAnggota();

    printf("Anggota berhasil ditambahkan!\n");
}

//menampilkan list anggota yang terdata
void tampilAnggota() {
    printf("\nDAFTAR ANGGOTA\n");

    for (int i = 0; i < jumlahAnggota; i++) {
        printf("%d. %s - %s (NPM: %s)\n",
            i + 1,
            daftarAnggota[i].nama_user,
            daftarAnggota[i].alamat,
            daftarAnggota[i].npm_user
        );
    }
}

//mencari anggota berdasarkan npm
int cariAnggota(char npm[]) {
    for (int i = 0; i < jumlahAnggota; i++) {
        if (strcmp(daftarAnggota[i].npm_user, npm) == 0) {
            return i;
        }
    }
    return -1;
}

//mengedit data anggota
void editAnggota() {
    char npm[20];
    printf("Masukkan NPM anggota yang ingin diedit: ");
    scanf(" %19s", npm);

    int idx = cariAnggota(npm);
    if (idx == -1) {
        printf("Anggota tidak ditemukan!\n");
        return;
    }

    printf("Nama baru     : ");
    scanf(" %[^\n]", daftarAnggota[idx].nama_user);

    printf("Alamat baru   : ");
    scanf(" %[^\n]", daftarAnggota[idx].alamat);

    printf("Telepon baru  : ");
    scanf(" %14s", daftarAnggota[idx].telepon);

    simpanAnggota();

    printf("Data anggota berhasil diperbarui!\n");
}

//menghapus data anggota
void hapusAnggota() {
    char npm[20];
    printf("NPM anggota yang ingin dihapus: ");
    scanf(" %19s", npm);

    int idx = cariAnggota(npm);
    if (idx == -1) {
        printf("Anggota tidak ditemukan!\n");
        return;
    }

    for (int i = idx; i < jumlahAnggota - 1; i++) {
        daftarAnggota[i] = daftarAnggota[i + 1];
    }

    jumlahAnggota--;
    simpanAnggota();

    printf("Anggota berhasil dihapus.\n");
}
