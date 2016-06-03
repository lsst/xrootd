//----------------------------------------------------------------------------------
// Copyright (c) 2014 by Board of Trustees of the Leland Stanford, Jr., University
// Author: Alja Mrak-Tadel, Matevz Tadel, Brian Bockelman
//----------------------------------------------------------------------------------
// XRootD is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// XRootD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with XRootD.  If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------------

#include <stdio.h>
#include <fcntl.h>

#include "XrdSys/XrdSysError.hh"
#include "XrdSfs/XrdSfsInterface.hh"
#include "XrdSys/XrdSysPthread.hh"

#include "XrdFileCacheIOEntireFile.hh"
#include "XrdFileCacheStats.hh"
#include "XrdFileCacheTrace.hh"

#include "XrdOuc/XrdOucEnv.hh"

using namespace XrdFileCache;

//______________________________________________________________________________


IOEntireFile::IOEntireFile(XrdOucCacheIO2 *io, XrdOucCacheStats &stats, Cache & cache)
   : IO(io, stats, cache),
     m_file(0),
     m_localStat(0)
{
   XrdCl::URL url(m_io->Path());
   std::string fname = Cache::GetInstance().RefConfiguration().m_cache_dir + url.GetPath();

   if (!(m_file = Cache::GetInstance().GetFileWithLocalPath(fname, this)))
   {
      struct stat st;
      Fstat(st);
      m_file = new File(io, fname, 0, st.st_size);
      Cache::GetInstance().AddActive(this, m_file);
   }
}



IOEntireFile::~IOEntireFile()
{
   delete m_localStat;
}

int IOEntireFile::Fstat(struct stat &sbuff)
{
   XrdCl::URL url(m_io->Path());
   std::string name = url.GetPath();
   name += ".cinfo";

   struct stat* ls = getValidLocalStat(name.c_str());
   if (ls) {
      memcpy(&sbuff, ls, sizeof(struct stat));
      return 0;
   }
   else {
      return m_io->Fstat(sbuff);
   }
}

void IOEntireFile::RelinquishFile(File* f)
{
   assert(m_file == f);
   m_file = 0;
}


struct stat* IOEntireFile::getValidLocalStat(const char* path)
{
   if (!m_localStat) {
      struct stat tmpStat;
      if (m_cache.GetOss()->Stat(path, &tmpStat) == XrdOssOK) {
         XrdOssDF* infoFile = m_cache.GetOss()->newFile(Cache::GetInstance().RefConfiguration().m_username.c_str()); 
         XrdOucEnv myEnv; 
         int res = infoFile->Open(path, O_RDONLY, 0600, myEnv);
         if (res >= 0) {
            Info info(m_cache.GetTrace());
            if (info.Read(infoFile) > 0) {
               tmpStat.st_size = info.GetFileSize();
               m_localStat = new struct stat;
               memcpy(m_localStat, &tmpStat, sizeof(struct stat));
            }
         }
         infoFile->Close();
         delete infoFile;
      }
   }

   return m_localStat;   
}

bool IOEntireFile::ioActive()
{
   if (!m_file)
      return false;
   else
      return m_file->InitiateClose();
}

XrdOucCacheIO *IOEntireFile::Detach()
{
   XrdOucCacheIO * io = m_io;

   // This will delete us!
   m_cache.Detach(this);
   return io;
}

void IOEntireFile::Read (XrdOucCacheIOCB &iocb, char *buff, long long offs, int rlen)
{
   iocb.Done(IOEntireFile::Read(buff, offs, rlen));
}

int IOEntireFile::Read (char *buff, long long off, int size)
{
   TRACEIO(Dump, "IOEntireFile::Read() "<< this << " off: " << off << " size: " << size );
   if (off < 0)
   {
      errno = EINVAL;
      return -1;
   }
   ssize_t bytes_read = 0;
   ssize_t retval = 0;

   retval = m_file->Read(buff, off, size);
   if (retval >= 0)
   {
      bytes_read += retval;
      buff += retval;
      size -= retval;

      if (size > 0)
          TRACEIO(Warning, "IOEntireFile::Read() bytes missed " <<  size );
   }      
   else
   {
       TRACEIO(Warning, "IOEntireFile::Read() pass to origin bytes ret " << retval );
   }

   return (retval < 0) ? retval : bytes_read;
}


/*
 * Perform a readv from the cache
 */
int IOEntireFile::ReadV (const XrdOucIOVec *readV, int n)
{
   TRACE(Dump, "IO::ReadV(), get " <<  n << " requests,  " << m_io->Path());
   return m_file->ReadV(readV, n);
}
