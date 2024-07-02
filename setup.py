import setuptools
from setuptools import Extension, setup
# from Cython.Build import cythonize

setup(
    name='pydvdnav',
    version='0.0.1',
    author='Connor Flanigan',
    author_email='connorflanigan@gmail.com',
    license='MIT',
    packages=setuptools.find_packages(exclude='src'),
    package_dir={'': '.'},
    ext_modules=[
        Extension(
            name="_pydvdnav",  # as it would be imported
            # may include packages/namespaces separated by `.`
            include_dirs=['/usr/include'],
            libraries=['dvdnav'],
            sources=['src/pyDvdNav.cpp', 'src/DvdNav.cpp'],  # all sources are compiled into a single binary file
            extra_compile_args=['-g3', '-std=c++20'],
        ),
    ]
)