#!/usr/bin/env python3

"""
XD0011_F_dot_to_svg_converter.py

Python-based converter for Graphviz DOT diagrams to SVG format.
Provides more flexibility than bash script and works cross-platform.

USAGE:
    python3 XD0011_F_dot_to_svg_converter.py

DEPENDENCIES:
    pip install graphviz pillow

FEATURES:
    - Converts DOT to multiple formats (SVG, PDF, PNG)
    - Generates multiple layout styles (hierarchical, circular, spring)
    - Creates summary statistics about the graph
    - Validates DOT syntax before conversion
    - Cross-platform (Windows, macOS, Linux)
"""

import os
import sys
import subprocess
import json
from pathlib import Path
from typing import Optional, List, Dict
import re

class DotGraphConverter:
    """Convert Graphviz DOT files to various formats"""
    
    def __init__(self, dot_file: str):
        """
        Initialize converter with DOT file path
        
        Args:
            dot_file: Path to .dot file
        """
        self.dot_file = Path(dot_file)
        self.output_dir = self.dot_file.parent
        self.base_name = self.dot_file.stem
        
        if not self.dot_file.exists():
            raise FileNotFoundError(f"DOT file not found: {self.dot_file}")
    
    def check_graphviz_installed(self) -> bool:
        """Check if graphviz 'dot' command is available"""
        try:
            subprocess.run(['dot', '-V'], capture_output=True, check=True)
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            return False
    
    def validate_dot_syntax(self) -> bool:
        """Validate DOT file syntax"""
        try:
            result = subprocess.run(
                ['dot', '-Tsvg', str(self.dot_file), '-o', '/dev/null'],
                capture_output=True,
                text=True,
                timeout=10
            )
            if result.returncode == 0:
                return True
            else:
                print(f"DOT validation error: {result.stderr}")
                return False
        except Exception as e:
            print(f"Validation failed: {e}")
            return False
    
    def extract_graph_stats(self) -> Dict:
        """Extract statistics from DOT file"""
        stats = {
            'nodes': 0,
            'edges': 0,
            'clusters': 0,
            'subgraphs': 0,
            'node_types': {}
        }
        
        with open(self.dot_file, 'r') as f:
            content = f.read()
            
            # Count nodes
            node_pattern = r'\w+\s*\[.*?\];'
            stats['nodes'] = len(re.findall(node_pattern, content))
            
            # Count edges
            edge_pattern = r'->'
            stats['edges'] = len(re.findall(edge_pattern, content))
            
            # Count clusters
            stats['clusters'] = content.count('subgraph cluster_')
            stats['subgraphs'] = content.count('subgraph ')
            
            # Count node colors
            fill_colors = re.findall(r'fillcolor = "([^"]+)"', content)
            for color in fill_colors:
                stats['node_types'][color] = stats['node_types'].get(color, 0) + 1
        
        return stats
    
    def convert_to_svg(self, layout: str = 'dot', optimize: bool = True) -> Optional[str]:
        """
        Convert DOT to SVG
        
        Args:
            layout: Graph layout engine ('dot', 'neato', 'fdp', 'sfdp', 'circo', 'twopi')
            optimize: Apply optimization flags
        
        Returns:
            Path to generated SVG file, or None on error
        """
        output_file = self.output_dir / f"{self.base_name}_{layout}.svg"
        
        cmd = ['dot']
        
        # Add layout engine
        if layout != 'dot':
            cmd.insert(1, f'-K{layout}')
        
        # Add output format and file
        cmd.extend(['-Tsvg', str(self.dot_file), '-o', str(output_file)])
        
        # Add optimization flags
        if optimize:
            cmd.extend([
                '-Gsize=20,40',
                '-Goverlap=false',
                '-Gsep=+0.5',
                '-Gsplines=curved'
            ])
        
        try:
            print(f"Converting to SVG ({layout} layout)...")
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0 and output_file.exists():
                file_size = os.path.getsize(output_file) / 1024  # KB
                print(f"  ✓ Created: {output_file.name} ({file_size:.1f} KB)")
                return str(output_file)
            else:
                print(f"  ✗ Conversion failed: {result.stderr}")
                return None
        except subprocess.TimeoutExpired:
            print(f"  ✗ Conversion timeout (>30s)")
            return None
        except Exception as e:
            print(f"  ✗ Error: {e}")
            return None
    
    def convert_to_pdf(self) -> Optional[str]:
        """Convert DOT to PDF (high quality)"""
        output_file = self.output_dir / f"{self.base_name}.pdf"
        
        cmd = [
            'dot', '-Tpdf',
            '-Gsize=20,40',
            '-Goverlap=false',
            str(self.dot_file),
            '-o', str(output_file)
        ]
        
        try:
            print(f"Converting to PDF...")
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0 and output_file.exists():
                file_size = os.path.getsize(output_file) / 1024
                print(f"  ✓ Created: {output_file.name} ({file_size:.1f} KB)")
                return str(output_file)
            else:
                print(f"  ✗ Conversion failed: {result.stderr}")
                return None
        except Exception as e:
            print(f"  ✗ Error: {e}")
            return None
    
    def convert_to_png(self, dpi: int = 100) -> Optional[str]:
        """Convert DOT to PNG"""
        output_file = self.output_dir / f"{self.base_name}.png"
        
        cmd = [
            'dot', '-Tpng',
            '-Gsize=20,40',
            '-Goverlap=false',
            f'-Dpi={dpi}',
            str(self.dot_file),
            '-o', str(output_file)
        ]
        
        try:
            print(f"Converting to PNG (DPI={dpi})...")
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0 and output_file.exists():
                file_size = os.path.getsize(output_file) / 1024
                print(f"  ✓ Created: {output_file.name} ({file_size:.1f} KB)")
                return str(output_file)
            else:
                print(f"  ✗ Conversion failed: {result.stderr}")
                return None
        except Exception as e:
            print(f"  ✗ Error: {e}")
            return None
    
    def generate_all_formats(self) -> Dict[str, str]:
        """Generate all supported formats"""
        outputs = {}
        
        # SVG with different layouts
        layouts = ['dot', 'neato', 'sfdp']
        for layout in layouts:
            result = self.convert_to_svg(layout)
            if result:
                outputs[f'svg_{layout}'] = result
        
        # PDF
        result = self.convert_to_pdf()
        if result:
            outputs['pdf'] = result
        
        # PNG
        result = self.convert_to_png()
        if result:
            outputs['png'] = result
        
        return outputs
    
    def print_summary(self):
        """Print conversion summary and usage instructions"""
        stats = self.extract_graph_stats()
        
        summary = f"""
{'='*60}
DOT to SVG Conversion Summary
{'='*60}

INPUT FILE:  {self.dot_file}

GRAPH STATISTICS:
  Nodes:       {stats['nodes']}
  Edges:       {stats['edges']}
  Clusters:    {stats['clusters']}
  Subgraphs:   {stats['subgraphs']}
  
  Node Colors:
"""
        for color, count in stats['node_types'].items():
            summary += f"    {color:25s}: {count:3d} nodes\n"
        
        summary += f"""
{'='*60}

LAYOUTS GENERATED:
  1. Hierarchical (dot)   - Top-down function call hierarchy
  2. Circular (neato)     - Circular arrangement (good for clusters)
  3. Spring (sfdp)        - Physics-based layout (best quality)

USAGE:
  1. Open SVG files in web browser
     - All modern browsers support SVG
     - Zoom, pan with mouse
     - Right-click to download

  2. Open PDF in document viewer
     - Best for printing
     - Vector format (no quality loss)

  3. Open PNG for quick preview
     - Raster format
     - Fast loading

VIEWING TIPS:
  - Use "Spring" layout for best overall visualization
  - Use "Hierarchical" for understanding call flow
  - Use "Circular" for identifying clusters/modules

{'='*60}
"""
        print(summary)

def main():
    """Main entry point"""
    
    # Find DOT file in script directory
    script_dir = Path(__file__).parent
    dot_file = script_dir / "XD0011_F_function_dependency.dot"
    
    if not dot_file.exists():
        print(f"ERROR: DOT file not found: {dot_file}")
        print("\nUsage: Place this script in same directory as .dot file")
        sys.exit(1)
    
    print("\n" + "="*60)
    print("XD0011_F DOT to SVG/PDF/PNG Converter")
    print("="*60 + "\n")
    
    # Create converter
    converter = DotGraphConverter(str(dot_file))
    
    # Check dependencies
    if not converter.check_graphviz_installed():
        print("ERROR: graphviz not installed!")
        print("\nInstall with:")
        print("  Ubuntu/Debian: sudo apt-get install graphviz")
        print("  macOS:         brew install graphviz")
        print("  Windows:       choco install graphviz")
        sys.exit(1)
    
    print("✓ Graphviz installed\n")
    
    # Validate syntax
    print("Validating DOT syntax...")
    if not converter.validate_dot_syntax():
        print("✗ DOT validation failed")
        sys.exit(1)
    print("✓ DOT syntax valid\n")
    
    # Generate all formats
    print("Generating graph formats...\n")
    outputs = converter.generate_all_formats()
    
    # Print summary
    print("\n")
    converter.print_summary()
    
    if not outputs:
        print("✗ No formats generated!")
        sys.exit(1)
    
    print(f"✓ Generated {len(outputs)} output files")
    print("\nAll files are in:", converter.output_dir)

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nConversion cancelled by user")
        sys.exit(1)
    except Exception as e:
        print(f"\n✗ Fatal error: {e}", file=sys.stderr)
        sys.exit(1)
