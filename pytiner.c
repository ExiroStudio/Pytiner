#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define OS_WINDOWS 1
#else
    #define OS_WINDOWS 0
#endif

void run_setup() {
    printf("[Pytiner] Membuat virtual environment...\n");

    if (OS_WINDOWS) {
        system("python -m venv .pytiner_env");
    } else {
        system("python3 -m venv .pytiner_env");
    }

    printf("[Pytiner] Virtual environment dibuat di .pytiner_env\n");
}

void run_container() {
    printf("[Pytiner] Menjalankan container (venv + main.py)...\n");

    if (OS_WINDOWS) {
        system(".\\.pytiner_env\\Scripts\\python.exe code\\main.py");
    } else {
        system("./.pytiner_env/bin/python3 code/main.py");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Gunakan: pytiner [setup | run]\n");
        return 1;
    }

    if (strcmp(argv[1], "setup") == 0) {
        run_setup();
    } else if (strcmp(argv[1], "run") == 0) {
        run_container();
    } else {
        printf("Perintah tidak dikenali: %s\n", argv[1]);
        printf("Gunakan: pytiner [setup | run]\n");
        return 1;
    }

    return 0;
}
