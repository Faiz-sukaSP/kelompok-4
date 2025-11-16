#include "perpustakaan.h"

int generateIDPeminjaman()
{
    static int id = 1;
    return id++;
}

void tanggalHariIni(char *buffer)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(buffer, "%02d-%02d-%d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
}

int uraiTanggal(const char *tgl)
{
    int d, m, y;
    sscanf(tgl, "%d-%d-%d", &d, &m, &y);

    struct tm waktu = {0};
    waktu.tm_mday = d;
    waktu.tm_mon = m - 1;
    waktu.tm_year = y - 1900;

    return mktime(&waktu);
}

double hitungDenda(const char *tglPinjam, const char *tglKembali)
{
    int t1 = uraiTanggal(tglPinjam);
    int t2 = uraiTanggal(tglKembali);

    double selisih = difftime(t2, t1) / (60 * 60 * 24);

    if (selisih <= 7)
        return 0;
    return (selisih - 7) * 1000;
}

void simpanPeminjamanKeFile(Peminjaman p)
{
    FILE *fp = fopen("peminjaman.dat", "ab");
    fwrite(&p, sizeof(Peminjaman), 1, fp);
    fclose(fp);
}

void loadPeminjamanDariFile(Peminjaman *arr, int *n)
{
    FILE *fp = fopen("peminjaman.dat", "rb");
    if (!fp)
    {
        *n = 0;
        return;
    }
    *n = 0;
    while (fread(&arr[*n], sizeof(Peminjaman), 1, fp))
        (*n)++;
    fclose(fp);
}

void tambahRiwayat(Peminjaman p, const char *namaFile)
{
    FILE *fp = fopen(namaFile, "a");
    fprintf(fp, "%s | %s | %s | %s | %s | %d | %s\n",
            p.id_peminjaman,
            p.id_buku,
            p.npm_user,
            p.tanggal_pinjam,
            p.tanggal_pengembalian,
            p.denda,
            p.status_pengembalian ? "kembali" : "dipinjam");
    fclose(fp);
}

void pinjamBuku()
{
    Peminjaman p;

    printf("Masukkan NPM: ");
    scanf("%s", p.npm_user);

    if (!validasiAnggota(p.npm_user))
    {
        printf("NPM tidak terdaftar!\n");
        return;
    }

    printf("Masukkan ID Buku: ");
    scanf("%s", p.id_buku);

    if (!cekStatusBuku(p.id_buku))
    {
        printf("Buku tidak tersedia!\n");
        return;
    }

    int id = generateIDPeminjaman();
    sprintf(p.id_peminjaman, "P%04d", id);

    tanggalHariIni(p.tanggal_pinjam);
    strcpy(p.tanggal_pengembalian, "-");
    p.status_pengembalian = 0;
    p.denda = 0;

    updateStatusBuku(p.id_buku, "dipinjam");
    simpanPeminjamanKeFile(p);
    tambahRiwayat(p, "riwayat.txt");

    printf("Peminjaman berhasil! ID: %s\n", p.id_peminjaman);
}

void kembalikanBuku()
{
    Peminjaman arr[200];
    int n;
    loadPeminjamanDariFile(arr, &n);

    char id[20];
    printf("Masukkan ID Peminjaman: ");
    scanf("%s", id);

    for (int i = 0; i < n; i++)
    {
        if (strcmp(arr[i].id_peminjaman, id) == 0 && arr[i].status_pengembalian == 0)
        {
            tanggalHariIni(arr[i].tanggal_pengembalian);
            arr[i].denda = hitungDenda(arr[i].tanggal_pinjam, arr[i].tanggal_pengembalian);
            arr[i].status_pengembalian = 1;

            updateStatusBuku(arr[i].id_buku, "tersedia");

            FILE *fp = fopen("peminjaman.dat", "wb");
            fwrite(arr, sizeof(Peminjaman), n, fp);
            fclose(fp);

            tambahRiwayat(arr[i], "riwayat.txt");

            printf("Buku dikembalikan! Denda: %d\n", arr[i].denda);
            return;
        }
    }

    printf("ID tidak ditemukan atau buku sudah dikembalikan.\n");
}

void tampilkanDaftarPeminjaman()
{
    Peminjaman arr[200];
    int n;
    loadPeminjamanDariFile(arr, &n);

    printf("\n=== DAFTAR PEMINJAMAN ===\n");
    for (int i = 0; i < n; i++)
    {
        printf("%s | %s | %s | %s | %s | %d | %s\n",
               arr[i].id_peminjaman,
               arr[i].id_buku,
               arr[i].npm_user,
               arr[i].tanggal_pinjam,
               arr[i].tanggal_pengembalian,
               arr[i].denda,
               arr[i].status_pengembalian ? "kembali" : "dipinjam");
    }
}

void cariPeminjamanByNPM()
{
    char npm[20];
    printf("Masukkan NPM: ");
    scanf("%s", npm);

    Peminjaman arr[200];
    int n;
    loadPeminjamanDariFile(arr, &n);

    printf("\n=== DATA PEMINJAMAN NPM %s ===\n", npm);
    for (int i = 0; i < n; i++)
    {
        if (strcmp(arr[i].npm_user, npm) == 0)
        {
            printf("%s | %s | %s | %s | %s | %d | %s\n",
                   arr[i].id_peminjaman,
                   arr[i].id_buku,
                   arr[i].npm_user,
                   arr[i].tanggal_pinjam,
                   arr[i].tanggal_pengembalian,
                   arr[i].denda,
                   arr[i].status_pengembalian ? "kembali" : "dipinjam");
        }
    }
}
