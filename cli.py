import os
import sys
import subprocess
from pathlib import Path
import typer
import platform

app = typer.Typer()

VENV_DIR = ".pytiner_env"

def detect_shell():
    if platform.system() == "Windows":
        parent = os.environ.get("ComSpec", "").lower()
        if "powershell" in parent:
            return "powershell"
        elif "cmd" in parent:
            return "cmd"
        else:
            return "windows"
    else:
        return "unix"

@app.command()
def setup():
    """Buat virtual environment di folder sekarang."""
    venv_path = Path.cwd() / VENV_DIR
    if venv_path.exists():
        typer.echo("Virtual environment sudah ada.")
        raise typer.Exit()
    
    typer.echo("Membuat virtual environment...")
    subprocess.run([sys.executable, "-m", "venv", VENV_DIR])
    typer.echo("Virtual environment berhasil dibuat di .pytiner_env")

@app.command()
def on(spawn: bool = typer.Option(False, help="Buka CMD/terminal baru dengan venv aktif")):
    venv_path = Path.cwd() / VENV_DIR
    if not venv_path.exists():
        typer.echo("Virtual environment belum dibuat. Jalankan `pytiner setup` dulu.")
        raise typer.Exit(1)

    shell = detect_shell()

    if spawn and platform.system() == "Windows":
        # Buka CMD baru yang langsung aktifkan venv
        script = f"{venv_path}\\Scripts\\activate.bat && cmd"
        subprocess.Popen(["cmd", "/k", script])
        typer.echo("CMD baru terbuka dengan venv aktif.")
        raise typer.Exit()

    # fallback seperti biasa
    if shell == "powershell":
        typer.echo(f"Jalankan di PowerShell:\n  .\\{VENV_DIR}\\Scripts\\Activate.ps1")
    elif shell == "cmd":
        typer.echo(f"Jalankan di CMD:\n  .\\{VENV_DIR}\\Scripts\\activate.bat")
    elif shell == "unix":
        typer.echo(f"Jalankan di Bash:\n  source ./{VENV_DIR}/bin/activate")
    else:
        typer.echo("Shell tidak dikenali. Silakan aktifkan venv manual.")

    
if __name__ == "__main__":
    app()
