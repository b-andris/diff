# diff
A simple commandline diff-tool

## Compilation
While I use Xcode to write diff all of Xcode's metadata is unnecessary to compile it. Simply compiling diff/main.c and diff/diff.c (Both of which rely on diff/diff.h) should work on virtually any System. diff should run and function correctly on most UNIX like systems.

## Usage
If diff is properly installed on your system:
```
diff /path/to/file/1 /path/to/file/2
```
otherwise:
```
/path/to/diff /path/to/file/1 /path/to/file/2
```

## License
diff is available under the MIT license:
```
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
