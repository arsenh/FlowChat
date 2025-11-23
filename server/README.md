Here you go — clean and readable **Markdown-formatted build instructions**:

---

# **Build Instructions**

## **1. Install Conan dependencies**

```bash
conan install . --output-folder=build --build=missing
```

---

## **2. Configure with CMake**

```bash
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
```

---

## **3. Build the project**

```bash
cmake --build .
```

---

If you want, I can also generate a full `README.md` section for the build process.
