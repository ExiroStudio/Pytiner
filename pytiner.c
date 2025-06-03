#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#define OS_WINDOWS 1
#else
#define OS_WINDOWS 0
#endif

int is_package_in_file(const char *package_name, FILE *file)
{
    char line[256];
    rewind(file);

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, package_name) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void add_package_to_requirements(const char *package_name)
{
    FILE *file = fopen("requirements.txt", "a+");
    if (!file)
    {
        printf("[Pytiner] ERROR: Failed to open or create requirements.txt\n");
        return;
    }

    if (is_package_in_file(package_name, file))
    {
        printf("[Pytiner] Package '%s' already exists in requirements.txt\n", package_name);
        fclose(file);
        return;
    }

    fprintf(file, "%s\n", package_name);
    printf("[Pytiner] Package '%s' added to requirements.txt\n", package_name);
    fclose(file);
}

void remove_package_from_requirements(const char *package_name)
{
    FILE *file = fopen("requirements.txt", "r");
    if (!file)
    {
        printf("[Pytiner] ERROR: Failed to open requirements.txt for reading\n");
        return;
    }

    char lines[100][256]; // Simpan maksimal 100 baris
    int count = 0;

    while (fgets(lines[count], sizeof(lines[count]), file))
    {
        // Strip newline
        lines[count][strcspn(lines[count], "\r\n")] = 0;

        if (strcmp(lines[count], package_name) != 0)
        {
            count++;
        }
    }

    fclose(file);

    file = fopen("requirements.txt", "w");
    if (!file)
    {
        printf("[Pytiner] ERROR: Failed to open requirements.txt for writing\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s\n", lines[i]);
    }

    fclose(file);
    printf("[Pytiner] Package '%s' removed from requirements.txt\n", package_name);
}

int check_python(const char *cmd)
{
    char check_command[100];
    if (OS_WINDOWS)
        snprintf(check_command, sizeof(check_command), "%s --version >nul 2>&1", cmd);
    else
        snprintf(check_command, sizeof(check_command), "%s --version >/dev/null 2>&1", cmd);

    return system(check_command) == 0;
}

void setup_venv()
{
    const char *interpreters[] = {"python", "python3", "py"};
    const char *chosen = NULL;

    for (int i = 0; i < 3; i++)
    {
        if (check_python(interpreters[i]))
        {
            chosen = interpreters[i];
            break;
        }
    }

    if (chosen)
    {
        char command[128];
        snprintf(command, sizeof(command), "%s -m venv .pytiner_env", chosen);
        printf("[Pytiner] Creating virtual environment using: %s\n", chosen);
        system(command);
        printf("[Pytiner] Virtual environment created at .pytiner_env\n");
        printf("[Pytiner] Setup complete!\n");
    }
    else
    {
        printf("[Pytiner] ERROR: Python interpreter not found.\n");
    }
}

void run_container(const char *filename)
{
    printf("[Pytiner] Running container with file: %s\n", filename);
    char command[256];

    if (OS_WINDOWS)
    {
        snprintf(command, sizeof(command), ".\\.pytiner_env\\Scripts\\python.exe code\\%s", filename);
    }
    else
    {
        snprintf(command, sizeof(command), "./.pytiner_env/bin/python3 code/%s", filename);
    }

    system(command);
}

int cmd_exists(const char *cmd)
{
    char check[256];
    snprintf(check, sizeof(check), "command -v %s >/dev/null 2>&1", cmd);
    return system(check) == 0;
}

void on_container()
{
    if (OS_WINDOWS)
    {
        printf("[Pytiner] Opening new terminal with virtual environment on Windows...\n");
        system("start cmd /k \"cd code && ..\\.pytiner_env\\Scripts\\activate\"");
        return;
    }

    printf("[Pytiner] Searching for available terminal on Linux/macOS...\n");

    if (cmd_exists("gnome-terminal"))
    {
        system("gnome-terminal -- bash -c 'cd code && source ../.pytiner_env/bin/activate; exec bash'");
    }
    else if (cmd_exists("konsole"))
    {
        system("konsole --noclose -e bash -c 'cd code && source ../.pytiner_env/bin/activate; exec bash'");
    }
    else if (cmd_exists("xfce4-terminal"))
    {
        system("xfce4-terminal --hold -e 'bash -c \"cd code && source ../.pytiner_env/bin/activate; exec bash\"'");
    }
    else if (cmd_exists("x-terminal-emulator"))
    {
        system("x-terminal-emulator -e bash -c 'cd code && source ../.pytiner_env/bin/activate; exec bash'");
    }
    else if (cmd_exists("alacritty"))
    {
        system("alacritty -e bash -c 'cd code && source ../.pytiner_env/bin/activate; exec bash'");
    }
    else if (cmd_exists("open"))
    {
        printf("[Pytiner] Attempting to open Terminal.app on macOS...\n");
        system("open -a Terminal ./code");
        printf("[Pytiner] After terminal opens, run:\n");
        printf("    source ../.pytiner_env/bin/activate\n");
    }
    else
    {
        printf("[Pytiner] ERROR: No supported terminal found!\n");
        printf("[Pytiner] Please activate the virtual environment manually:\n");
        printf("    source activate.sh\n");
    }
}

void uninstall_package(const char *package_name)
{
    char command[256];

    if (OS_WINDOWS)
    {
        snprintf(command, sizeof(command), ".\\.pytiner_env\\Scripts\\pip.exe uninstall %s", package_name);
    }
    else
    {
        snprintf(command, sizeof(command), "./.pytiner_env/bin/pip uninstall %s", package_name);
    }

    printf("[Pytiner] Uninstalling package: %s\n", package_name);
    int ret = system(command);
    if (ret != 0)
    {
        printf("[Pytiner] ERROR: Failed to uninstall package: %s\n", package_name);
    }
    else
    {
        remove_package_from_requirements(package_name);
    }
}

void install_package(const char *package_name)
{
    char command[256];

    if (OS_WINDOWS)
    {
        snprintf(command, sizeof(command), ".\\.pytiner_env\\Scripts\\pip.exe install %s", package_name);
    }
    else
    {
        snprintf(command, sizeof(command), "./.pytiner_env/bin/pip install %s", package_name);
    }

    printf("[Pytiner] Installing package: %s\n", package_name);
    int ret = system(command);
    if (ret != 0)
    {
        printf("[Pytiner] ERROR: Failed to install package: %s\n", package_name);
    }
    else
    {
        add_package_to_requirements(package_name);
    }
}

void help()
{
    printf("\n[Pytiner Help Guide]\n");
    printf("============================================================\n");
    printf("NOTE: On Linux or macOS, always run Pytiner using './pytiner'\n");
    printf("      Example: ./pytiner setup\n\n");

    printf("Available Commands:\n");
    printf("  setup                       - Create a virtual environment (.pytiner_env)\n");
    printf("  on                          - Open a new terminal and auto-activate the venv\n");
    printf("                                (Only works in GUI environments)\n");
    printf("  run [file.py]               - Run the specified Python file inside the 'code/' folder\n");
    printf("                                using the virtual environment\n");
    printf("  install [package]           - Install a Python package and add it to requirements.txt\n");
    printf("  uninstall [package]         - Uninstall a package and remove it from requirements.txt\n\n");
    printf("  install requirement         - Install the Python package in requirements.txt\n");

    printf("Advanced CLI Commands:\n");
    printf("  ./pytiner.sh                - Load extra shell functions for CLI environments\n");
    printf("    > pytineron               - Activate the virtual environment and enter 'code/'\n");
    printf("    > pytineroff / deactivate - Deactivate the virtual environment\n");

    printf("============================================================\n");
    printf("Tips:\n");
    printf("- Use 'pytineron' for long sessions inside the venv\n");
    printf("- Combine with aliases or bash functions for better experience\n\n");
}


void install_from_requirements()
{
    FILE *file = fopen("requirements.txt", "r");
    if (!file)
    {
        printf("[Pytiner] ERROR: requirements.txt not found.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        // Remove newline characters
        line[strcspn(line, "\r\n")] = 0;

        if (strlen(line) == 0)
            continue;

        printf("[Pytiner] Installing package from requirements: %s\n", line);

        char command[300];
        if (OS_WINDOWS)
        {
            snprintf(command, sizeof(command), ".\\.pytiner_env\\Scripts\\pip.exe install %s", line);
        }
        else
        {
            snprintf(command, sizeof(command), "./.pytiner_env/bin/pip install %s", line);
        }

        int result = system(command);
        if (result != 0)
        {
            printf("[Pytiner] ERROR: Failed to install %s\n", line);
        }
        else
        {
            printf("[Pytiner] Installed %s successfully\n", line);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        help();
        return 1;
    }

    if (strcmp(argv[1], "setup") == 0)
    {
        setup_venv();
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        help();
        return 1;
    }
    else if (strcmp(argv[1], "install") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: pytiner install <package | requirement>\n");
            return 1;
        }
        if (strcmp(argv[2], "requirement") == 0)
        {
            install_from_requirements();
        }
        else
        {
            install_package(argv[2]);
        }
    }
    else if (strcmp(argv[1], "uninstall") == 0)
    {
        uninstall_package(argv[2]);
    }
    else if (strcmp(argv[1], "on") == 0)
    {
        on_container();
    }
    else if (strcmp(argv[1], "install") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: pytiner install <package>\n");
            return 1;
        }
        install_package(argv[2]);
    }
    else if (strcmp(argv[1], "run") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: pytiner run <file.py>\n");
            return 1;
        }
        run_container(argv[2]);
    }
    else
    {
        printf("Unknown command: %s\n", argv[1]);
        printf("Usage: pytiner [setup | run <file.py> | install <package> | on]\n");
        return 1;
    }

    return 0;
}
