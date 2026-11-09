--- src/nicksrc/strsubs.c.orig	2026-07-20 20:21:19 UTC
+++ src/nicksrc/strsubs.c
@@ -10,7 +10,7 @@
 #include <errno.h>
 #include <sys/types.h>
 #include <sys/stat.h>
-#include <sys/prctl.h>
+//#include <sys/prctl.h>
 #include <xsearch.h>   
 
 
@@ -227,12 +227,12 @@ void setdump(int cdump) 
 
 void setdump(int cdump) 
 {
-
-  prctl(PR_SET_DUMPABLE, cdump) ; 
-
-
+#ifdef __linux__
+    prctl(PR_SET_DUMPABLE, cdump);
+#else
+    (void) cdump;
+#endif
 }
-
 
 int
 docommand (char *fmt, ...)
