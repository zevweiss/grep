@echo off
echo Configuring GNU Grep for DJGPP v2.x...

Rem The SmallEnv tests protect against fixed and too small size
Rem of the environment in stock DOS shell.

Rem Find out where the sources are
set XSRC=.
if not "%XSRC%" == "." goto SmallEnv
if "%1" == "" goto InPlace
set XSRC=%1
if not "%XSRC%" == "%1" goto SmallEnv
redir -e /dev/null update %XSRC%/configure.orig ./configure
if not exist configure update %XSRC%/configure ./configure

:InPlace
Rem Edit configuration files due to DOS-specific problems
echo Updating configuration scripts...
if not exist configure.orig update configure configure.orig
sed -f %XSRC%/djgpp/config.sed configure.orig > configure
if errorlevel 1 goto SedError

Rem Make sure they have a config.site file
set CONFIG_SITE=%XSRC%/djgpp/config.site
if not "%CONFIG_SITE%" == "%XSRC%/djgpp/config.site" goto SmallEnv

Rem Make sure crucial file names are not munged by unpacking
test -f %XSRC%/intl/po2tbl.sed.in
if not errorlevel 1 mv -f %XSRC%/intl/po2tbl.sed.in %XSRC%/intl/po2tblsed.in
test -f %XSRC%/intl/po2tblsed.in
if errorlevel 1 mv -f %XSRC%/intl/po2tbl.sed %XSRC%/intl/po2tblsed.in
test -f %XSRC%/po/Makefile.in.in
if not errorlevel 1 mv -f %XSRC%/po/Makefile.in.in %XSRC%/po/Makefile.in-in

Rem This is required because DOS/Windows are case-insensitive
Rem to file names, and "make install" will do nothing if Make
Rem finds a file called `install'.
if exist INSTALL mv -f INSTALL INSTALL.txt

Rem install-sh is required by the configure script but clashes with the
Rem various Makefile install-foo targets, so we MUST have it before the
Rem script runs and rename it afterwards
test -f %XSRC%/install-sh
if not errorlevel 1 goto NoRen0
test -f %XSRC%/install-sh.sh
if not errorlevel 1 mv -f %XSRC%/install-sh.sh %XSRC%/install-sh
:NoRen0

Rem Set HOSTNAME so it shows in config.status
if not "%HOSTNAME%" == "" goto hostdone
if "%windir%" == "" goto msdos
set OS=MS-Windows
if not "%OS%" == "MS-Windows" goto SmallEnv
goto haveos
:msdos
set OS=MS-DOS
if not "%OS%" == "MS-DOS" goto SmallEnv
:haveos
if not "%USERNAME%" == "" goto haveuname
if not "%USER%" == "" goto haveuser
echo No USERNAME and no USER found in the environment, using default values
set HOSTNAME=Unknown PC
if not "%HOSTNAME%" == "Unknown PC" goto SmallEnv
goto userdone
:haveuser
set HOSTNAME=%USER%'s PC
if not "%HOSTNAME%" == "%USER%'s PC" goto SmallEnv
goto userdone
:haveuname
set HOSTNAME=%USERNAME%'s PC
if not "%HOSTNAME%" == "%USERNAME%'s PC" goto SmallEnv
:userdone
set _HOSTNAME=%HOSTNAME%, %OS%
if not "%_HOSTNAME%" == "%HOSTNAME%, %OS%" goto SmallEnv
set HOSTNAME=%_HOSTNAME%
:hostdone
set _HOSTNAME=
set OS=

echo Running the ./configure script...
sh ./configure --src=%XSRC% --disable-nls
if errorlevel 1 goto CfgError
echo Done.
goto End

:CfgError
echo ./configure script exited abnormally!
goto End

:SmallEnv
echo Your environment size is too small.  Enlarge it and run me again.
echo Configuration NOT done!

:End
test -f %XSRC%/install-sh.sh
if not errorlevel 1 goto NoRen1
test -f %XSRC%/install-sh
if not errorlevel 1 mv -f %XSRC%/install-sh %XSRC%/install-sh.sh
:NoRen1
set CONFIG_SITE=
set HOSTNAME=
set XSRC=
