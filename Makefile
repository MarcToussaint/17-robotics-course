BASE = .

target: src
NAME   = $(shell basename $(PWD))

################################################################################

src_paths =  $(shell find src -mindepth 1 -maxdepth 1 -type d -not -name 'extern' -not -name 'CMakeFiles' -not -name 'retired' -printf "%f ")

exa_paths = $(shell find examples -maxdepth 3 -name 'Makefile' -printf "%h ")

pysrc_paths = Core Kin Gui KOMO Optim

test_paths = $(shell find test -maxdepth 3 -name 'Makefile' -printf "%h ")

################################################################################

tests: $(test_paths:%=makePath/%)

srcExa: $(src_paths:%=makeDepend/%) $(exa_paths:%=makePath/%)

src: $(src_paths:%=makeDepend/%)

depend: $(src_paths:%=dependPath/%)

python: $(src_paths:%=makeDepend/%) $(pysrc_paths:%=makePythonPath/%)

clean: $(src_paths:%=cleanPath/%) $(exa_paths:%=cleanPath/%) cleanLocks

cleanStart: force
	@read -p " *** WARNING: This will rm ALL local files/changes (e.g. project/temporary/data files) - abort if you don't want to continue" yn
	git clean -f -d -x
	cp build/config.mk.default build/config.mk
	bin/createMakefileLinks.sh

cleanCmake:
	@find . -type f \( -name 'CMakeCache.txt' -or -name 'CTestTestfile.cmake' -or -name 'cmake_install.cmake' \) -print -delete
	@find . -type d \( -name 'CMakeFiles' \) -exec rm -Rf {} \;

################################################################################

# test: setConfigFlag $(exa_paths:%=cleanPath/%) cleanLocks $(exa_paths:%=makePath/%)

# setConfigFlag: force
# 	echo "MLR_TESTS = 1" > build/z.mk

runTests: tests
	@rm -f z.test-report
	@find test -mindepth 2 -maxdepth 2 -type d \
		-not -name '*py' -not -name 'CMakeFiles'\
		-exec build/run-path.sh {} \;

#testLibs:
#	@find examples -mindepth 2 -maxdepth 2 -type d \
# 	      -not -name '*py' -not -name 'CMakeFiles'\
#	      -exec $(BASE)/build/make-path.sh {} lib_test.so \;

################################################################################

doc:
	cd doc; doxygen MLR.doxy;
#	$(MAKE) -w -C doc guide doxy

reformatSources:
	astyle --options=src/style.astyle "src/MT/*.h" "src/MT/*.cpp" "src/MT/*.cxx"
	cd src; find MT/ \( -name "*.h" -or -name "*.cpp" -or -name "*.cxx" \) -exec ./style.sed.sh {} \;

%.tgz: force
	-mv -f build/config.mk build/config.mk.mine
	cp -f slices/$*.config.mk ./build/config.mk
	cp -f slices/$*.README.md README.md
	cp -f ../README.md README.mlr.md
	cp -f ../COPYING .
	cp -f ../install/INSTALL_ALL_UBUNTU_PACKAGES.sh .
	rm -f $*.tgz;
	cd ..; \
	tar cvzfh $*.tgz --transform "s/^share/$*/" --exclude-vcs \
	--files-from $(PWD)/slices/default.incl \
	--files-from $(PWD)/slices/$*.incl \
	--exclude-from $(PWD)/slices/default.excl \
	--exclude-from $(PWD)/slices/$*.excl
	cp -f build/config.mk.mine build/config.mk
	rm -f README.md README.mlr.md COPYING INSTALL_ALL_UBUNTU_PACKAGES.sh
	rm -Rf z.$*; mkdir z.$*; cd z.$*; tar xzf ~/git/mlr/$*.tgz

force:

################################################################################

include build/generic.mk
