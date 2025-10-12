 Runtime Memory monitoring tool (known as Dumper tool)
 This tool is used to monitor any changes made to Android application's native library
 Initially is was written in C then I re wrote it for efficiency
 Though it was built for Android it works perfectly on linux too.
 To make it work on linux just take direct PID input instead of using getPID() function

 To compile for Android download andorid NDK (any version) from https://developer.android.com/ndk/downloads
 Then simply run ndk-build
 Already compiled binaries are located at libs folder (For Android)
 
 Author : @Rhythm113

