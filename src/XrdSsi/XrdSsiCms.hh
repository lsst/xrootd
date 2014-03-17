#ifndef __XRDSSICMS_H__
#define __XRDSSICMS_H__
/******************************************************************************/
/*                                                                            */
/*                          X r d S s i C m s . h h                           */
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

#include "XrdCms/XrdCmsClient.hh"
#include "XrdSsi/XrdSsiCluster.hh"


class XrdSsiCms : public XrdSsiCluster
{
public:

        void   Added(const char *name, bool pend=false) {};

XrdOucTList   *Managers() {return 0;}

        void   Removed(const char *name) {};

        void   Resume (bool perm=true) {}

        void   Suspend(bool perm=true) {}

        int    Resource(int n)   {return 0;}

        int    Reserve (int n=1) {return 0;}

        int    Release (int n=1) {return 0;}

               XrdSsiCms(XrdCmsClient *cmsP) : theCms(cmsP) {}
virtual       ~XrdSsiCms() {}

private:

XrdCmsClient *theCms;
};
#endif
