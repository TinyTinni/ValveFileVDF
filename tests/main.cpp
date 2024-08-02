#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include <iostream>

const char *testdata_dir = SOURCE_DIR "/testdata/";

#ifdef _WIN32
#include <direct.h>
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

int main(int argc, char *argv[])
{

    doctest::Context context;
    context.applyCommandLine(argc, argv);

    if (cd(testdata_dir) != 0)
    {
        std::cerr << "Cannot set working directory to " << testdata_dir
                  << std::endl;
        return 1;
    }

    int result = context.run();

    // your clean-up...

    return result;
}
