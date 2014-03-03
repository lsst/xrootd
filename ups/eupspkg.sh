# EupsPkg config file. Sourced by 'eupspkg'

# Breaks on Darwin w/o this
export LANG=C
	
PKGDIR=$PWD
BUILDDIR=$PWD/../xrootd-build

config()
{
	rm -rf ${BUILDDIR}
	mkdir ${BUILDDIR}
	cd ${BUILDDIR}
	cmake ${PKGDIR} -DCMAKE_INSTALL_PREFIX=${PREFIX} -DENABLE_PERL=FALSE
}

build()
{
	cd ${BUILDDIR}
	default_build
}

install()
{
	cd ${BUILDDIR}
	make -j$NJOBS install

        ARCH=`arch`
        case "${ARCH}" in
            x86_64) ln -s ${PREFIX}/lib64 ${PREFIX}/lib ;;
            *)      echo "Default behaviour for managing lib(64)/ directory" ;;
        esac


	cd ${PKGDIR}
	install_ups
}
