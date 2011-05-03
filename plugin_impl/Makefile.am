
source_cpp =    $(top_srcdir)/src/pdf_thread.cpp                   \
                $(top_srcdir)/src/pdf_anchor.cpp                   \
		$(top_srcdir)/src/pdf_doc.cpp                      \
                $(top_srcdir)/src/pdf_doc_controller.cpp           \
                $(top_srcdir)/src/pdf_library.cpp                  \
                $(top_srcdir)/src/pdf_toc.cpp                      \
                $(top_srcdir)/src/pdf_page.cpp                     \
                $(top_srcdir)/src/pdf_prerender_policy.cpp         \
                $(top_srcdir)/src/pdf_renderer.cpp                 \
                $(top_srcdir)/src/pdf_render_requests.cpp          \
                $(top_srcdir)/src/pdf_searcher.cpp                 \
                $(top_srcdir)/src/pdf_search_task.cpp              \
                $(top_srcdir)/src/pdf_render_task.cpp              \
                $(top_srcdir)/src/pdf_pages_cache.cpp              \
		$(top_srcdir)/goo/GooString.cc                     \
		$(top_srcdir)/goo/GooList.cc                       \
		$(top_srcdir)/goo/gmem.cc                          \
		$(top_srcdir)/src/ipc.c                            \
                collection_impl.cpp                                \
                export_impl.cpp                                    \
                listeners.cpp                                      \
                render_settings_impl.cpp                           \
                string_impl.cpp                                    \
                view_impl.cpp                                      \
                document_impl.cpp                                  \
                library_impl.cpp                                   \
                render_result_impl.cpp                             \
                search_criteria_impl.cpp                           \
                interfaces_utils.cpp                               \
                marker_entry_impl.cpp
 
INCLUDES =  -I$(top_srcdir)/interfaces -I $(top_srcdir) -I$(top_srcdir)/inc @ddjvuapi_CFLAGS@ @DEPS_CFLAGS@

DATADIR=$(datadir)

AM_CFLAGS = -DDATADIR=\"$(datadir)\"


AM_CPPFLAGS = -I$(top_srcdir)/interfaces -I$(top_srcdir)/inc 

CXXFLAGS = -Wall -Werror -I$(top_srcdir)/interfaces -I$(top_srcdir)/common -I$(top_srcdir)/inc -DLOCALE_DIR=\"$(localedir)\"  -DDATA_DIR=\"$(pkgdatadir)\" -DGCC=1


AM_CFLAGS += -Wall 

AM_LDFLAGS = @DEPS_LIBS@ @ddjvuapi_LIBS@
 
plugin_LTLIBRARIES                = libuds-plugin-@PACKAGE_NAME@.la
plugindir = $(uds_plugin_dir)

libuds_plugin_@PACKAGE_NAME@_la_SOURCES     = $(source_cpp)
libuds_plugin_@PACKAGE_NAME@_la_LIBADD      = @DEPS_LIBS@ @ddjvuapi_LIBS@
libuds_plugin_@PACKAGE_NAME@_la_LDFLAGS     = -avoid-version -Wl,--no-undefined -Wl,--version-script=plugin.expmap
libuds_plugin_@PACKAGE_NAME@_la_DEPENDENCIES = plugin.expmap
