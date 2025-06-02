#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#define OS_WINDOWS 1
#else
#define OS_WINDOWS 0
#endif

int is_package_in_file(const char *package_name, FILE *file) {
    char line[256];
    rewind(file);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, package_name) == 0) {
            return 1;
        }
    }

    return 0;
}

void add_package_to_requirements(const char *package_name) {
    FILE *file = fopen("requirements.txt", "a+");
    if (!file) {
        printf("[Pytiner] ERROR: Failed to open or create requirements.txt\n");
        return;
    }

    if (is_package_in_file(package_name, file)) {
        printf("[Pytiner] Package '%s' already exists in requirements.txt\n", package_name);
        fclose(file);
        return;
    }

    fprintf(file, "%s\n", package_name);
    printf("[Pytiner] Package '%s' added to requirements.txt\n", package_name);
    fclose(file);
}

int check_python(const char *cmd) {
    char check_command[100];
    if (OS_WINDOWS)
        snprintf(check_command, sizeof(check_command), "%s --version >nul 2>&1", cmd);
    else
        snprintf(check_command, sizeof(check_command), "%s --version >/dev/null 2>&1", cmd);

    return system(check_command) == 0;
}

void setup_venv() {
    const char *interpreters[] = {"python", "python3", "py"};
    const char *chosen = NULL;

    for (int i = 0; i < 3; i++) {
        if (check_python(interpreters[i])) {
            chosen = interpreters[i];
            break;
        }
    }

    if (chosen) {
        char command[128];
        snprintf(command, sizeof(command), "%s -m venv .pytiner_env", chosen);
        printf("[Pytiner] Creating virtual environment using: %s\n", chosen);
        system(command);
        printf("[Pytiner] Virtual environment created at .pytiner_env\n");
    } else {
        printf("[Pytiner] ERROR: Python interpreter not found.\n");
    }
}

void run_container(const char *filename) {
    printf("[Pytiner] Running container with file: %s\n", filename);
    char command[256];

    if (OS_WINDOWS) {
        snprintf(command, sizeof(command), ".\\.pytiner_env\\Scripts\\python.exe code\\%s", filename);
    } else {
        snprintf(command, sizeof(command), "./.pytiner_env/bin/python3 code/%s", filename);
    }

    system(command);
}

int cmd_exists(const char *cmd) {
    char check[256];
    snprintf(check, sizeof(check), "command -v %s >/dev/null 2>&1", cmd);
    return system(check) == 0;
}

void on_container() {
    if (OS_WINDOWS) {
        printf("[Pytiner] Opening new terminal with virtual environment on Windows...\n");
        system("start cmd /k \".\\.pytiner_env\\Scripts\\activate\"");
        return;
    }

    printf("[Pytiner] Searching for available terminal on Linux/macOS...\n");

    if (cmd_exists("gnome-terminal")) {
        system("gnome-terminal -- bash -c 'source ./.pytiner_env/bin/activate; exec bash'");
    } else if (cmd_exists("konsole")) {
        system("konsole --noclose -e bash -c 'source ./.pytiner_env/bin/activate'");
    } else if (cmd_exists("xfce4-terminal")) {
        system("xfce4-terminal --hold -e 'bash -c \"source ./.pytiner_env/bin/activate; exec bash\"'");
    } else if (cmd_exists("x-terminal-emulator")) {
        system("x-terminal-emulator -e bash -c 'source ./.pytiner_env/bin/activate; exec bash'");
    } else if (cmd_exists("alacritty")) {
        system("alacritty -e bash -c 'source ./.pytiner_env/bin/activate; exec bash'");
    } else if (cmd_exists("open")) {
        printf("[Pytiner] Attempting to open Terminal.app on macOS...\n");
        system("open -a Terminal .");
    } else {
        printf("[Pytiner] ERROR: No supported terminal found!\n");
        printf("[Pytiner] Please activate the virtual environment manually.\n");
    }
}

void install_package(const char *package_name) {
    char command[256];

    if (OS_WINDOWS) {
        snprintf(command, sizeof(command), ".\\.pytiner_env\\Scripts\\pip.exe install %s", package_name);
    } else {
        snprintf(command, sizeof(command), "./.pytiner_env/bin/pip install %s", package_name);
    }

    printf("[Pytiner] Installing package: %s\n", package_name);
    int ret = system(command);
    if (ret != 0) {
        printf("[Pytiner] ERROR: Failed to install package: %s\n", package_name);
    } else {
        add_package_to_requirements(package_name);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: pytiner [setup | run <file.py> | install <package> | on]\n");
        return 1;
    }

    if (strcmp(argv[1], "setup") == 0) {
        setup_venv();
    } else if (strcmp(argv[1], "on") == 0) {
        on_container();
    } else if (strcmp(argv[1], "install") == 0) {
        if (argc < 3) {
            printf("Usage: pytiner install <package>\n");
            return 1;
        }
        install_package(argv[2]);
    } else if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            printf("Usage: pytiner run <file.py>\n");
            return 1;
        }
        run_container(argv[2]);
    } else {
        printf("Unknown command: %s\n", argv[1]);
        printf("Usage: pytiner [setup | run <file.py> | install <package> | on]\n");
        return 1;
    }

    return 0;
}
