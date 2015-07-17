#! /usr/bin/env python
import sys

from pybindgen import Module, retval, param

def module_gen(name):
    mod = Module('display')
    mod.add_include('"display.h"')
    display = mod.add_class('Display')
    display.add_constructor([param('int', 'height'),
                            param('int', 'width')])
    display.add_method('stop', None, [])
    display.add_method('getHeight', retval('int'), [])
    display.add_method('getWidth', retval('int'), [])
    display.add_method('getValue', retval('int'), 
                                  [param('int', 'row'),
                                   param('int', 'col')])
    display.add_method('setValue', None, [param('int', 'row'),
                                         param('int', 'col'),
                                         param('int', 'color')])
    mod.add_function('loop', retval('int'), [param('Display *', 'disp', 
                                                  transfer_ownership=False)])
    mod.generate(name)

if __name__ == "__main__":
    module_gen()
