#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX 1000
#define MAX_BORROW 3        // maksimal pinjaman per anggota
#define MAX_DAYS 7          // hari tanpa denda
#define DENDA_PER_HARI 1000 // rupiah per hari keterlambatan

/* ---------- Structs ---------- */
typedef struct {
    char id[10];
    char judul[120];
    char penulis[80];
    char genre[40];
    char rilis[16]; // DD-MM-YYYY
    int status; // 0 tersedia, 1 dipinjam
    int count;  // jumlah dipinjam (ranking)
} Buku;

typedef struct {
    char id[10];
    char username[40];
    char password[40];
} Anggota;

typedef struct {
    char idPin[32];
    char idBuku[10];
    char idAng[10];
    int tgl, bln, thn;
} Peminjaman;

/* ---------- Utility ---------- */
void trim_newline(char *s) {
    char *p = strchr(s, '\n');
    if (p) *p = '\0';
}

/* case-insensitive search helpers */
int strncasecmp_custom(const char *s1, const char *s2, size_t n) {
    while (n--) {
        unsigned char a = (unsigned char) tolower(*s1++);
        unsigned char b = (unsigned char) tolower(*s2++);
        if (a != b) return (int)a - (int)b;
        if (a == '\0') return 0;
    }
    return 0;
}
char *strcasestr_custom(const char *haystack, const char *needle) {
    size_t len = strlen(needle);
    if (!len) return (char *)haystack;
    for (; *haystack; haystack++) {
        if (tolower((unsigned char)*haystack) == tolower((unsigned char)*needle)) {
            if (strncasecmp_custom(haystack, needle, len) == 0)
                return (char *)haystack;
        }
    }
    return NULL;
}

/* ensure files exist */
void ensure_files_exist() {
    FILE *f;
    f = fopen("buku.txt", "a"); if (f) fclose(f);
    f = fopen("anggota.txt", "a"); if (f) fclose(f);
    f = fopen("peminjaman.txt", "a"); if (f) fclose(f);
    f = fopen("history.txt", "a"); if (f) fclose(f);
}

/* ---------- ID generation ---------- */
void generate_next_book_id(char *outId) {
    FILE *f = fopen("buku.txt", "r");
    if (!f) { strcpy(outId, "B001"); return; }
    char line[512];
    int max = 0;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        char id[16];
        if (sscanf(line, "%15[^|]|", id) == 1) {
            if (id[0] == 'B') {
                int num = atoi(id+1);
                if (num > max) max = num;
            }
        }
    }
    fclose(f);
    sprintf(outId, "B%03d", max + 1);
}

/* ---------- Count active loans for anggota ---------- */
int count_active_loans(const char *idAng) {
    FILE *f = fopen("peminjaman.txt", "r");
    if (!f) return 0;
    char line[256];
    int cnt = 0;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        char idP[32], idB[16], idA[16]; int t,m,y;
        if (sscanf(line, "%31[^|]|%15[^|]|%15[^|]|%d|%d|%d", idP,idB,idA,&t,&m,&y) == 6) {
            if (strcmp(idA, idAng) == 0) cnt++;
        }
    }
    fclose(f);
    return cnt;
}

/* ---------- Time helpers ---------- */
/* convert date to time_t (midnight local) */
time_t date_to_time(int d, int m, int y) {
    struct tm tm0;
    memset(&tm0, 0, sizeof(tm0));
    tm0.tm_mday = d;
    tm0.tm_mon = m - 1;
    tm0.tm_year = y - 1900;
    tm0.tm_hour = 12; // midday to avoid daylight issues
    return mktime(&tm0);
}

/* ---------- Basic IO safe input ---------- */
void input_line(const char *prompt, char *buf, int bufsize) {
    printf("%s", prompt);
    if (fgets(buf, bufsize, stdin) == NULL) { buf[0] = '\0'; return; }
    trim_newline(buf);
}

/* ---------- Prototypes ---------- */
void main_menu();
void register_member();
int login_system(char *outType, char *outId); // outType: "admin" or "anggota"
void menu_admin();
void menu_anggota(const char *idAng);

void admin_add_books();
void admin_edit_book();
void admin_delete_book();
void view_books();
void view_category();
void search_books();

void borrow_book(const char *idAng);
void return_book(const char *idAng);
void view_history_active(const char *idAng);
void view_history_all(const char *idAng); // optional detailed history from history.txt
void ranking_books();

/* ---------- Implementation ---------- */

int main() {
    ensure_files_exist();
    printf("=== SISTEM PERPUSTAKAAN MINI ===\n");
    main_menu();
    return 0;
}

void main_menu() {
    int choice;
    char type[16], idAng[16];
    do {
        printf("\n1. Login\n2. Registrasi Anggota\n0. Keluar\nPilihan: ");
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n'); // clear newline
        if (choice == 1) {
            int res = login_system(type, idAng);
            if (res == 1) {
                printf("Login admin berhasil.\n");
                menu_admin();
            } else if (res == 2) {
                printf("Login anggota berhasil. ID: %s\n", idAng);
                menu_anggota(idAng);
            } else {
                printf("Login gagal.\n");
            }
        } else if (choice == 2) {
            register_member();
        } else if (choice == 0) {
            printf("Keluar program.\n");
        } else {
            printf("Pilihan tidak valid.\n");
        }
    } while (choice != 0);
}

/* ---------- Register anggota: auto-generate ID A001... and store id|username|password ---------- */
void register_member() {
    char username[40], password[40];
    input_line("\nUsername baru: ", username, sizeof(username));
    input_line("Password baru: ", password, sizeof(password));

    // check unique username
    FILE *f = fopen("anggota.txt", "r");
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            trim_newline(line);
            if (strlen(line) == 0) continue;
            char id[16], user[40], pass[40];
            if (sscanf(line, "%15[^|]|%39[^|]|%39s", id, user, pass) >= 2) {
                if (strcmp(user, username) == 0) {
                    printf("Username sudah terpakai. Registrasi dibatalkan.\n");
                    fclose(f);
                    return;
                }
            }
        }
        fclose(f);
    }

    // generate next ID
    f = fopen("anggota.txt", "r");
    int max = 0;
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            trim_newline(line);
            if (strlen(line) == 0) continue;
            char id[16];
            if (sscanf(line, "%15[^|]|", id) == 1) {
                if (id[0] == 'A') {
                    int num = atoi(id+1);
                    if (num > max) max = num;
                }
            }
        }
        fclose(f);
    }
    char nextId[16];
    sprintf(nextId, "A%03d", max + 1);

    FILE *fw = fopen("anggota.txt", "a");
    if (!fw) { printf("Gagal membuka file anggota.\n"); return; }
    fprintf(fw, "%s|%s|%s\n", nextId, username, password);
    fclose(fw);
    printf("Registrasi sukses! ID anggota: %s\n", nextId);
}

/* ---------- Login: admin (admin/admin123) or anggota (search file). Return 1=admin,2=anggota,0=fail.
   If anggota, outId filled with anggota ID.
*/
int login_system(char *outType, char *outId) {
    char user[40], pass[40];
    input_line("\nUsername: ", user, sizeof(user));
    input_line("Password: ", pass, sizeof(pass));

    if (strcmp(user, "admin") == 0 && strcmp(pass, "admin123") == 0) {
        strcpy(outType, "admin");
        return 1;
    }

    FILE *f = fopen("anggota.txt", "r");
    if (!f) return 0;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        char id[16], uname[40], pwd[40];
        if (sscanf(line, "%15[^|]|%39[^|]|%39s", id, uname, pwd) == 3) {
            if (strcmp(uname, user) == 0 && strcmp(pwd, pass) == 0) {
                strcpy(outType, "anggota");
                strcpy(outId, id);
                fclose(f);
                return 2;
            }
        }
    }
    fclose(f);
    return 0;
}

/* ================= Admin Menu ================= */
void menu_admin() {
    int pilih;
    do {
        printf("\n=== MENU ADMIN ===\n");
        printf("1. Tambah Buku (banyak)\n2. Edit Buku\n3. Hapus Buku\n4. Lihat Buku\n5. Kategori\n6. Cari Buku\n7. Ranking Buku\n0. Logout\nPilihan: ");
        if (scanf("%d", &pilih) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');
        switch (pilih) {
            case 1: admin_add_books(); break;
            case 2: admin_edit_book(); break;
            case 3: admin_delete_book(); break;
            case 4: view_books(); break;
            case 5: view_category(); break;
            case 6: search_books(); break;
            case 7: ranking_books(); break;
            case 0: printf("Logout admin.\n"); break;
            default: printf("Pilihan salah.\n");
        }
    } while (pilih != 0);
}

/* ================= Anggota Menu ================= */
void menu_anggota(const char *idAng) {
    int pilih;
    do {
        printf("\n=== MENU ANGGOTA (%s) ===\n", idAng);
        printf("1. Lihat Buku\n2. Kategori\n3. Cari Buku\n4. Pinjam Buku\n5. Kembalikan Buku\n6. Riwayat Pinjam (aktif)\n7. Riwayat Lengkap (arsip)\n0. Logout\nPilihan: ");
        if (scanf("%d", &pilih) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');
        switch (pilih) {
            case 1: view_books(); break;
            case 2: view_category(); break;
            case 3: search_books(); break;
            case 4: borrow_book(idAng); break;
            case 5: return_book(idAng); break;
            case 6: view_history_active(idAng); break;
            case 7: view_history_all(idAng); break;
            case 0: printf("Logout anggota.\n"); break;
            default: printf("Pilihan salah.\n");
        }
    } while (pilih != 0);
}

/* ---------- Admin: add multiple books with auto ID ---------- */
void admin_add_books() {
    int n;
    char tmp[256];
    input_line("\nBerapa buku ingin ditambahkan? ", tmp, sizeof(tmp));
    n = atoi(tmp);
    for (int i = 0; i < n; ++i) {
        Buku b;
        generate_next_book_id(b.id);
        printf("\n-- Buku #%d (ID otomatis: %s) --\n", i+1, b.id);
        input_line("Judul: ", b.judul, sizeof(b.judul));
        input_line("Penulis: ", b.penulis, sizeof(b.penulis));
        input_line("Genre (Fiksi/Non-Fiksi/Komik/Majalah/Referensi): ", b.genre, sizeof(b.genre));
        input_line("Tanggal rilis (DD-MM-YYYY): ", b.rilis, sizeof(b.rilis));
        b.status = 0; b.count = 0;
        FILE *f = fopen("buku.txt", "a");
        if (!f) { printf("Gagal buka buku.txt\n"); return; }
        fprintf(f, "%s|%s|%s|%s|%s|%d|%d\n", b.id, b.judul, b.penulis, b.genre, b.rilis, b.status, b.count);
        fclose(f);
        printf("Buku %s (ID %s) berhasil ditambahkan.\n", b.judul, b.id);
    }
}

/* ---------- Admin: edit book by ID ---------- */
void admin_edit_book() {
    char id[16];
    input_line("\nMasukkan ID buku yang ingin diedit: ", id, sizeof(id));
    FILE *f = fopen("buku.txt","r");
    FILE *tmp = fopen("tmp_buku.txt","w");
    if (!f || !tmp) { if (f) fclose(f); if (tmp) fclose(tmp); printf("File error.\n"); return; }
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        Buku b;
        if (sscanf(line, "%9[^|]|%119[^|]|%79[^|]|%39[^|]|%15[^|]|%d|%d",
            b.id,b.judul,b.penulis,b.genre,b.rilis,&b.status,&b.count) >= 7) {
            if (strcmp(b.id, id) == 0) {
                found = 1;
                printf("\nJudul baru (enter = tidak berubah): "); {
                    char s[200]; fgets(s, sizeof(s), stdin); trim_newline(s);
                    if (strlen(s) > 0) strncpy(b.judul, s, sizeof(b.judul));
                }
                printf("Penulis baru (enter = tidak berubah): "); {
                    char s[200]; fgets(s, sizeof(s), stdin); trim_newline(s);
                    if (strlen(s) > 0) strncpy(b.penulis, s, sizeof(b.penulis));
                }
                printf("Genre baru (enter = tidak berubah): "); {
                    char s[200]; fgets(s, sizeof(s), stdin); trim_newline(s);
                    if (strlen(s) > 0) strncpy(b.genre, s, sizeof(b.genre));
                }
                printf("Rilis baru (DD-MM-YYYY) (enter = tidak berubah): "); {
                    char s[200]; fgets(s, sizeof(s), stdin); trim_newline(s);
                    if (strlen(s) > 0) strncpy(b.rilis, s, sizeof(b.rilis));
                }
            }
            fprintf(tmp, "%s|%s|%s|%s|%s|%d|%d\n", b.id,b.judul,b.penulis,b.genre,b.rilis,b.status,b.count);
        } else {
            fprintf(tmp, "%s\n", line);
        }
    }
    fclose(f); fclose(tmp);
    remove("buku.txt"); rename("tmp_buku.txt", "buku.txt");
    if (found) printf("Update sukses.\n"); else printf("ID buku tidak ditemukan.\n");
}

/* ---------- Admin: delete book by ID ---------- */
void admin_delete_book() {
    char id[16];
    input_line("\nMasukkan ID buku yang akan dihapus: ", id, sizeof(id));
    FILE *f = fopen("buku.txt","r");
    FILE *tmp = fopen("tmp_buku.txt","w");
    if (!f || !tmp) { if (f) fclose(f); if (tmp) fclose(tmp); printf("File error.\n"); return; }
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        char idb[16];
        if (sscanf(line, "%15[^|]|", idb) == 1) {
            if (strcmp(idb, id) == 0) { found = 1; continue; }
        }
        fprintf(tmp, "%s\n", line);
    }
    fclose(f); fclose(tmp);
    remove("buku.txt"); rename("tmp_buku.txt", "buku.txt");
    if (found) printf("Buku %s dihapus.\n", id); else printf("ID buku tidak ditemukan.\n");
}

/* ---------- View all books ---------- */
void view_books() {
    FILE *f = fopen("buku.txt","r");
    if (!f) { printf("Tidak ada data buku.\n"); return; }
    printf("\n=== DAFTAR BUKU ===\n");
    char line[1024];
    int i = 1;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        Buku b;
        if (sscanf(line, "%9[^|]|%119[^|]|%79[^|]|%39[^|]|%15[^|]|%d|%d",
            b.id,b.judul,b.penulis,b.genre,b.rilis,&b.status,&b.count) >= 7) {
            printf("\n%d. %s (ID: %s)\n   Penulis: %s\n   Genre: %s\n   Rilis: %s\n   Status: %s\n   Dipinjam: %d kali\n",
                i++, b.judul, b.id, b.penulis, b.genre, b.rilis, b.status? "Dipinjam":"Tersedia", b.count);
        }
    }
    fclose(f);
}

/* ---------- View books by category ---------- */
void view_category() {
    char key[64];
    input_line("\nMasukkan kategori (Fiksi/Non-Fiksi/Komik/Majalah/Referensi): ", key, sizeof(key));
    FILE *f = fopen("buku.txt","r");
    if (!f) { printf("Tidak ada data buku.\n"); return; }
    char line[1024];
    int found = 0;
    printf("\nBuku kategori '%s':\n", key);
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        Buku b;
        if (sscanf(line, "%9[^|]|%119[^|]|%79[^|]|%39[^|]|%15[^|]|%d|%d",
            b.id,b.judul,b.penulis,b.genre,b.rilis,&b.status,&b.count) >= 7) {
            if (strncasecmp_custom(b.genre, key, strlen(key)) == 0) {
                printf("%s (ID: %s)\n", b.judul, b.id);
                found = 1;
            }
        }
    }
    if (!found) printf("Tidak ada buku di kategori tersebut.\n");
    fclose(f);
}

/* ---------- Search books (case-insensitive in judul/penulis/genre) ---------- */
void search_books() {
    char key[128];
    input_line("\nMasukkan kata kunci (judul/penulis/genre): ", key, sizeof(key));
    FILE *f = fopen("buku.txt","r");
    if (!f) { printf("Tidak ada data buku.\n"); return; }
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        Buku b;
        if (sscanf(line, "%9[^|]|%119[^|]|%79[^|]|%39[^|]|%15[^|]|%d|%d",
            b.id,b.judul,b.penulis,b.genre,b.rilis,&b.status,&b.count) >= 7) {
            if (strcasestr_custom(b.judul, key) || strcasestr_custom(b.penulis, key) || strcasestr_custom(b.genre, key)) {
                printf("\n%s (ID: %s)\n Penulis: %s\n Genre: %s\n Rilis: %s\n Status: %s\n Dipinjam: %d kali\n",
                    b.judul, b.id, b.penulis, b.genre, b.rilis, b.status? "Dipinjam":"Tersedia", b.count);
                found = 1;
            }
        }
    }
    if (!found) printf("Tidak ditemukan.\n");
    fclose(f);
}

/* ---------- Borrow book: check limit, set status, record peminjaman with date ---------- */
void borrow_book(const char *idAng) {
    char idBuku[16];
    input_line("\nMasukkan ID Buku untuk dipinjam: ", idBuku, sizeof(idBuku));

    int current = count_active_loans(idAng);
    if (current >= MAX_BORROW) { printf("Anda sudah meminjam %d buku (maks %d). Kembalikan dulu.\n", current, MAX_BORROW); return; }

    FILE *fb = fopen("buku.txt","r");
    FILE *tmp = fopen("tmp_buku.txt","w");
    if (!fb || !tmp) { if (fb) fclose(fb); if (tmp) fclose(tmp); printf("File error.\n"); return; }

    char line[1024]; int found = 0;
    Buku bsel;
    while (fgets(line, sizeof(line), fb)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        Buku b;
        if (sscanf(line, "%9[^|]|%119[^|]|%79[^|]|%39[^|]|%15[^|]|%d|%d",
            b.id,b.judul,b.penulis,b.genre,b.rilis,&b.status,&b.count) >= 7) {
            if (strcmp(b.id, idBuku) == 0) {
                found = 1;
                if (b.status == 1) {
                    printf("Maaf, buku sedang dipinjam.\n");
                    fprintf(tmp, "%s|%s|%s|%s|%s|%d|%d\n", b.id,b.judul,b.penulis,b.genre,b.rilis,b.status,b.count);
                } else {
                    // lakukan pinjam
                    b.status = 1; b.count++;
                    strcpy(bsel.id, b.id); strcpy(bsel.judul, b.judul);
                    fprintf(tmp, "%s|%s|%s|%s|%s|%d|%d\n", b.id,b.judul,b.penulis,b.genre,b.rilis,b.status,b.count);

                    // tulis peminjaman
                    FILE *fp = fopen("peminjaman.txt", "a");
                    if (fp) {
                        Peminjaman p;
                        sprintf(p.idPin, "PJ%ld", time(NULL));
                        strcpy(p.idBuku, b.id);
                        strcpy(p.idAng, idAng);
                        time_t t = time(NULL);
                        struct tm *tm = localtime(&t);
                        p.tgl = tm->tm_mday; p.bln = tm->tm_mon+1; p.thn = tm->tm_year+1900;
                        fprintf(fp, "%s|%s|%s|%d|%d|%d\n", p.idPin, p.idBuku, p.idAng, p.tgl, p.bln, p.thn);
                        fclose(fp);
                        printf("Berhasil meminjam '%s'. ID Pinjam: %s\n", b.judul, p.idPin);
                    } else {
                        printf("Gagal menyimpan data peminjaman.\n");
                    }
                }
            } else {
                fprintf(tmp, "%s|%s|%s|%s|%s|%d|%d\n", b.id,b.judul,b.penulis,b.genre,b.rilis,b.status,b.count);
            }
        } else {
            fprintf(tmp, "%s\n", line);
        }
    }
    fclose(fb); fclose(tmp);
    remove("buku.txt"); rename("tmp_buku.txt","buku.txt");
    if (!found) printf("ID buku tidak ditemukan.\n");
}

/* ---------- Return book: find peminjaman by idPin (and idAng), compute denda, update buku status, archive to history ---------- */
void return_book(const char *idAng) {
    char idPin[32];
    input_line("\nMasukkan ID Pinjam (contoh PJ...): ", idPin, sizeof(idPin));

    FILE *fp = fopen("peminjaman.txt","r");
    FILE *tmp = fopen("tmp_peminjaman.txt","w");
    if (!fp || !tmp) { if (fp) fclose(fp); if (tmp) fclose(tmp); printf("File error.\n"); return; }

    char line[256];
    int found = 0;
    Peminjaman record;
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        char idP[32], idB[16], idA[16]; int d,m,y;
        if (sscanf(line, "%31[^|]|%15[^|]|%15[^|]|%d|%d|%d", idP,idB,idA,&d,&m,&y) == 6) {
            if (strcmp(idP, idPin) == 0 && strcmp(idA, idAng) == 0) {
                found = 1;
                strcpy(record.idPin, idP); strcpy(record.idBuku, idB); strcpy(record.idAng, idA);
                record.tgl = d; record.bln = m; record.thn = y;
                // skip writing -> remove from peminjaman
            } else {
                fprintf(tmp, "%s\n", line);
            }
        } else {
            fprintf(tmp, "%s\n", line);
        }
    }
    fclose(fp); fclose(tmp);
    remove("peminjaman.txt"); rename("tmp_peminjaman.txt","peminjaman.txt");

    if (!found) { printf("ID Pinjam tidak ditemukan atau bukan milik Anda.\n"); return; }

    // hitung denda
    time_t tnow = time(NULL);
    time_t tpin = date_to_time(record.tgl, record.bln, record.thn);
    double diff_sec = difftime(tnow, tpin);
    int days = (int)(diff_sec / (60*60*24));
    if (days < 0) days = 0;
    int late = (days > MAX_DAYS) ? (days - MAX_DAYS) : 0;
    int denda = late * DENDA_PER_HARI;

    // update buku status -> set dipinjam = 0
    FILE *fb = fopen("buku.txt","r");
    FILE *tmpb = fopen("tmp_buku.txt","w");
    if (!fb || !tmpb) { if (fb) fclose(fb); if (tmpb) fclose(tmpb); printf("File buku error.\n"); return; }
    char l2[1024];
    while (fgets(l2, sizeof(l2), fb)) {
        trim_newline(l2);
        if (strlen(l2) == 0) continue;
        Buku b;
        if (sscanf(l2, "%9[^|]|%119[^|]|%79[^|]|%39[^|]|%15[^|]|%d|%d",
            b.id,b.judul,b.penulis,b.genre,b.rilis,&b.status,&b.count) >= 7) {
            if (strcmp(b.id, record.idBuku) == 0) {
                b.status = 0;
            }
            fprintf(tmpb, "%s|%s|%s|%s|%s|%d|%d\n", b.id,b.judul,b.penulis,b.genre,b.rilis,b.status,b.count);
        } else {
            fprintf(tmpb, "%s\n", l2);
        }
    }
    fclose(fb); fclose(tmpb);
    remove("buku.txt"); rename("tmp_buku.txt","buku.txt");

    // tulis ke history.txt
    FILE *fh = fopen("history.txt","a");
    if (fh) {
        time_t tnow2 = time(NULL);
        struct tm *tmnow = localtime(&tnow2);
        fprintf(fh, "%s|%s|%s|%d|%d|%d|%d|%d|%d|%d\n",
            record.idPin, record.idBuku, record.idAng,
            record.tgl, record.bln, record.thn,
            tmnow->tm_mday, tmnow->tm_mon+1, tmnow->tm_year+1900,
            denda);
        fclose(fh);
    }

    printf("Pengembalian berhasil. Lama pinjam: %d hari. Keterlambatan: %d hari. Denda: Rp %d\n", days, late, denda);
}

/* ---------- View active peminjaman for anggota ---------- */
void view_history_active(const char *idAng) {
    FILE *fp = fopen("peminjaman.txt","r");
    if (!fp) { printf("Tidak ada peminjaman aktif.\n"); return; }
    char line[256];
    int found = 0;
    printf("\n=== Peminjaman aktif untuk %s ===\n", idAng);
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        char idP[32], idB[16], idA[16]; int d,m,y;
        if (sscanf(line, "%31[^|]|%15[^|]|%15[^|]|%d|%d|%d", idP,idB,idA,&d,&m,&y) == 6) {
            if (strcmp(idA, idAng) == 0) {
                // cari judul buku
                FILE *fb = fopen("buku.txt","r");
                char l2[1024];
                char judul[120] = "-";
                while (fb && fgets(l2, sizeof(l2), fb)) {
                    trim_newline(l2);
                    char idb[16], title[120];
                    if (sscanf(l2, "%15[^|]|%119[^|]|", idb, title) >= 2) {
                        if (strcmp(idb, idB) == 0) { strcpy(judul, title); break; }
                    }
                }
                if (fb) fclose(fb);
                printf("IDPin: %s | IDBuku: %s | Judul: %s | TglPinjam: %02d-%02d-%04d\n", idP, idB, judul, d,m,y);
                found = 1;
            }
        }
    }
    if (!found) printf("Tidak ada peminjaman aktif.\n");
    fclose(fp);
}

/* ---------- View full history (returned) for anggota ---------- */
void view_history_all(const char *idAng) {
    FILE *f = fopen("history.txt","r");
    if (!f) { printf("Belum ada history pengembalian.\n"); return; }
    char line[512];
    int found = 0;
    printf("\n=== Riwayat pengembalian (arsip) untuk %s ===\n", idAng);
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        // idPin|idBuku|idAng|tglPin|blnPin|thnPin|tglRet|blnRet|thnRet|denda
        char idP[32], idB[16], idA[16]; int tp,bp,yp,tr,br,yr,denda;
        if (sscanf(line, "%31[^|]|%15[^|]|%15[^|]|%d|%d|%d|%d|%d|%d|%d",
            idP,idB,idA,&tp,&bp,&yp,&tr,&br,&yr,&denda) == 10) {
            if (strcmp(idA, idAng) == 0) {
                // cari judul
                FILE *fb = fopen("buku.txt","r");
                char l2[1024], judul[120] = "-";
                while (fb && fgets(l2, sizeof(l2), fb)) {
                    trim_newline(l2);
                    char idb[16], title[120];
                    if (sscanf(l2, "%15[^|]|%119[^|]|", idb, title) >= 2) {
                        if (strcmp(idb, idB) == 0) { strcpy(judul, title); break; }
                    }
                }
                if (fb) fclose(fb);
                printf("IDPin: %s | Buku: %s | Pinjam: %02d-%02d-%04d | Kembali: %02d-%02d-%04d | Denda: Rp %d\n",
                    idP, judul, tp,bp,yp, tr,br,yr, denda);
                found = 1;
            }
        }
    }
    if (!found) printf("Tidak ada riwayat pengembalian.\n");
    fclose(f);
}

/* ---------- Ranking books by count ---------- */
void ranking_books() {
    FILE *f = fopen("buku.txt","r");
    if (!f) { printf("Tidak ada data buku.\n"); return; }
    Buku arr[MAX];
    int n = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        if (sscanf(line, "%9[^|]|%119[^|]|%79[^|]|%39[^|]|%15[^|]|%d|%d",
            arr[n].id,arr[n].judul,arr[n].penulis,arr[n].genre,arr[n].rilis,&arr[n].status,&arr[n].count) >= 7) {
            n++; if (n >= MAX) break;
        }
    }
    fclose(f);
    // sort descending by count
    for (int i=0;i<n;i++) for (int j=i+1;j<n;j++) if (arr[j].count > arr[i].count) {
        Buku t = arr[i]; arr[i] = arr[j]; arr[j] = t;
    }
    printf("\n=== Ranking Buku (terbanyak dipinjam) ===\n");
    for (int i=0;i<n && i<20;i++) printf("%d. %s (ID %s) - %d kali\n", i+1, arr[i].judul, arr[i].id, arr[i].count);
}
