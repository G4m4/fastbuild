// TestBuildAndLinkLibrary.cpp
//------------------------------------------------------------------------------

// Includes
//------------------------------------------------------------------------------
#include "FBuildTest.h"

#include "Tools/FBuild/FBuildCore/FBuild.h"

#include "Core/FileIO/FileIO.h"
#include "Core/Strings/AStackString.h"

// TestBuildAndLinkLibrary
//------------------------------------------------------------------------------
class TestBuildAndLinkLibrary : public FBuildTest
{
private:
    DECLARE_TESTS

    void TestParseBFF() const;
    void TestBuildLib() const;
    void TestBuildLib_NoRebuild() const;
    void TestLibMerge() const;
    void TestLibMerge_NoRebuild() const;

    const char * GetBuildLibDBFileName() const { return "../../../../tmp/Test/BuildAndLinkLibrary/buildlib.fdb"; }
    const char * GetMergeLibDBFileName() const { return "../../../../tmp/Test/BuildAndLinkLibrary/mergelib.fdb"; }
};

// Register Tests
//------------------------------------------------------------------------------
REGISTER_TESTS_BEGIN( TestBuildAndLinkLibrary )
    REGISTER_TEST( TestParseBFF )
    REGISTER_TEST( TestBuildLib )
    REGISTER_TEST( TestBuildLib_NoRebuild )
    REGISTER_TEST( TestLibMerge )
    REGISTER_TEST( TestLibMerge_NoRebuild )
REGISTER_TESTS_END

// TestStackFramesEmpty
//------------------------------------------------------------------------------
void TestBuildAndLinkLibrary::TestParseBFF() const
{
    FBuildOptions options;
    options.m_ConfigFile = "Data/TestBuildAndLinkLibrary/fbuild.bff";

    FBuild fBuild( options );
    TEST_ASSERT( fBuild.Initialize() );
}

// TestBuildLib
//------------------------------------------------------------------------------
void TestBuildAndLinkLibrary::TestBuildLib() const
{
    FBuildOptions options;
    options.m_ShowSummary = true; // required to generate stats for node count checks
    options.m_ConfigFile = "Data/TestBuildAndLinkLibrary/fbuild.bff";

    FBuild fBuild( options );
    fBuild.Initialize();

    const AStackString<> lib( "../../../../tmp/Test/BuildAndLinkLibrary/test.lib" );
    #if defined( __WINDOWS__ )
        const AStackString<> obj1( "../../../../tmp/Test/BuildAndLinkLibrary/a.obj" );
        const AStackString<> obj2( "../../../../tmp/Test/BuildAndLinkLibrary/b.obj" );
        const AStackString<> obj3( "../../../../tmp/Test/BuildAndLinkLibrary/c.obj" );
    #else
        const AStackString<> obj1( "../../../../tmp/Test/BuildAndLinkLibrary/a.o" );
        const AStackString<> obj2( "../../../../tmp/Test/BuildAndLinkLibrary/b.o" );
        const AStackString<> obj3( "../../../../tmp/Test/BuildAndLinkLibrary/c.o" );
    #endif

    // clean up anything left over from previous runs
    EnsureFileDoesNotExist( lib );
    EnsureFileDoesNotExist( obj1 );
    EnsureFileDoesNotExist( obj2 );
    EnsureFileDoesNotExist( obj3 );

    // Build
    TEST_ASSERT( fBuild.Build( lib ) );
    TEST_ASSERT( fBuild.SaveDependencyGraph( GetBuildLibDBFileName() ) );

    // make sure all output files are as expected
    EnsureFileExists( lib );
    EnsureFileExists( obj1 );
    EnsureFileExists( obj2 );
    EnsureFileExists( obj3 );

    // Check stats
    //               Seen,  Built,  Type
    CheckStatsNode ( 1,     1,      Node::DIRECTORY_LIST_NODE );
    CheckStatsNode ( 6,     3,      Node::FILE_NODE ); // 3 cpps
    CheckStatsNode ( 1,     1,      Node::COMPILER_NODE );
    CheckStatsNode ( 3,     3,      Node::OBJECT_NODE );
    CheckStatsNode ( 1,     1,      Node::LIBRARY_NODE );
    CheckStatsTotal( 12,    9 );
}

// TestBuildLib_NoRebuild
//------------------------------------------------------------------------------
void TestBuildAndLinkLibrary::TestBuildLib_NoRebuild() const
{
    FBuildOptions options;
    options.m_ConfigFile = "Data/TestBuildAndLinkLibrary/fbuild.bff";
    options.m_ShowSummary = true; // required to generate stats for node count checks

    FBuild fBuild( options );
    fBuild.Initialize( GetBuildLibDBFileName() );

    const AStackString<> lib( "../../../../tmp/Test/BuildAndLinkLibrary/test.lib" );

    // Build
    TEST_ASSERT( fBuild.Build( lib ) );

    // Check stats
    //               Seen,  Built,  Type
    CheckStatsNode ( 1,     1,      Node::DIRECTORY_LIST_NODE );
    CheckStatsNode ( 6,     6,      Node::FILE_NODE ); // 3 cpps + 3 headers
    CheckStatsNode ( 1,     0,      Node::COMPILER_NODE );
    CheckStatsNode ( 3,     0,      Node::OBJECT_NODE );
    CheckStatsNode ( 1,     0,      Node::LIBRARY_NODE );
    CheckStatsTotal( 12,    7 );
}

// TestLibMerge
//------------------------------------------------------------------------------
void TestBuildAndLinkLibrary::TestLibMerge() const
{
    FBuildOptions options;
    options.m_ForceCleanBuild = true;
    options.m_ShowSummary = true; // required to generate stats for node count checks
    options.m_ConfigFile = "Data/TestBuildAndLinkLibrary/fbuild.bff";

    FBuild fBuild( options );
    fBuild.Initialize();

    const AStackString<> lib( "../../../../tmp/Test/BuildAndLinkLibrary/merged.lib" );

    // clean up anything left over from previous runs
    EnsureFileDoesNotExist( lib );

    // Build
    TEST_ASSERT( fBuild.Build( lib ) );
    TEST_ASSERT( fBuild.SaveDependencyGraph( GetMergeLibDBFileName() ) );

    // make sure all output files are as expected
    EnsureFileExists( lib );

    // Check stats
    //               Seen,  Built,  Type
    CheckStatsNode ( 6,     3,      Node::FILE_NODE ); // 3x .cpp + 3x .h
    CheckStatsNode ( 1,     1,      Node::COMPILER_NODE );
    CheckStatsNode ( 1,     1,      Node::OBJECT_LIST_NODE );
    CheckStatsNode ( 3,     3,      Node::OBJECT_NODE );
    CheckStatsNode ( 3,     3,      Node::LIBRARY_NODE ); // 2 libs + merge lib
    CheckStatsTotal( 14,    11 );
}

// TestLibMerge_NoRebuild
//------------------------------------------------------------------------------
void TestBuildAndLinkLibrary::TestLibMerge_NoRebuild() const
{
    FBuildOptions options;
    options.m_ConfigFile = "Data/TestBuildAndLinkLibrary/fbuild.bff";
    options.m_ShowSummary = true; // required to generate stats for node count checks

    FBuild fBuild( options );
    fBuild.Initialize( GetMergeLibDBFileName() );

    const AStackString<> lib( "../../../../tmp/Test/BuildAndLinkLibrary/merged.lib" );

    // Build
    TEST_ASSERT( fBuild.Build( lib ) );

    // Check stats
    //               Seen,  Built,  Type
    CheckStatsNode ( 6,     6,      Node::FILE_NODE ); // 3 cpps + 3 headers
    CheckStatsNode ( 1,     0,      Node::COMPILER_NODE );
    CheckStatsNode ( 1,     0,      Node::OBJECT_LIST_NODE );
    CheckStatsNode ( 3,     0,      Node::OBJECT_NODE );
    CheckStatsNode ( 3,     0,      Node::LIBRARY_NODE ); // 2 libs + merge lib
    CheckStatsTotal( 14,    6 );
}

//------------------------------------------------------------------------------
