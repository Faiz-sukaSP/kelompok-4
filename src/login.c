#include "perpustakaan.h"


pustakawan adminList[MAX_PUSTAKAWAN];
int jumlahAdmin = 0;

// Membuka file 
// Format file pustakawan.txt:
// id;username;password
void loadAdmin() {
    FILE *file = fopen(FILE_PUSTAKAWAN, "r");// buka file 
    if (!file) return; // jika file tidak ada, keluar fungsi

    jumlahAdmin = 0;
    // baca tiap baris 
   while (fscanf(file, "%[^;];%[^;];%[^;];%[^;];%[^\n]\n",
    adminList[jumlahAdmin].id_pustakawan,
    adminList[jumlahAdmin].pustakawan,
    adminList[jumlahAdmin].username,
    adminList[jumlahAdmin].password,
    adminList[jumlahAdmin].waktu_login
) != EOF) {
    jumlahAdmin++;
}

    fclose(file); // tutup file
}

//Login Pustakawan

int loginPustakawan() {
    char user[20], pass[20];

    printf("\n=== LOGIN PUSTAKAWAN ===\n");
    printf("Username : ");
    scanf(" %19s", user); // baca username max 19 karakter

    printf("Password : ");
    scanf(" %19s", pass); // baca password max 19 karakter

    // cek username & password
    for (int i = 0; i < jumlahAdmin; i++) {
        if (strcmp(adminList[i].username, user) == 0 &&
            strcmp(adminList[i].password, pass) == 0) 
        {
            printf("Login berhasil! Selamat datang, %s.\n", adminList[i].pustakawan); //jika login berhasil
            return 1; 
        }
    }

    printf("Login gagal! Username atau password salah.\n"); //jika login gagal
    return 0; 
}

//cek apakah username sudah ada apa belum
int cekUsername(char user[]) {
    for (int i = 0; i < jumlahAdmin; i++) {
        if (strcmp(adminList[i].username, user) == 0) {
            return 1; // username sudah ada
        }
    }
    return 0; // username belum terdaftar
}

