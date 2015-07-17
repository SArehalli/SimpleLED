from distutils.core import setup, Extension
import os
from buildPythonBindings import module_gen

module_fname = os.path.join("build", "display-binding.cpp")
with open(module_fname, "wt") as file_:
    print("Generating file {}".format(module_fname))
    module_gen(file_)


module1 = Extension('display',
                    include_dirs = ['/usr/include','.'],
                    sources = [module_fname, 'display.cpp'],
                    extra_compile_args=['-std=gnu++0x'],
                    extra_link_args=['gpio.o'])

setup (name = 'Rpi-Display',
       version = '1.0',
       description = 'This is a demo package',
       author = 'Suhas Arehalli',
       author_email = 'sgarehal@ucsd.edu',
       ext_modules = [module1])
