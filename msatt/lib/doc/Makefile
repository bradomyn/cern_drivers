all: docs

docs: Doxyfile.patch
	doxygen -s -g
	patch -p0 < Doxyfile.patch
	doxygen 2>&1

clean cleanall:
	rm -rf html Doxyfile

# empty install rule
install:
