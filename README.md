# EXE launcher for Python 3 scripts

Small EXE stub which launches a python script. Supposed to be used with the embeddable Python distribution.

## How-To

Choose one of `launchw.exe` for GUI applications or `launchc.exe` for console applications. Rename as desired (e.g. `app.exe`).

A launcher named `app.exe` will search for a python script to run in the following locations (relative to the containing directory):

* `app.py`
* `app.pyc`
* `app.pyz` (a ZIP file, see [zipapp](https://docs.python.org/3/library/zipapp.html))
* `app/__main__.py`
* `app/__main__.pyc`

The embedded python distribution (more specifically, `python3.dll`) must be placed into

* the same directory as the launcher exe, or
* a subdirectory `python3/`, or
* a subdirectory `3rdparty/python3/`

## Caveats

* `sys.executable` will be the launcher EXE, and not a conventional Python interpreter.
  Your code and its dependencies need to be prepared for this possibility. For example,
  if your application uses the `multiprocessing` module, it will need to call
  `multiprocessing.set_executable()` to let the module know where to find the standard
  Python interpreter.
* The launcher EXE has an embedded manifest which declares compatibility with Windows 7
  through Windows 10, never requests elevation and enables commctl32 V6 (i.e. Visual Styles).
  This is the same as the original Python 3.8 interpreter, but it might not be what later
  versions of Python use and might therefore lead to subtle problems in the future.

## License

Zlib license. No copyright notice is required when shipping a compiled binary.

## Similar Work

[python-embedded-launcher](https://github.com/zsquareplusc/python-embedded-launcher) also supports merging the python script into the EXE file.
