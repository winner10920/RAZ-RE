#!/bin/bash

##############################################################################
#
# XD0011_F_generate_graphs.sh
#
# Bash script to convert Graphviz DOT and Mermaid diagrams to SVG format
#
# USAGE:
#   chmod +x XD0011_F_generate_graphs.sh
#   ./XD0011_F_generate_graphs.sh
#
# DEPENDENCIES:
#   - graphviz (dot command)
#   - mermaid-cli (mmdc command, optional)
#
# INSTALLATION:
#   Ubuntu/Debian:
#     sudo apt-get install graphviz
#     npm install -g mermaid-cli
#
#   macOS:
#     brew install graphviz
#     npm install -g mermaid-cli
#
#   Windows (WSL):
#     sudo apt-get install graphviz
#     npm install -g mermaid-cli
#
##############################################################################

set -e  # Exit on error

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DOT_FILE="$SCRIPT_DIR/XD0011_F_function_dependency.dot"
MMD_FILE="$SCRIPT_DIR/XD0011_F_execution_flow.mmd"
SVG_DOT_OUTPUT="$SCRIPT_DIR/XD0011_F_function_dependency.svg"
SVG_MMD_OUTPUT="$SCRIPT_DIR/XD0011_F_execution_flow.svg"
PDF_DOT_OUTPUT="$SCRIPT_DIR/XD0011_F_function_dependency.pdf"
PNG_DOT_OUTPUT="$SCRIPT_DIR/XD0011_F_function_dependency.png"

echo "=========================================="
echo "XD0011_F Graph Generation Script"
echo "=========================================="
echo ""

# Check for graphviz installation
if ! command -v dot &> /dev/null; then
    echo "ERROR: graphviz not found!"
    echo "Install with: sudo apt-get install graphviz (Linux)"
    echo "             brew install graphviz (macOS)"
    exit 1
fi

echo "✓ Graphviz found: $(dot -V 2>&1)"
echo ""

# Convert DOT to SVG (function dependency diagram)
if [ -f "$DOT_FILE" ]; then
    echo "Converting DOT to SVG..."
    echo "  Input:  $DOT_FILE"
    echo "  Output: $SVG_DOT_OUTPUT"
    
    # Generate SVG with optimized settings
    dot -Tsvg \
        -Gsize="20,40" \
        -Goverlap=false \
        -Gsep="+0.5" \
        -Gsplines=curved \
        "$DOT_FILE" \
        -o "$SVG_DOT_OUTPUT"
    
    if [ $? -eq 0 ]; then
        SIZE=$(du -h "$SVG_DOT_OUTPUT" | cut -f1)
        echo "  ✓ Created: $SIZE"
        echo ""
    else
        echo "  ✗ Failed to generate SVG"
        exit 1
    fi
    
    # Also generate PDF version
    echo "Generating PDF (high quality)..."
    dot -Tpdf \
        -Gsize="20,40" \
        -Goverlap=false \
        "$DOT_FILE" \
        -o "$PDF_DOT_OUTPUT"
    
    if [ $? -eq 0 ]; then
        echo "  ✓ Created PDF: $(du -h "$PDF_DOT_OUTPUT" | cut -f1)"
        echo ""
    fi
    
    # Generate PNG version for quick preview
    echo "Generating PNG (preview)..."
    dot -Tpng \
        -Gsize="20,40" \
        -Goverlap=false \
        -Dpi=100 \
        "$DOT_FILE" \
        -o "$PNG_DOT_OUTPUT"
    
    if [ $? -eq 0 ]; then
        echo "  ✓ Created PNG: $(du -h "$PNG_DOT_OUTPUT" | cut -f1)"
        echo ""
    fi
    
else
    echo "ERROR: DOT file not found: $DOT_FILE"
    exit 1
fi

# Convert Mermaid to SVG (execution flow diagram)
if [ -f "$MMD_FILE" ]; then
    echo "Converting Mermaid to SVG..."
    echo "  Input:  $MMD_FILE"
    echo "  Output: $SVG_MMD_OUTPUT"
    
    if command -v mmdc &> /dev/null; then
        mmdc -i "$MMD_FILE" \
             -o "$SVG_MMD_OUTPUT" \
             -s 2 \
             --width 1920 \
             --height 1080
        
        if [ $? -eq 0 ]; then
            SIZE=$(du -h "$SVG_MMD_OUTPUT" | cut -f1)
            echo "  ✓ Created: $SIZE"
            echo ""
        else
            echo "  ⚠ Mermaid conversion failed (optional)"
            echo "  Install mermaid-cli: npm install -g mermaid-cli"
            echo ""
        fi
    else
        echo "  ⚠ mermaid-cli not found (optional)"
        echo "  Install with: npm install -g mermaid-cli"
        echo "  For now, view Mermaid diagram at: https://mermaid.live/"
        echo "  Copy contents of: $MMD_FILE"
        echo ""
    fi
else
    echo "ERROR: Mermaid file not found: $MMD_FILE"
    exit 1
fi

# Generate alternative DOT layouts
echo "Generating alternative graph layouts..."

# Circular layout
echo "  Generating circular layout..."
dot -Tsvg \
    -Kneato \
    -Gstart=rand \
    -Gsep="+0.5" \
    "$DOT_FILE" \
    -o "$SCRIPT_DIR/XD0011_F_function_dependency_circular.svg" 2>/dev/null && \
    echo "    ✓ Circular layout created"

# Spring layout
echo "  Generating spring layout..."
dot -Tsvg \
    -Ksfdp \
    -GK=0.5 \
    -GL=1.5 \
    "$DOT_FILE" \
    -o "$SCRIPT_DIR/XD0011_F_function_dependency_spring.svg" 2>/dev/null && \
    echo "    ✓ Spring layout created"

echo ""

# Summary and usage instructions
cat << 'EOF'
==========================================
✓ Graph Generation Complete!
==========================================

GENERATED FILES:
  Function Dependency (Graphviz DOT):
    - XD0011_F_function_dependency.svg    (Main - Hierarchical layout)
    - XD0011_F_function_dependency_circular.svg
    - XD0011_F_function_dependency_spring.svg
    - XD0011_F_function_dependency.pdf    (High quality PDF)
    - XD0011_F_function_dependency.png    (Quick preview)

  Execution Flow (Mermaid):
    - XD0011_F_execution_flow.svg          (If mermaid-cli installed)
    - XD0011_F_execution_flow.mmd         (Source - view at https://mermaid.live/)

VIEWING OPTIONS:
  1. SVG in Browser:
     - Open any .svg file in your web browser
     - Zoom, pan, and right-click to download

  2. VS Code:
     - Install "SVG Preview" extension
     - Right-click .svg → Open Preview

  3. Mermaid Diagram Online:
     - Go to https://mermaid.live/
     - Copy contents of XD0011_F_execution_flow.mmd
     - Paste and view interactively

  4. PDF (Print Quality):
     - Open XD0011_F_function_dependency.pdf in PDF viewer

INTERPRETING THE GRAPHS:

  Function Dependency (DOT):
    Colors:
      Red    = State machine & main control flow
      Teal   = Hardware drivers & core functionality
      Blue   = Utility & helper functions
      Orange = Floating-point math library
      Purple = Power management

    Arrows:
      → calls     : Direct function call
      → depends   : Data dependency (not code call)
      → polling   : Status polling loop

    Clusters:
      Group related functions by category
      Clusters can be expanded/collapsed in some viewers

  Execution Flow (Mermaid):
    Boxes:
      Rounded rectangles = Processing steps
      Diamonds          = Decision points
      Rounded corners   = Start/End

    Colors:
      Green  = Start state
      Red    = Error condition
      Gold   = Decision points
      Teal   = Sensor operations
      Blue   = LCD operations

NEXT STEPS:
  1. Study the function dependency diagram to understand module organization
  2. Trace execution paths using the flow diagram
  3. Identify critical paths (boot, measurement, display update)
  4. Look for optimization opportunities in long call chains

EOF

echo ""
echo "Graph generation script completed successfully!"
echo ""
