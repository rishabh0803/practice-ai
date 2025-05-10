# Rikai: Machine Learning Library in C

This folder contains the core implementation of `Rikai`, a minimalistic machine learning library written in C.

---

## 📁 Folder Structure

```plaintext
ml-framework-1/
├── include/                # Public header files for library users
│   ├── error/
│   └── matrix/
├── private/
│   └── include/            # Internal headers (not for distribution)
│       ├── error/
│       └── matrix/
├── src/
│   ├── error/              # Error handling module source
│   └── matrix/             # Matrix operations source
├── obj_files/              # Object files and build intermediates
├── lib/                    # Compiled static library (.a)
├── test_cases/             # Test programs and user-facing usage examples
│   └── test_matrix.c
└── README.md               # This file
```

---

## 🔧 Building the Library

To build the static library (librikai.a), navigate to the obj_files/ directory and run:

```bash
gcc -c ../src/error/*.c -I ../include/ -I ../include/error -I ../include/matrix
gcc -c ../src/matrix/*.c -I ../include -I ../include/error/ -I ../include/matrix -I ../private/include

ar rcs ../lib/librikai.a *.o
```

This will generate librikai.a in the lib/ directory.

---

## 🚀 Running Test Cases

After building the library, navigate to the test_cases/ folder and compile a test program using:

```bash
gcc test_matrix.c -I ../include -I ../include/error -I ../include/matrix -L ../lib -lrikai -o test_matrix.exe
```

This links your test code against the static library and outputs an executable test_matrix.exe.

---

## 📌 Important Notes

### 🔐 Internal Headers

A dedicated folder private/include/ is used to hold internal headers. These contain function prototypes and declarations necessary for library development but should not be exposed to end users. This separation ensures:

### 🧱 Object Files

All intermediate .o object files are built in the obj_files/ directory to avoid polluting source directories.

### 🗂️ Static Library Output

The compiled static library is placed in the lib/ folder for easy linkage with test cases or user projects.

### 🧪 Test Philosophy

Test cases are placed in test_cases/ to simulate real user scenarios. These act as both correctness tests and usage examples, ensuring library functionality aligns with user expectations.

---
