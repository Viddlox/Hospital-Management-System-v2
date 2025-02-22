# 🏥 MedTek+ Hospital Management System

🚀 **A terminal-based hospital management system designed for efficient record management, streamlined navigation, and an intuitive user interface.**

## 📖 Table of Contents
- [📌 Introduction](#-introduction)
- [⚙️ Features](#-features)
- [💻 System Requirements](#-system-requirements)
- [📦 Installation](#-installation)
- [🎮 Controls & Key Bindings](#-controls--key-bindings)
- [🗺️ User Journey](#-user-journey)
- [✅ Data Validation Rules](#-data-validation-rules)

---

## 📌 Introduction
MedTek+ Hospital Management System is a **terminal-based application** built for **hospital staff** to efficiently manage patient and admin records. 

✅ **Features:** 
- Interactive **text-based UI** powered by **ncurses** 🖥️
- **CRUD operations** for patient & admin records 📝
- **Paginated search system** for efficient record retrieval 🔍
- **Local file storage using JSON** for data persistence 📁

---

## ⚙️ Features
✨ **User-friendly CLI interface**
✨ **Multi-threaded for seamless performance**
✨ **Interactive menu-driven navigation**
✨ **Advanced record filtering & search**
✨ **Secure Admin authentication**

---

## 💻 System Requirements
🖥️ **Operating System:** Linux (Recommended), Windows (via WSL)
🛠️ **Compiler:** g++ (C++17 support)
📚 **Dependencies:** `ncursesw`, `nlohmann-json`, `uuidv4`
💾 **Storage:** Local JSON-based file system

---

## 📦 Installation
### 🏗️ **Windows Installation Using WSL (Recommended)**
1️⃣ **Enable WSL & Install Ubuntu:**
   - Open **PowerShell** as Administrator and run:
     ```powershell
     wsl --install
     ```
   - Restart your computer when prompted.

2️⃣ **Install Dependencies in Ubuntu:**
   ```bash
   sudo apt update && sudo apt upgrade -y
   sudo apt install g++ make libncursesw5-dev libformw5-dev libmenuw5-dev uuid-dev
   ```

3️⃣ **Clone the Repository:**
   ```bash
   git clone https://github.com/Viddlox/Hospital-Management-System-v2.git
   cd Hospital-Management-System-v2
   ```

4️⃣ **Compile the Program:**
   ```bash
   make re
   ```

5️⃣ **Run the Executable:**
   ```bash
   ./Hospital_Management_System.exe
   ```

---

## 🎮 Controls & Key Bindings
🕹️ **Navigation:**
- ⬆️⬇️⬅️➡️ Arrow Keys - Move selection
- ↩️ Enter - Select / Confirm an action
- 🔄 `TAB` - Toggle between filters (Admin/Patient DB, Profile/Admissions)
- 📜 `PgUp/PgDn` - Scroll through paginated records
- ❌ `ESC / CTRL + C` - Exit application
- ⬅️ `CTRL + B` - Go back

📝 **Record Actions:**
- ➕ `+` - Create new record (Patient/Admin)
- ❌ `Delete` - Remove selected record

🔍 **Search & Filter:**
- 🔎 Type in search fields to filter results

---

## 🗺️ User Journey
1️⃣ **Login Screen:** Secure authentication for staff & admins 🔐
2️⃣ **Dashboard:** Quick access to record management options 📋
3️⃣ **Profile Screen:** View & edit current user details 👤
4️⃣ **Database Screen:** Manage **Patient & Admin Records** 📑

🎯 **Supported Operations:**
✔️ **Add / View / Update / Delete** Patient & Admin Records
✔️ **Manage Admissions**
✔️ **Advanced Search & Pagination**
✔️ **Secure Role-based Access Control**

---

## ✅ Data Validation Rules
⚠️ **Strict Input Validation Ensured!**
- 📌 **Names:** Letters & spaces only
- 📅 **Age:** Numeric (0-120)
- 📞 **Phone:** 10-15 digit numeric
- ⚖️ **Weight/Height:** Numeric (realistic BMI range)
- 🆔 **IC Number:** Must follow `010403141107` format
- 📧 **Email:** Must be a valid format
- 🔐 **Unique usernames for all accounts**
- ❌ **Admins cannot delete themselves**
- 🚫 **Avoid modifying `/db` manually**

---

### 💡 **Contributing & Support**
👥 **Contributions are welcome!** Feel free to fork, submit issues, and improve this project. 🚀
📩 **Need help?** Open an issue on the repository.

---

📝 License

This project is licensed under the MIT License. See the [LICENSE](license.txt) file for details.

---

⚡ **Built with ❤️ for efficiency & reliability.**

