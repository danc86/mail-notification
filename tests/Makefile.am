tests = \
	test-sgml-ref.stamp			\
	test-uri.stamp				\
	test-vfs-read-line1-smallblock.output	\
	test-vfs-read-line1-largeblock.output	\
	test-vfs-read-line2-smallblock.output	\
	test-vfs-read-line2-largeblock.output

noinst_PROGRAMS = \
	test-sgml-ref			\
	test-uri			\
	test-vfs-read-line-smallblock	\
	test-vfs-read-line-largeblock
noinst_DATA = $(tests)

test_sgml_ref_SOURCES = \
	$(top_srcdir)/src/mn-sgml-ref.c	\
	test-sgml-ref.c

test_uri_SOURCES = \
	$(top_srcdir)/src/mn-uri.c	\
	test-uri.c

test_vfs_read_line_sources = \
	$(top_srcdir)/src/mn-vfs.c	\
	$(top_srcdir)/src/mn-vfs.h	\
	test-vfs-read-line.c

test_vfs_read_line_smallblock_SOURCES = $(test_vfs_read_line_sources)
test_vfs_read_line_smallblock_CPPFLAGS = $(AM_CPPFLAGS) \
	-DREAD_LINE_BLOCK_SIZE=4

test_vfs_read_line_largeblock_SOURCES = $(test_vfs_read_line_sources)
test_vfs_read_line_largeblock_CPPFLAGS = $(AM_CPPFLAGS) \
	-DREAD_LINE_BLOCK_SIZE=16384

AM_LDFLAGS = $(GNOME_LIBS)
AM_CPPFLAGS = $(WARN_CFLAGS) $(G_ASSERTIONS) $(GNOME_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-DMN_REGRESSION_TEST

test-sgml-ref.stamp: test-sgml-ref
	./test-sgml-ref
	@touch test-sgml-ref.stamp

test-uri.stamp: test-uri
	./test-uri
	@touch test-uri.stamp

test-vfs-read-line1-smallblock.output: test-vfs-read-line-smallblock test-vfs-read-line1.input test-vfs-read-line1.expected
	./test-vfs-read-line-smallblock file://`pwd`/test-vfs-read-line1.input > test-vfs-read-line1-smallblock.output
	@if cmp test-vfs-read-line1-smallblock.output test-vfs-read-line1.expected > /dev/null; then echo "+++ VFS-READ-LINE TEST 1-SMALLBLOCK PASSED"; else echo "--- VFS-READ-LINE TEST 1-SMALLBLOCK FAILED"; exit 1; fi

test-vfs-read-line1-largeblock.output: test-vfs-read-line-largeblock test-vfs-read-line1.input test-vfs-read-line1.expected
	./test-vfs-read-line-largeblock file://`pwd`/test-vfs-read-line1.input > test-vfs-read-line1-largeblock.output
	@if cmp test-vfs-read-line1-largeblock.output test-vfs-read-line1.expected > /dev/null; then echo "+++ VFS-READ-LINE TEST 1-LARGEBLOCK PASSED"; else echo "--- VFS-READ-LINE TEST 1-LARGEBLOCK FAILED"; exit 1; fi

test-vfs-read-line2-smallblock.output: test-vfs-read-line-smallblock test-vfs-read-line2.input test-vfs-read-line2.expected
	./test-vfs-read-line-smallblock file://`pwd`/test-vfs-read-line2.input > test-vfs-read-line2-smallblock.output
	@if cmp test-vfs-read-line2-smallblock.output test-vfs-read-line2.expected > /dev/null; then echo "+++ VFS-READ-LINE TEST 2-SMALLBLOCK PASSED"; else echo "--- VFS-READ-LINE TEST 2-SMALLBLOCK FAILED"; exit 1; fi

test-vfs-read-line2-largeblock.output: test-vfs-read-line-largeblock test-vfs-read-line2.input test-vfs-read-line2.expected
	./test-vfs-read-line-largeblock file://`pwd`/test-vfs-read-line2.input > test-vfs-read-line2-largeblock.output
	@if cmp test-vfs-read-line2-largeblock.output test-vfs-read-line2.expected > /dev/null; then echo "+++ VFS-READ-LINE TEST 2-LARGEBLOCK PASSED"; else echo "--- VFS-READ-LINE TEST 2-LARGEBLOCK FAILED"; exit 1; fi

MOSTLYCLEANFILES = $(tests)

EXTRA_DIST = \
	test-vfs-read-line1.input		\
	test-vfs-read-line1.expected		\
	test-vfs-read-line2.input		\
	test-vfs-read-line2.expected
