dnl Autoconf macros for cvl

# Taken from gnutls and modified for cvl -- Martin Lambers
# Modified for LIBGNUTLS -- nmav
# Configure paths for LIBGCRYPT
# Shamelessly stolen from the one of XDELTA by Owen Taylor
# Werner Koch   99-12-09

dnl AM_PATH_CVL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test for libcv, and define CVL_CFLAGS and CVL_LIBS
dnl
AC_DEFUN([AM_PATH_CVL],
[dnl
dnl Get the cflags and libraries from the cvl-config script
dnl
AC_ARG_WITH(cvl-prefix,
          [  --with-cvl-prefix=PFX   Prefix where cvl is installed (optional)],
          cvl_config_prefix="$withval", cvl_config_prefix="")

  if test x$cvl_config_prefix != x ; then
     if test x${CVL_CONFIG+set} != xset ; then
        CVL_CONFIG=$cvl_config_prefix/bin/cvl-config
     fi
  fi

  AC_PATH_PROG(CVL_CONFIG, cvl-config, no)
  min_cvl_version=ifelse([$1], ,0.0.0,$1)
  AC_MSG_CHECKING(for cvl - version >= $min_cvl_version)
  no_cvl=""
  if test "$CVL_CONFIG" = "no" ; then
    no_cvl=yes
  else
    CVL_CFLAGS=`$CVL_CONFIG $cvl_config_args --cflags`
    CVL_LIBS=`$CVL_CONFIG $cvl_config_args --libs`
    cvl_config_version=`$CVL_CONFIG $cvl_config_args --version`


      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $CVL_CFLAGS"
      LIBS="$LIBS $CVL_LIBS"
dnl
dnl Now check if the installed cvl is sufficiently new. Also sanity
dnl checks the results of cvl-config to some extent
dnl
      rm -f conf.cvltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cvl/cvl.h>

int
main ()
{
    system ("touch conf.cvltest");

    if( strcmp( cvl_check_version(NULL), "$cvl_config_version" ) )
    {
      printf("\n*** 'cvl-config --version' returned %s, but CVL (%s)\n",
             "$cvl_config_version", cvl_check_version(NULL) );
      printf("*** was found! If cvl-config was correct, then it is best\n");
      printf("*** to remove the old version of CVL. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If cvl-config was wrong, set the environment variable CVL_CONFIG\n");
      printf("*** to point to the correct copy of cvl-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    }
    else if ( strcmp(cvl_check_version(NULL), CVL_VERSION ) )
    {
      printf("\n*** CVL header file (version %s) does not match\n", CVL_VERSION);
      printf("*** library (version %s)\n", cvl_check_version(NULL) );
    }
    else
    {
      if ( cvl_check_version( "$min_cvl_version" ) )
      {
        return 0;
      }
     else
      {
        printf("no\n*** An old version of CVL (%s) was found.\n",
                cvl_check_version(NULL) );
        printf("*** You need a version of CVL newer than %s. The latest version of\n",
               "$min_cvl_version" );
        printf("*** CVL is always available from http://cvtool.sourceforge.net/.\n");
        printf("*** \n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the cvl-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of CVL, but you can also set the CVL_CONFIG environment to point to the\n");
        printf("*** correct copy of cvl-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_cvl=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_cvl" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])
  else
     if test -f conf.cvltest ; then
        :
     else
        AC_MSG_RESULT(no)
     fi
     if test "$CVL_CONFIG" = "no" ; then
       echo "*** The cvl-config script installed by CVL could not be found"
       echo "*** If CVL was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the CVL_CONFIG environment variable to the"
       echo "*** full path to cvl-config."
     else
       if test -f conf.cvltest ; then
        :
       else
          echo "*** Could not run cvl test program, checking why..."
          CFLAGS="$CFLAGS $CVL_CFLAGS"
          LIBS="$LIBS $CVL_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cvl/cvl.h>
],      [ return !!cvl_check_version(NULL); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding CVL or finding the wrong"
          echo "*** version of CVL. If it is not finding CVL, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
          echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means CVL was incorrectly installed"
          echo "*** or that you have moved CVL since it was installed. In the latter case, you"
          echo "*** may want to edit the cvl-config script: $CVL_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     CVL_CFLAGS=""
     CVL_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  rm -f conf.cvltest
  AC_SUBST(CVL_CFLAGS)
  AC_SUBST(CVL_LIBS)
])
dnl *-*wedit:notab*-*  Please keep this as the last line.
