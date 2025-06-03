# 🐍 Pytiner

**Pytiner** is a minimalist, terminal-based Python environment manager built in C, designed for developers who want a lightweight and efficient workflow. It automates the creation, activation, and management of Python virtual environments and packages — all without relying on bloated tools or frameworks.

---

## ✨ Features

- 🔧 `setup` – Quickly create a virtual environment using `python3 -m venv`
- 🚀 `on` – Opens a new terminal and activates the venv (GUI terminals only)
- 🐍 `run [file]` – Execute Python scripts inside the `code/` folder within the venv
- 📦 `install [package]` – Installs and tracks Python packages via `requirements.txt`
- ❌ `uninstall [package]` – Uninstalls packages and cleans up `requirements.txt`
- 💻 Shell enhancements for CLI-only environments using `pytiner.sh`

---

## 📦 Getting Started

### 1. Clone this Repository

```bash
git clone https://github.com/your-username/Pytiner.git
cd Pytiner
```

---

## ⚙️ Commands Overview

| Command                     | Description                                                 |
| --------------------------- | ----------------------------------------------------------- |
| `./pytiner setup`           | Create virtual environment `.pytiner_env/`                  |
| `./pytiner on`              | Open a GUI terminal with the venv activated                 |
| `./pytiner run main.py`     | Run a script inside the `code/` folder using the venv       |
| `./pytiner install flask`   | Install a Python package and add it to `requirements.txt`   |
| `./pytiner uninstall flask` | Uninstall the package and remove it from `requirements.txt` |

---

## 🧠 For CLI-Only Users

You can enable extra shell commands by sourcing the shell script:

```bash
source pytiner.sh
```

This enables:

| Shell Function | Description                      |
| -------------- | -------------------------------- |
| `pytineron`    | Activate the venv and `cd code/` |
| `pytineroff`   | Deactivate the venv              |

> 🔥 Pro Tip: Add `source /path/to/pytiner/pytiner.sh` to your `.bashrc` or `.zshrc` for automatic loading.

---

## 🪛 Requirements

* Python 3.12+
* `python3-venv` package (for Ubuntu/Debian)
* A terminal that supports bash for full features (e.g., `gnome-terminal`, `xfce4-terminal`, etc.)

Install venv on Ubuntu:

```bash
sudo apt install python3.12-venv
```

---

## 🧰 File Structure

```
Pytiner/
├── code/               # Your Python scripts go here
├── pytiner             # Compiled binary
├── pytiner.c           # Source code
├── pytiner.sh          # Optional CLI shell helpers
├── requirements.txt    # Tracked Python packages
└── .pytiner_env/       # Virtual environment folder (auto-created)
```

---

## 🧪 Example

```bash
./pytiner setup
./pytiner install requests
./pytiner run hello.py
```

Or for CLI:

```bash
source pytiner.sh
pytineron
python main.py
pytineroff
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).

---

## 🚧 Future Plans

* `Makefile` or `CMake` optional support
* Docker integration
* Package autoupdate detection

---

Made with ❤️ and a bit of C magic.

```