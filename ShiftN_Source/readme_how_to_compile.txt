How to build ShiftN for Windows using Visual Studio 2008:

1. Go to .\zlib\projects\visualc6 and open zlib.sln to build zlib.lib (LIB Release). 

2. Open a Command Prompt window and call "vcvars32.bat" from the Visual Studio's VC\bin directory.

3. In the Command Prompt window, go to .\libjpeg and call "nmake /f Makefile nodebug=1".

4. In the Command Prompt window, go to .\libtiff and call "nmake /f Makefile.vc nodebug=1".

5. Build the files shiftn_english.dll, shiftn_deutsch.dll, shiftn_francais.dll and shiftn_espanol.dll (Release) using the .sln in the respective subfolder.

6. Open "ShiftN.sln" to build "ShiftN.exe" (Release).

7. In order to run the executable, you need to copy the following files to the same directory:
ShiftN.exe
shiftn_english.dll
shiftn_deutsch.dll
shiftn_francais.dll
shiftn_espanol.dll

The following files from Visual Studio's VC\redist\x86 directory are also needed:
mfc90.dll
mfcm90.dll
Microsoft.VC90.CRT.manifest
Microsoft.VC90.MFC.manifest
msvcm90.dll
msvcp90.dll
msvcr90.dll
