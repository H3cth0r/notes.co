import argparse
import os
import sys
from .my_markdown_extension import process_markdown_file


__version__ = "0.1.0"
def show_version(): print(f"notesco version {__version__}")
def main() -> None:
    parser = argparse.ArgumentParser(
            description='Markdown to html converter'
    )

    # create a group for commands that are mutually exclusive
    group = parser.add_mutually_exclusive_group()
    group.add_argument(
        '-v', '--version',
        action='store_true',
        help='Show program version'
    )
    group.add_argument(
        '--help-examples',
        action='store_true',
        help='Show usage examples'
    )

    # main arguments
    parser.add_argument(
        'markdown_file', 
        help='Path to the markdown file (.md)', 
        nargs='?'
    )
    parser.add_argument(
        'html_output', 
        help='Path to the output html file (default: <md_name_file>.html)', 
        nargs='?'
    )

    args = parser.parse_args()

    if args.version:
        show_version()
        return

    if args.help_examples:
        print("""
Usage Examples:
---------------
1. Basic usage:
   notesco input.md output.html

2. No output name, will generate and 'output.md' file:
   notesco input.md

3. Show version:
   notesco --version

4. Show examples:
   notesco --help-examples
        """)
        return

    # check if required arguments are provided
    if not all([args.markdown_file]):
        parser.print_help()
        sys.exit(1)

    # validate markdown extension
    if not args.markdown_file.endswith('.md'):
        print("Error: Input file must have .md extension")
        sys.exit(1)

    if args.html_output and not args.html_output.endswith('.html'):
        print("Error: Output file must have .html extension")
        sys.exit(1)

    # if args.html_output: os.makedirs(args.html_output, exist_ok=True)

    # Apply notesco functionality
    try:
        if args.html_output: 
            process_markdown_file(args.markdown_file, args.html_output)
        else: 
            output_name = args.markdown_file.replace('.md', '.html')
            process_markdown_file(args.markdown_file, output_name)
    except Exception as e:
        print(f"Error generating output file: {str(e)}")
        sys.exit(1)

    
if __name__ == '__main__':
    main() 
