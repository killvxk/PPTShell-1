# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _xdelta3
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


xd3_encode_memory = _xdelta3.xd3_encode_memory
xd3_decode_memory = _xdelta3.xd3_decode_memory
xd3_main_cmdline = _xdelta3.xd3_main_cmdline
XD3_SEC_DJW = _xdelta3.XD3_SEC_DJW
XD3_SEC_FGK = _xdelta3.XD3_SEC_FGK
XD3_SEC_NODATA = _xdelta3.XD3_SEC_NODATA
XD3_SEC_NOINST = _xdelta3.XD3_SEC_NOINST
XD3_SEC_NOADDR = _xdelta3.XD3_SEC_NOADDR
XD3_ADLER32 = _xdelta3.XD3_ADLER32
XD3_ADLER32_NOVER = _xdelta3.XD3_ADLER32_NOVER
XD3_ALT_CODE_TABLE = _xdelta3.XD3_ALT_CODE_TABLE
XD3_NOCOMPRESS = _xdelta3.XD3_NOCOMPRESS
XD3_BEGREEDY = _xdelta3.XD3_BEGREEDY
XD3_COMPLEVEL_SHIFT = _xdelta3.XD3_COMPLEVEL_SHIFT
XD3_COMPLEVEL_MASK = _xdelta3.XD3_COMPLEVEL_MASK
XD3_COMPLEVEL_1 = _xdelta3.XD3_COMPLEVEL_1
XD3_COMPLEVEL_3 = _xdelta3.XD3_COMPLEVEL_3
XD3_COMPLEVEL_6 = _xdelta3.XD3_COMPLEVEL_6
XD3_COMPLEVEL_9 = _xdelta3.XD3_COMPLEVEL_9


