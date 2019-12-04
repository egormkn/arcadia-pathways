
bin_PROGRAMS = $(top_builddir)/bin/arcadia

__top_builddir__bin_arcadia_SOURCES = \
		arcadia/graphwindow.cpp \
		arcadia/graphwindow.h \
		arcadia/graphcontroller.cpp \
		arcadia/graphcontroller.h \
		arcadia/graphmodel.cpp \
		arcadia/graphmodel.h \
		arcadia/graphloader.cpp \
		arcadia/graphloader.h \
		arcadia/defaultgraphloader.cpp \
		arcadia/defaultgraphloader.h \
		arcadia/graphvizgraphloader.cpp \
		arcadia/graphvizgraphloader.h \
		arcadia/graphlayout.cpp \
		arcadia/graphlayout.h \
		arcadia/content.cpp \
		arcadia/content.h \
		arcadia/containercontent.cpp \
		arcadia/containercontent.h \
		arcadia/clonecontent.cpp \
		arcadia/clonecontent.h \
		arcadia/connector.cpp \
		arcadia/connector.h \
		arcadia/contentlayoutmanager.cpp \
		arcadia/contentlayoutmanager.h \
		arcadia/squarecontentlayoutmanager.cpp \
		arcadia/squarecontentlayoutmanager.h \
		arcadia/graphvizcontentlayoutmanager.cpp \
		arcadia/graphvizcontentlayoutmanager.h \
		arcadia/connectorlayoutmanager.cpp \
		arcadia/connectorlayoutmanager.h \
		arcadia/stylesheet.cpp \
		arcadia/stylesheet.h \
		arcadia/edgestyle.cpp \
		arcadia/edgestyle.h \
		arcadia/vertexstyle.cpp \
		arcadia/vertexstyle.h \
		arcadia/containerstyle.cpp \
		arcadia/containerstyle.h \
		arcadia/branchcontainerstyle.cpp \
		arcadia/branchcontainerstyle.h \
		arcadia/clonecontainerstyle.cpp \
		arcadia/clonecontainerstyle.h \
		arcadia/trianglecontainerstyle.cpp \
		arcadia/trianglecontainerstyle.h \
		arcadia/edgeproperty.cpp \
		arcadia/edgeproperty.h \
		arcadia/vertexproperty.cpp \
		arcadia/vertexproperty.h \
		arcadia/graphview.cpp \
		arcadia/graphview.h \
		arcadia/modelgraphview.cpp \
		arcadia/modelgraphview.h \
		arcadia/listgraphview.cpp \
		arcadia/listgraphview.h \
		arcadia/propertygraphview.cpp \
		arcadia/propertygraphview.h \
		arcadia/layoutgraphview.cpp \
		arcadia/layoutgraphview.h \
		arcadia/edgegraphics.cpp \
		arcadia/edgegraphics.h \
		arcadia/vertexgraphics.cpp \
		arcadia/vertexgraphics.h \
		arcadia/containergraphics.cpp \
		arcadia/containergraphics.h \
		pathways/pathwaygraphwindow.cpp \
		pathways/pathwaygraphwindow.h \
		pathways/pathwaygraphcontroller.cpp \
		pathways/pathwaygraphcontroller.h \
		pathways/pathwaygraphmodel.cpp \
		pathways/pathwaygraphmodel.h \
		pathways/sbmlgraphloader.cpp \
		pathways/sbmlgraphloader.h \
		pathways/compartmentcontainer.cpp \
		pathways/compartmentcontainer.h \
		pathways/pathwaystylesheet.cpp \
		pathways/pathwaystylesheet.h \
		pathways/reactionvertexstyle.cpp \
		pathways/reactionvertexstyle.h \
		pathways/speciesvertexstyle.cpp \
		pathways/speciesvertexstyle.h \
		pathways/modifieredgestyle.cpp \
		pathways/modifieredgestyle.h \
		pathways/productedgestyle.cpp \
		pathways/productedgestyle.h \
		pathways/reactantedgestyle.cpp \
		pathways/reactantedgestyle.h \
		pathways/compartmentcontainerstyle.cpp \
		pathways/compartmentcontainerstyle.h \
		pathways/pathwayedgeproperty.cpp \
		pathways/pathwayedgeproperty.h \
		pathways/modifieredgeproperty.cpp \
		pathways/modifieredgeproperty.h \
		pathways/productedgeproperty.cpp \
		pathways/productedgeproperty.h \
		pathways/reactantedgeproperty.cpp \
		pathways/reactantedgeproperty.h \
		pathways/pathwayvertexproperty.cpp \
		pathways/pathwayvertexproperty.h \
		pathways/speciesvertexproperty.cpp \
		pathways/speciesvertexproperty.h \
		pathways/reactionvertexproperty.cpp \
		pathways/reactionvertexproperty.h \
		pathways/ontologycontainer.cpp \
		pathways/ontologycontainer.h \
		main.cpp \
		.moc/moc_graphwindow.cpp \
		.moc/moc_modelgraphview.cpp \
		.moc/moc_listgraphview.cpp \
		.moc/moc_layoutgraphview.cpp

AM_CPPFLAGS = \
	-I$(srcdir)/../lib \
	-I$(srcdir)/arcadia \
	-I$(srcdir)/pathways \
	-I$(srcdir)/.moc \
	$(QT4_CFLAGS) \
	$(SBML_CFLAGS) \
	$(GVC_CFLAGS) \
	$(GRAPH_CFLAGS)
#	$(PTHREAD_CFLAGS)

LDADD = \
	$(top_builddir)/lib/libavoid.la \
	$(top_builddir)/lib/libtwine.la \
	$(SBML_LIBS) \
	$(GVC_LIBS) \
	$(QT4_LIBS) \
	-lpthread

## The correct place for these is in arcadia_LDADD. See:
## http://stackoverflow.com/questions/13610572/correcting-the-gcc-command-line-ordering-using-automake
##arcadiapathways_LDFLAGS = \
	$(SBML_LIBS) \
	$(GVC_LIBS) \
	$(GRAPH_LIBS) \
	$(QT4_LIBS) \
	-lpthread
#	$(PTHREAD_LIBS)


.moc/moc_%.cpp: arcadia/%.h | .moc
	$(MOC) $(DEFS) $(arcadiapathways_CXXFLAGS) $< -o $@

.moc:
	mkdir .moc

clean-local:
	-rm -rf $(srcdir)/.moc/*

#moc_%.cpp: %.h
#	$(MOC) $(DEFS) $(arcadiapathways_CXXFLAGS) $< -o $@
