# Emulator - CaNS assignment 1
Complete 60 marks emulator for CaNS at UWE by William Randall (16002374).

## How To (Compiling binary from source)
### Linux
To compile and run this emulator, you need:
- wine
- mingw-w64-gcc

Clone this source code, run `make` from *bash*.

### Windows
- Create a new VisualStudio C++ empty console application
- Copy all files from `src` into `Source Files`
- Ensure `_CRT_SECURE_NO_WARNINGS` and `_WINSOCK_DEPRECATED_NO_WARNINGS` have been added to the project preprocessor definitions
