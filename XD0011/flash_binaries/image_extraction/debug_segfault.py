#!/usr/bin/env python3
"""
Debug wrapper to find exact line causing segfault
Run with: python3 -X faulthandler debug_segfault.py
"""

import faulthandler
import sys
import traceback

# Enable fault handler to show C-level segfaults
faulthandler.enable()

# Import after faulthandler is enabled
print("Importing modules...")
sys.stdout.flush()

try:
    from extract_images import main
    print("Import successful, starting main()...")
    sys.stdout.flush()
    main()
except Exception as e:
    print(f"\nPython exception caught:")
    traceback.print_exc()
    sys.exit(1)
