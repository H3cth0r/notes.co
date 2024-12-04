from setuptools import setup 
from wrapcco.tools import Extension


markdown_extension = Extension(
    header_file="markdown_to_html.h",  # Header file
    source_file="markdown_to_html.c",  # Source file
    methods_to_include=["process_markdown_file"],            # List of methods (if applicable, otherwise leave empty)
    output_name="my_markdown_extension",  # Output module name
    output_path="./",                  # Path to output generated .c files
    extra_compile_args=["-std=c99"],  # Additional compile arguments
)

# Setup script
setup(
    name="my_markdown_extension",
    version="1.0",
    description="A Python wrapper for converting markdown to HTML.",
    ext_modules=list(markdown_extension),
)
