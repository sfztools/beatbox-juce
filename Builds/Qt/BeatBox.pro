TEMPLATE = subdirs
SUBDIRS += SharedCode StandalonePlugin
           SharedCode.subdir        = SharedCode
           StandalonePlugin.subdir  = StandalonePlugin
           StandalonePlugin.depends = SharedCode