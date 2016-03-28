#ifndef __XRDFILECACHE_CACHE_IO_HH__
#define __XRDFILECACHE_CACHE_IO_HH__

#include "XrdFileCache.hh"
#include "XrdOuc/XrdOucCache2.hh"
#include "XrdCl/XrdClDefaultEnv.hh"

namespace XrdFileCache
{
   //----------------------------------------------------------------------------
   //! Base cache-io class that implements XrdOucCacheIO abstract methods.
   //----------------------------------------------------------------------------
   class IO : public XrdOucCacheIO2
   {
      public:
         IO (XrdOucCacheIO2 *io, XrdOucCacheStats &stats, Cache &cache) :
         m_io(io), m_statsGlobal(stats), m_cache(cache) {}

         //! Original data source.
         virtual XrdOucCacheIO *Base() { return m_io; }

         //! Original data source URL.
         virtual long long FSize() { return m_io->FSize(); }

         //! Original data source URL.
         virtual const char *Path() { return m_io->Path(); }

         virtual int Sync() { return 0; }

         virtual int Trunc(long long Offset) { errno = ENOTSUP; return -1; }

         virtual int Write(char *Buffer, long long Offset, int Length)
         { errno = ENOTSUP; return -1; }

      virtual void Update(XrdOucCacheIO2 &iocp) { m_io = &iocp; }

      protected:
         XrdCl::Log* clLog() const { return XrdCl::DefaultEnv::GetLog(); }

         XrdOucCacheIO2   *m_io;          //!< original data source
         XrdOucCacheStats &m_statsGlobal; //!< reference to Cache statistics
         Cache            &m_cache;       //!< reference to Cache needed in detach
   };
}

#endif
