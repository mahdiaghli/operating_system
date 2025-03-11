Project Description: Parallel File Search Utility

This project is a multi-threaded, multi-process file search utility designed to efficiently search for a specific text pattern within files across directories. It combines a C-based backend for parallel processing with a Python-based GUI for user interaction.

# Key Features:
- Parallel Processing:  
  Utilizes both POSIX threads (pthreads) and `fork()` system calls to traverse directories and search files concurrently, improving performance on multi-core systems.

- Synchronization:  
  Implements `mutex` locks and semaphores to ensure thread-safe access to shared resources (e.g., matched file counters).

- GUI Interface:  
  A Tkinter-based Python frontend (`fff.py`) allows users to input a root directory and search pattern. It executes the compiled C program and displays results in a scrollable text widget.

- Recursive Directory Traversal:  
  The C backend (`first.c`/`test2sag.c`) recursively explores directories, spawning child processes for subdirectories and threads for file processing.

- Match Reporting:  
  Outputs filenames, line numbers, and matching lines containing the pattern. Provides a summary of total checked files and matches.

# Technologies Used:
- C: Core logic for parallel file processing and directory traversal.
- Python (Tkinter): GUI for user input and result visualization.
- POSIX APIs: `fork()`, `pthread`, `semaphore`, and `mmap` for shared memory.
- System Programming: File I/O, process management, and inter-process communication.

# How It Works:
1. GUI Input: The user specifies a root directory and search pattern via the Tkinter interface.
2. C Backend Execution: The Python script invokes the compiled C executable, passing the directory and pattern as arguments.
3. Parallel Search:  
   - Child processes handle subdirectories via `fork()`.  
   - Threads (`pthread`) process individual files, scanning lines for the pattern.  
   - Results are synchronized using mutexes and semaphores to avoid race conditions.
4. Result Display: The GUI captures the C program's output and displays it in a user-friendly format.

# Usage:
1. Compile the C program:  
   ```bash
   gcc first.c -o search_tool -lpthread
   ```
2. Run the Python GUI:  
   ```bash
   python3 fff.py
   ```

# Example Output:
```
file1.txt:3: hello world
file2.c:10: printf("hello");
Total number of files: 42
Total number of Matches: 5
```

# Ideal For:
- Developers needing a customizable parallel file search tool.
- Educational purposes (demonstrating multi-threading, process management, and synchronization).

Note: Adjust the root directory and search pattern in the C code (`main()`) or via the GUI for custom use cases.
