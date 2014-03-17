
include( XRootDCommon )

#-------------------------------------------------------------------------------
# Shared library version
#-------------------------------------------------------------------------------
set( XRD_SSI_VERSION   1.0.0 )
set( XRD_SSI_SOVERSION 1 )

#-------------------------------------------------------------------------------
# The XrdSsi library
#-------------------------------------------------------------------------------
add_library(
  XrdSsi
  SHARED
XrdSsi/XrdSsiCallBack.hh
XrdSsi/XrdSsiCluster.hh
XrdSsi/XrdSsiCms.cc                    XrdSsi/XrdSsiCms.hh
XrdSsi/XrdSsiEndPReal.cc               XrdSsi/XrdSsiEndPReal.hh
XrdSsi/XrdSsiEndPoint.cc               XrdSsi/XrdSsiEndPoint.hh
XrdSsi/XrdSsiFile.cc                   XrdSsi/XrdSsiFile.hh
XrdSsi/XrdSsiFileReq.cc                XrdSsi/XrdSsiFileReq.hh
XrdSsi/XrdSsiManager.cc                XrdSsi/XrdSsiManager.hh
                                       XrdSsi/XrdSsiRequest.hh
XrdSsi/XrdSsiResponse.cc               XrdSsi/XrdSsiResponse.hh
                                       XrdSsi/XrdSsiService.hh
                                       XrdSsi/XrdSsiSession.hh
XrdSsi/XrdSsiSfs.cc                    XrdSsi/XrdSsiSfs.hh
XrdSsi/XrdSsiSfsConfig.cc              XrdSsi/XrdSsiSfsConfig.hh
                                       XrdSsi/XrdSsiStream.hh
                                       XrdSsi/XrdSsiTrace.hh)

target_link_libraries(
  XrdSsi
  XrdClient
  XrdUtils
  pthread )

set_target_properties(
  XrdSsi
  PROPERTIES
  VERSION   ${XRD_SSI_VERSION}
  SOVERSION ${XRD_SSI_SOVERSION}
  LINK_INTERFACE_LIBRARIES "" )

#-------------------------------------------------------------------------------
# Install
#-------------------------------------------------------------------------------
install(
  TARGETS XrdSsi
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
