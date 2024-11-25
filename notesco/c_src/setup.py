from setuptools import setup, Extension

# Define the extension module
markdown_extension = Extension(
    "markdown_extension",
    # sources=["markdown_to_html.c", "markdown_to_html.c"],  # Add all relevant source files
    sources=["markdown_to_html.c"],  # Add all relevant source files
)

# Setup script
setup(
    name="markdown_extension",
    version="1.0",
    description="A Python wrapper for converting markdown to HTML.",
    ext_modules=[markdown_extension],
)
