# Windows DLL Injector

A lightweight, command-line DLL injector for Windows written in C++17. This tool allows you to inject dynamic link libraries (DLLs) into running processes using the classic `CreateRemoteThread` + `LoadLibrary` technique.

## Features

- **Dual Targeting**: Inject by Process ID (PID) or Process Name (e.g., `notepad.exe`).
- **Auto-Build Script**: Includes a smart `build.bat` that automatically detects Visual Studio, CMake, or MinGW.
- **Test DLL**: Comes with a sample `test_dll` to verify injection functionality.
- **Standard C++17**: Clean, modern codebase using `std::filesystem`.

## Building

This project includes a zero-configuration build script.

1. Clone the repository.
2. Run `build.bat` in the project root.

```cmd
.\build.bat
```

The script will automatically detect your compiler (Visual Studio / MSVC, MinGW, or CMake) and generate the executables in the `build/` directory.

### Requirements
- Windows 10/11
- One of the following compilers:
  - Visual Studio 2019/2022 (Desktop development with C++)
  - MinGW-w64
  - CMake (3.15+)

## Usage

Run the injector from the command line. You must provide the target (PID or Name) and the path to the DLL.

**Syntax:**
```powershell
injector.exe <target> <dll_path>
```

**Examples:**

1. **Inject into Notepad by Name:**
   ```powershell
   .\build\injector.exe notepad.exe .\build\test_dll.dll
   ```

2. **Inject by PID:**
   ```powershell
   .\build\injector.exe 1234 C:\Path\To\My.dll
   ```

## Technical Details

The injector performs the following steps:
1. Resolves the target process ID (if a name is provided).
2. Opens a handle to the target process with `PROCESS_ALL_ACCESS`.
3. Allocates memory in the target process using `VirtualAllocEx`.
4. Writes the absolute path of the DLL into the allocated memory using `WriteProcessMemory`.
5. Creates a remote thread in the target process that executes `LoadLibraryW` with the DLL path.

## Disclaimer

This software is for educational purposes only. Users are responsible for ensuring they have permission to inject code into target processes. The authors are not responsible for any misuse of this tool.
