from setuptools import setup, find_packages, Extension
import glob
import sys

if sys.platform.startswith('linux'):
    pdbpython_libraries = ['TDBAPI_v2']
    pdbpython_library_dirs = ['lib/linux']
else:
    pdbpython_libraries = ['TDB_API_Windows_v2']
    pdbpython_library_dirs = ['lib/x64Release']

setup(
        name = 'tdbpython',
        version = '1.0.2',
        packages = find_packages(exclude=['tests']),
        description = 'TDB client',
        ext_modules = [
            Extension('tdbpython.core', 
                sources = ['tdbpython/core.c'], 
                library_dirs = pdbpython_library_dirs,
                include_dirs=['include'],
                libraries=pdbpython_libraries,
                ),
        ],
        package_data  = {
            'dll': ['x64release/*.dll'],
        },
)
