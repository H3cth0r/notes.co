from setuptools import setup, find_packages
from wrapcco.tools import Extension

markdown_extension = Extension(
    header_file="./notesco/c_src/markdown_to_html.h",
    source_file="./notesco/c_src/markdown_to_html.c",
    methods_to_include=["process_markdown_file"],
    output_name="my_markdown_extension",
    output_path="./notesco/c_src/",
    module_name="notesco.",
    extra_compile_args=["-std=c99"],
)

setup(
        name="notesco",
        version="0.1.0",
        packages=find_packages(),
        entry_points={
            'console_scripts': [
                'notesco=notesco:main',
            ],
        },
        package_data={
            'notesco': [
                'c_src/*.c',
                'c_src/*.h',
            ],
        },
        include_package_data=True,
        author="h3cth0r",
        author_email="hector.miranda@zentinel.mx",
        description="Tool to parse Markdown to HTML.",
        long_description=open("./README.md").read(),
        long_description_content_type="text/markdown",
        url="https://github.com/H3cth0r/notes.co",
        classifiers=[
            "Programming Language :: Python :: 3",
            "License :: OSI Approved :: MIT License",
            "Operating System :: OS Independient",
        ],
        python_requires=">=3.6",
        ext_modules=list(markdown_extension),
)
