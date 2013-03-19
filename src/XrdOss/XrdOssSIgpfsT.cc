/******************************************************************************/
/*                                                                            */
/*                      X r d O s s S I g p f s T . c c                       */
/*                                                                            */
/* (c) 2013 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/*                                                                            */
/* This file is part of the XRootD software suite.                            */
/*                                                                            */
/* XRootD is free software: you can redistribute it and/or modify it under    */
/* the terms of the GNU Lesser General Public License as published by the     */
/* Free Software Foundation, either version 3 of the License, or (at your     */
/* option) any later version.                                                 */
/*                                                                            */
/* XRootD is distributed in the hope that it will be useful, but WITHOUT      */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      */
/* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public       */
/* License for more details.                                                  */
/*                                                                            */
/* You should have received a copy of the GNU Lesser General Public License   */
/* along with XRootD in a file called COPYING.LESSER (LGPL license) and file  */
/* COPYING (GPL license).  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                            */
/* The copyright holder's institutional names and contributor's names may not */
/* be used to endorse or promote products derived from this software without  */
/* specific prior written permission of the institution or contributor.       */
/******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "XrdVersion.hh"
#include "XrdOss/XrdOss.hh"
#include "XrdOss/XrdOssStatInfo.hh"
#include "XrdOuc/XrdOucEnv.hh"
#include "XrdSys/XrdSysError.hh"

//------------------------------------------------------------------------------
//! This file defines a default plug-in that can be used to handle stat()
//! calls for GPFS backed with a tape system. Valid parameters that can be
//! passed to XrdOssStatInfoInit are:
//!
//! <prog>  := cmsd | frm_xfrd | frm_purged | xrootd
//! <role>  := manager | peer | proxy | server | supervisor
//! <token> := stat[.<prog>[.<role>]]={all|online}[&<token>]
//!
//! where: <prog> applies the specification only to programs named <prog>. If
//!               <prog> is not specified, the parametr applies to all programs.
//!        <role> applies the specification only to programs named <prog>
//!               running with the role <role>. If <role> is not specified, the
//!               parameter applies to all <prog>'s regardless of their role.
//!               In all cases, the most restrictive specification applies.
//!        all    allows stat() to return info for online and offline files.
//!        online allows stat() to return info for online files only. If stat()
//!               encounters an offline file is returns ENOENT for the cmsd
//!               and EPERM for other programs. This is the default. Note that
//!               any keyword other than "all" is taken as "online".
//------------------------------------------------------------------------------

extern "C"
{
/******************************************************************************/
/*                 X r d O s s S t a t I n f o R e s O n l y                  */
/******************************************************************************/

//------------------------------------------------------------------------------
//! This global variable is set by XrdOssStatInfoInit to indicate whether we
//! will allow access to all files or only disk resident files. By default,
//! only disk resident files are allowed to be handled via stat(). The value
//! is the errno to be returned when we trip over a non-resident file.
//------------------------------------------------------------------------------

int XrdOssStatInfoResOnly = ENOENT;
  
/******************************************************************************/
/*                        X r d O s s S t a t I n f o                         */
/******************************************************************************/
  
int XrdOssStatInfo(const char *path, struct stat *buff,
                   int         opts, XrdOucEnv   *envP)
{

// Do a regular stat and if it fails return immediately
//
   if (stat(path, buff)) return -1;

// Check if this is an online file, return success
//
   if (buff->st_size == 0 || buff->st_blocks) return 0;

// If caller only wants resident files, then return ENOENT. If we are
// restricting the caller to residdent only files, return the proper errno.
// Otherwise, we indicate the file actually exists.
//
   if (opts & XRDOSS_resonly) errno = ENOENT;
      else errno = XrdOssStatInfoResOnly;
   return (errno ? -1 : 0);
}

/******************************************************************************/
/*                    X r d O s s S t a t I n f o I n i t                     */
/******************************************************************************/

//------------------------------------------------------------------------------
//! The following function is invoked by the plugin manager to obtain the
//! function that is to be used for stat() calls. Valid parameters are:
//!
//! <pgm>   := cmsd | frm_xfrd | frm_purged | xrootd
//! <role>  := manager | peer | proxy | server | supervisor
//! <token> := stat[.<pgm>[.<role>]]={online|all}[&<token>]
//------------------------------------------------------------------------------
  
XrdOssStatInfo_t XrdOssStatInfoInit(XrdOss        *native_oss,
                                    XrdSysLogger  *Logger,
                                    const char    *config_fn,
                                    const char    *parms)
{
   const char *myProg = getenv("XRDPROG");
   const char *myRole = getenv("XRDROLE");
   const char *xWhat  = " only online ";
   XrdSysError Say(Logger, "");
   XrdOucEnv   myEnv(parms);
   char  vChk[512], *val;
   int   n;
   bool expAll[3] = {false, false, false};

// Check for global parms first
//
   strcpy(vChk, "stat");
   if ((val = myEnv.Get(vChk)) && !strcmp(val, "all")) expAll[0] = true;

// Check for program specific variable
//
   if (myProg)
      {strcat(vChk, "."); strcat(vChk, myProg);
       if ((val = myEnv.Get(vChk)) && !strcmp(val, "all")) expAll[1] = true;
          else expAll[1] = expAll[0];
      }   else expAll[1] = expAll[0];

// Check for role specific variable
//
   if (myProg && myRole)
      {strcat(vChk, "."); strcat(vChk, myRole);
       if ((val = myEnv.Get(vChk)) && !strcmp(val, "all")) expAll[2] = true;
          else expAll[2] = expAll[1];
      }   else expAll[2] = expAll[1];

// Now set the global variable indicate whether we will only allow online
// files or all files (i.e. online and offline).
//
        if (expAll[2])                        XrdOssStatInfoResOnly = 0;
   else if (myProg && !strcmp(myProg,"cmsd")) XrdOssStatInfoResOnly = ENOENT;
   else                                       XrdOssStatInfoResOnly = EPERM;

// Record in the log what stat will stat
//
   if (myProg) strncpy(vChk, " for ", 5);
      else *vChk = 0;
   if (!XrdOssStatInfoResOnly) xWhat = " all ";
   Say.Say("Config", " stat() allows", xWhat, "files", vChk);

// Return the stat function
//
    return XrdOssStatInfo;
}
};

/******************************************************************************/
/*                   V e r s i o n   I n f o r m a t i o n                    */
/******************************************************************************/

XrdVERSIONINFO(XrdOssStatInfoInit,Stat-GPFS+TAPE);
