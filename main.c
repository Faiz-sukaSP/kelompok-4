#include "perpustakaan.h"
#include "utility.h"

// Definisi variabel global
Buku daftar_buku[MAX_BUKU];
int jumlah_buku = 0;

Anggota daftar_anggota[MAX_ANGGOTA];
int jumlah_anggota = 0;

Peminjaman daftar_peminjaman[MAX_PEMINJAMAN];
int jumlah_peminjaman = 0;

// Fungsi untuk membersihkan layar
void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Fungsi untuk menampilkan header
void tampilkan_header(const char* judul) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║  %-48s║\n", judul);
    printf("╚═══════════════════════════════════════════════════╝\n");
}

// Menu Utama
void menu_utama() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║       SISTEM MANAJEMEN PERPUSTAKAAN              ║\n");
    printf("╠═══════════════════════════════════════════════════╣\n");
    printf("║  1. Login sebagai Pustakawan                     ║\n");
    printf("║  2. Login sebagai Anggota                        ║\n");
    printf("║  3. Registrasi Anggota Baru                      ║\n");
    printf("║  0. Keluar                                       ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");
    printf("Pilihan Anda: ");
}

// Menu Pustakawan
void menu_pustakawan() {
    int pilihan;
    
    do {
        clear_screen();
        printf("\n");
        printf("╔═══════════════════════════════════════════════════╗\n");
        printf("║              MENU PUSTAKAWAN                     ║\n");
        printf("╠═══════════════════════════════════════════════════╣\n");
        printf("║  MANAJEMEN BUKU                                  ║\n");
        printf("║  1. Tampilkan Daftar Buku                        ║\n");
        printf("║  2. Tambah Buku Baru                             ║\n");
        printf("║  3. Edit Buku                                    ║\n");
        printf("║  4. Hapus Buku                                   ║\n");
        printf("║  5. Cari Buku                                    ║\n");
        printf("║  6. Ranking Buku Terpopuler                      ║\n");
        printf("║                                                  ║\n");
        printf("║  MANAJEMEN PEMINJAMAN                            ║\n");
        printf("║  7. Proses Peminjaman Buku                       ║\n");
        printf("║  8. Proses Pengembalian Buku                     ║\n");
        printf("║  9. Tampilkan Daftar Peminjaman                  ║\n");
        printf("║  10. Cari Peminjaman by NPM                      ║\n");
        printf("║                                                  ║\n");
        printf("║  0. Logout                                       ║\n");
        printf("╚═══════════════════════════════════════════════════╝\n");
        printf("Pilihan Anda: ");
        
        if (scanf("%d", &pilihan) != 1) {
            clear_input_buffer();
            printf("\n❌ Input tidak valid!\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();
        
        switch(pilihan) {
            case 1:
                clear_screen();
                tampilkan_header("DAFTAR BUKU");
                tampilkanDaftarBuku();
                pause_screen();
                break;
            case 2:
                clear_screen();
                tampilkan_header("TAMBAH BUKU BARU");
                tambahBuku();
                pause_screen();
                break;
            case 3:
                clear_screen();
                tampilkan_header("EDIT BUKU");
                editBuku();
                pause_screen();
                break;
            case 4:
                clear_screen();
                tampilkan_header("HAPUS BUKU");
                hapusBuku();
                pause_screen();
                break;
            case 5:
                clear_screen();
                tampilkan_header("CARI BUKU");
                cariBuku();
                pause_screen();
                break;
            case 6:
                clear_screen();
                tampilkan_header("RANKING BUKU TERPOPULER");
                tampilkanRankingBuku();
                pause_screen();
                break;
            case 7:
                clear_screen();
                tampilkan_header("PROSES PEMINJAMAN BUKU");
                pinjamBuku();
                pause_screen();
                break;
            case 8:
                clear_screen();
                tampilkan_header("PROSES PENGEMBALIAN BUKU");
                kembalikanBuku();
                pause_screen();
                break;
            case 9:
                clear_screen();
                tampilkan_header("DAFTAR PEMINJAMAN");
                tampilkanDaftarPeminjaman();
                pause_screen();
                break;
            case 10:
                clear_screen();
                tampilkan_header("CARI PEMINJAMAN BY NPM");
                cariPeminjamanByNPM();
                pause_screen();
                break;
            case 0:
                printf("\n✅ Logout berhasil!\n");
                pause_screen();
                break;
            default:
                printf("\n❌ Pilihan tidak valid!\n");
                pause_screen();
        }
    } while(pilihan != 0);
}

// Menu Anggota
void menu_anggota(const char* npm_user) {
    int pilihan;
    int index = cari_index_anggota(npm_user);
    
    if (index < 0) {
        printf("\n❌ Data anggota tidak ditemukan!\n");
        return;
    }
    
    do {
        clear_screen();
        printf("\n");
        printf("╔═══════════════════════════════════════════════════╗\n");
        printf("║              MENU ANGGOTA                        ║\n");
        printf("╠═══════════════════════════════════════════════════╣\n");
        printf("║  Nama  : %-40s║\n", daftar_anggota[index].nama_user);
        printf("║  NPM   : %-40s║\n", npm_user);
        printf("║  Pinjaman Aktif: %-3d                            ║\n", 
               hitung_pinjaman_aktif(npm_user));
        printf("╠═══════════════════════════════════════════════════╣\n");
        printf("║  1. Lihat Daftar Buku Tersedia                   ║\n");
        printf("║  2. Cari Buku                                    ║\n");
        printf("║  3. Lihat Riwayat Peminjaman Saya                ║\n");
        printf("║  4. Lihat Buku yang Sedang Dipinjam              ║\n");
        printf("║  0. Logout                                       ║\n");
        printf("╚═══════════════════════════════════════════════════╝\n");
        printf("Pilihan Anda: ");
        
        if (scanf("%d", &pilihan) != 1) {
            clear_input_buffer();
            printf("\n❌ Input tidak valid!\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();
        
        switch(pilihan) {
            case 1:
                clear_screen();
                tampilkan_header("DAFTAR BUKU TERSEDIA");
                tampilkanDaftarBuku();
                pause_screen();
                break;
                
            case 2:
                clear_screen();
                tampilkan_header("CARI BUKU");
                cariBuku();
                pause_screen();
                break;
                
            case 3:
                clear_screen();
                tampilkan_header("RIWAYAT PEMINJAMAN");
                printf("\n");
                int found = 0;
                for (int i = 0; i < jumlah_peminjaman; i++) {
                    if (strcmp(daftar_peminjaman[i].npm_user, npm_user) == 0) {
                        printf("─────────────────────────────────────────────────\n");
                        printf("ID Peminjaman     : %s\n", daftar_peminjaman[i].id_peminjaman);
                        printf("ID Buku           : %s\n", daftar_peminjaman[i].id_buku);
                        
                        // Tampilkan judul buku
                        int idx_buku = cari_index_buku(daftar_peminjaman[i].id_buku);
                        if (idx_buku >= 0) {
                            printf("Judul Buku        : %s\n", daftar_buku[idx_buku].judul_buku);
                        }
                        
                        printf("Tanggal Pinjam    : %s\n", daftar_peminjaman[i].tanggal_pinjam);
                        printf("Tanggal Kembali   : %s\n", daftar_peminjaman[i].tanggal_pengembalian);
                        printf("Status            : %s\n", 
                               daftar_peminjaman[i].status_pengembalian ? "✅ Sudah Dikembalikan" : "⏳ Masih Dipinjam");
                        
                        if (daftar_peminjaman[i].denda > 0) {
                            printf("Denda             : Rp %d\n", daftar_peminjaman[i].denda);
                        }
                        found = 1;
                    }
                }
                
                if (!found) {
                    printf("\n📭 Anda belum memiliki riwayat peminjaman.\n");
                }
                printf("\n");
                pause_screen();
                break;
                
            case 4:
                clear_screen();
                tampilkan_header("BUKU YANG SEDANG DIPINJAM");
                printf("\n");
                int dipinjam = 0;
                for (int i = 0; i < jumlah_peminjaman; i++) {
                    if (strcmp(daftar_peminjaman[i].npm_user, npm_user) == 0 &&
                        daftar_peminjaman[i].status_pengembalian == 0) {
                        
                        printf("─────────────────────────────────────────────────\n");
                        printf("ID Peminjaman     : %s\n", daftar_peminjaman[i].id_peminjaman);
                        printf("ID Buku           : %s\n", daftar_peminjaman[i].id_buku);
                        
                        int idx_buku = cari_index_buku(daftar_peminjaman[i].id_buku);
                        if (idx_buku >= 0) {
                            printf("Judul Buku        : %s\n", daftar_buku[idx_buku].judul_buku);
                            printf("Penulis           : %s\n", daftar_buku[idx_buku].penulis);
                        }
                        
                        printf("Tanggal Pinjam    : %s\n", daftar_peminjaman[i].tanggal_pinjam);
                        printf("Tanggal Kembali   : %s\n", daftar_peminjaman[i].tanggal_pengembalian);
                        
                        // Hitung sisa hari atau keterlambatan
                        char tanggal_sekarang[20];
                        get_tanggal_sekarang(tanggal_sekarang);
                        int selisih = hitung_selisih_hari(daftar_peminjaman[i].tanggal_pengembalian, tanggal_sekarang);
                        
                        if (selisih > 0) {
                            printf("Status            : ⚠️  TERLAMBAT %d hari\n", selisih);
                            printf("Denda             : Rp %d\n", selisih * DENDA_PER_HARI);
                        } else {
                            printf("Status            : ✅ Masih dalam masa pinjam (%d hari lagi)\n", -selisih);
                        }
                        
                        dipinjam = 1;
                    }
                }
                
                if (!dipinjam) {
                    printf("\n📭 Anda tidak memiliki buku yang sedang dipinjam.\n");
                }
                printf("\n");
                pause_screen();
                break;
                
            case 0:
                printf("\n✅ Logout berhasil!\n");
                pause_screen();
                break;
                
            default:
                printf("\n❌ Pilihan tidak valid!\n");
                pause_screen();
        }
    } while(pilihan != 0);
}

// Fungsi login pustakawan sederhana
int login_pustakawan() {
    char username[50], password[50];
    
    clear_screen();
    tampilkan_header("LOGIN PUSTAKAWAN");
    
    printf("\nUsername: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    
    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;
    
    // Login sederhana (hardcoded untuk demo)
    // Anda bisa integrasikan dengan fungsi login yang ada
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        printf("\n✅ Login berhasil! Selamat datang, %s\n", username);
        pause_screen();
        return 1;
    } else {
        printf("\n❌ Login gagal! Username atau password salah.\n");
        pause_screen();
        return 0;
    }
}

// Fungsi login anggota
int login_anggota(char* npm_user) {
    clear_screen();
    tampilkan_header("LOGIN ANGGOTA");
    
    printf("\nNPM: ");
    fgets(npm_user, 20, stdin);
    npm_user[strcspn(npm_user, "\n")] = 0;
    
    // Validasi NPM ada di database
    int index = cari_index_anggota(npm_user);
    if (index >= 0) {
        printf("\n✅ Login berhasil! Selamat datang, %s\n", daftar_anggota[index].nama_user);
        pause_screen();
        return 1;
    } else {
        printf("\n❌ Login gagal! NPM tidak ditemukan.\n");
        printf("💡 Tip: Gunakan menu Registrasi untuk mendaftar sebagai anggota baru.\n");
        pause_screen();
        return 0;
    }
}

// Fungsi registrasi anggota baru
void registrasi_anggota() {
    if (jumlah_anggota >= MAX_ANGGOTA) {
        printf("\n❌ Database anggota sudah penuh!\n");
        return;
    }
    
    Anggota anggota_baru;
    
    clear_screen();
    tampilkan_header("REGISTRASI ANGGOTA BARU");
    
    printf("\nNPM: ");
    fgets(anggota_baru.npm_user, sizeof(anggota_baru.npm_user), stdin);
    anggota_baru.npm_user[strcspn(anggota_baru.npm_user, "\n")] = 0;
    
    // Cek apakah NPM sudah terdaftar
    if (cari_index_anggota(anggota_baru.npm_user) >= 0) {
        printf("\n❌ NPM sudah terdaftar!\n");
        return;
    }
    
    printf("Nama Lengkap: ");
    fgets(anggota_baru.nama_user, sizeof(anggota_baru.nama_user), stdin);
    anggota_baru.nama_user[strcspn(anggota_baru.nama_user, "\n")] = 0;
    
    printf("Alamat: ");
    fgets(anggota_baru.alamat, sizeof(anggota_baru.alamat), stdin);
    anggota_baru.alamat[strcspn(anggota_baru.alamat, "\n")] = 0;
    
    printf("No. Telepon: ");
    fgets(anggota_baru.telepon, sizeof(anggota_baru.telepon), stdin);
    anggota_baru.telepon[strcspn(anggota_baru.telepon, "\n")] = 0;
    
    anggota_baru.pinjaman_aktif = 0;
    
    // Simpan ke array
    daftar_anggota[jumlah_anggota] = anggota_baru;
    jumlah_anggota++;
    
    printf("\n✅ Registrasi berhasil! Sekarang Anda bisa login dengan NPM Anda.\n");
}

// Fungsi untuk load semua data dari file
void load_semua_data() {
    printf("⏳ Memuat data dari file...\n");
    
    muatDataBuku(daftar_buku, &jumlah_buku);
    loadPeminjamanDariFile(daftar_peminjaman, &jumlah_peminjaman);
    
    printf("✅ Data berhasil dimuat!\n");
    printf("   - Buku: %d\n", jumlah_buku);
    printf("   - Anggota: %d\n", jumlah_anggota);
    printf("   - Peminjaman: %d\n", jumlah_peminjaman);
}

// Fungsi untuk save semua data ke file
void save_semua_data() {
    printf("\n⏳ Menyimpan data ke file...\n");
    
    simpanDataBuku(daftar_buku, jumlah_buku);
    
    printf("✅ Data berhasil disimpan!\n");
}

// FUNGSI MAIN
int main() {
    int pilihan;
    char npm_user[20];
    
    // Load data dari file saat program dimulai
    load_semua_data();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║                                                  ║\n");
    printf("║        SELAMAT DATANG DI PERPUSTAKAAN            ║\n");
    printf("║          UNIVERSITAS INDONESIA                   ║\n");
    printf("║                                                  ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");
    pause_screen();
    
    do {
        clear_screen();
        menu_utama();
        
        if (scanf("%d", &pilihan) != 1) {
            clear_input_buffer();
            printf("\n❌ Input tidak valid!\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();
        
        switch(pilihan) {
            case 1:
                if (login_pustakawan()) {
                    menu_pustakawan();
                }
                break;
                
            case 2:
                if (login_anggota(npm_user)) {
                    menu_anggota(npm_user);
                }
                break;
                
            case 3:
                registrasi_anggota();
                pause_screen();
                break;
                
            case 0:
                clear_screen();
                printf("\n");
                printf("╔═══════════════════════════════════════════════════╗\n");
                printf("║                                                  ║\n");
                printf("║   Terima kasih telah menggunakan sistem ini!     ║\n");
                printf("║              Sampai jumpa lagi!                  ║\n");
                printf("║                                                  ║\n");
                printf("╚═══════════════════════════════════════════════════╝\n");
                printf("\n");
                
                // Save data sebelum keluar
                save_semua_data();
                break;
                
            default:
                printf("\n❌ Pilihan tidak valid! Silakan pilih 0-3.\n");
                pause_screen();
        }
    } while(pilihan != 0);
    
    return 0;
}