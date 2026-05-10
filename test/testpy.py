import ctypes
import os

# 1. Load the shared library
lib_path = os.path.abspath("libfmem.so")
safemem_lib = ctypes.CDLL(lib_path)

# 2. Define function signatures (Crucial for 64-bit pointers)
safemem_lib.safemem.argtypes = [ctypes.c_size_t]
safemem_lib.safemem.restype = ctypes.c_void_p

safemem_lib.safe_free.argtypes = [ctypes.c_void_p]
safemem_lib.safe_free.restype = None

# 3. Usage
print("--- Safe-Mem Python Integration ---")
size = 128
ptr = safemem_lib.safemem(size)

if ptr:
    print(f"Memory allocated at address: {hex(ptr)}")
    
    # Writing data to that memory (Advanced)
    char_ptr = ctypes.cast(ptr, ctypes.POINTER(ctypes.c_char))
    # Python-style data injection into your C++ slab
    
    safemem_lib.safe_free(ptr)
    print("Memory freed successfully from Python!")
