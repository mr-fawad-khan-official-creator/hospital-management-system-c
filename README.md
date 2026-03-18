# 🏥 Hospital Management System in C

A console-based Hospital Management System built in **C** using binary file I/O, structs, and sorting algorithms — no external libraries or frameworks.

---

## 📌 Features

- **Hospital Management** — Add hospitals with city, beds, price, rating, and reviews
- **Patient Management** — Register patients and link them to hospitals by ID
- **Search by City** — Filter hospitals based on location
- **Persistent Storage** — All data saved to binary `.dat` files (survives program restarts)
- **Sorting Options** — Sort hospitals by:
  - 💰 Bed price (High to Low)
  - 🛏️ Available beds
  - 🔤 Hospital name (A–Z)
  - ⭐ Rating & number of reviews

---

## 🛠️ Tech Stack

| Component | Details |
|-----------|---------|
| Language | C (C99) |
| Storage | Binary file I/O (`fread` / `fwrite`) |
| Data Structures | Structs (`Hospital`, `Patient`) |
| Algorithms | Bubble Sort (multiple criteria) |
| Platform | Cross-platform (Windows / Linux / macOS) |

---

## 🚀 How to Run

### Prerequisites
- GCC compiler installed

### Compile & Run

```bash
# Clone the repository
git clone https://github.com/mr-fawad-khan-official-creator/hospital-management-system-c.git
cd hospital-management-system-c

# Compile
gcc Hospital.c -o hospital

# Run
./hospital          # Linux / macOS
hospital.exe        # Windows
```

---

## 📂 Project Structure

```
hospital-management-system-c/
│
├── hospital.c          # Main source file (all logic)
├── hospitals.dat       # Auto-generated: hospital records (binary)
├── patients.dat        # Auto-generated: patient records (binary)
└── README.md
```

> ⚠️ The `.dat` files are created automatically on first run. Do not edit them manually.

---

## 🖥️ Menu Overview

```
------ HOSPITAL MANAGEMENT SYSTEM ------
1. Hospital Management
2. Patient Management
3. Sorting Options
0. Exit
```

**Hospital Management**
- Add Hospital
- Display All Hospitals
- Display Hospitals by City

**Patient Management**
- Add Patient
- Display All Patients (with linked hospital name)

**Sorting**
- By Bed Price / Available Beds / Name / Rating & Reviews

---

## 📈 What I Learned

- Working with **binary file I/O** in C (`fread`, `fwrite`, `fopen` with `"ab"/"rb"` modes)
- Designing clean **struct-based data models**
- Implementing **multi-criteria sorting** with bubble sort
- Handling **edge cases**: stale data in search, buffer overflow protection, input validation

---

## 🔧 Known Limitations / Future Improvements

- [ ] Add update and delete functionality for hospitals and patients
- [ ] Replace fixed array size (100) with dynamic memory allocation
- [ ] Add duplicate ID validation on entry
- [ ] Improve input handling with `fgets` instead of `scanf`
- [ ] Add a search patient by name or disease feature

---

## 👤 Author

**MUHAMMAD FAWAD**
- GitHub: [MUHAMMAD FAWAD](https://github.com/mr-fawad-khan-official-creator)
- LinkedIn: [MUHAMMAD FAWAD](https://www.linkedin.com/in/themuhammadfawad)

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).
