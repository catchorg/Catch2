// 110-Fix-ClassFixture.cpp

// Catch has two ways to express fixtures:
// - Sections
// - Traditional class-based fixtures (this file)

// main() provided in 000-CatchMain.cpp

#include <catch2/catch_test_macros.hpp>

class DBConnection
{
public:
    static DBConnection createConnection( std::string const & /*dbName*/ ) {
        return DBConnection();
    }

    bool executeSQL( std::string const & /*query*/, int const /*id*/, std::string const & arg ) {
        if ( arg.length() == 0 ) {
            throw std::logic_error("empty SQL query argument");
        }
        return true; // ok
    }
};

class UniqueTestsFixture
{
protected:
    UniqueTestsFixture()
    : conn( DBConnection::createConnection( "myDB" ) )
    {}

    int getID() {
        return ++uniqueID;
    }

protected:
    DBConnection conn;

private:
    static int uniqueID;
};

int UniqueTestsFixture::uniqueID = 0;

TEST_CASE_METHOD( UniqueTestsFixture, "Create Employee/No Name", "[create]" ) {
    REQUIRE_THROWS( conn.executeSQL( "INSERT INTO employee (id, name) VALUES (?, ?)", getID(), "") );
}

TEST_CASE_METHOD( UniqueTestsFixture, "Create Employee/Normal", "[create]" ) {
    REQUIRE( conn.executeSQL( "INSERT INTO employee (id, name) VALUES (?, ?)", getID(), "Joe Bloggs" ) );
}

// Compile & run:
// - g++ -std=c++11 -Wall -I$(CATCH_SINGLE_INCLUDE) -o 110-Fix-ClassFixture 110-Fix-ClassFixture.cpp 000-CatchMain.o && 110-Fix-ClassFixture --success
// - cl -EHsc -I%CATCH_SINGLE_INCLUDE% 110-Fix-ClassFixture.cpp 000-CatchMain.obj && 110-Fix-ClassFixture --success

// Expected compact output (all assertions):
//
// prompt> 110-Fix-ClassFixture.exe --reporter compact --success
// 110-Fix-ClassFixture.cpp:47: passed: conn.executeSQL( "INSERT INTO employee (id, name) VALUES (?, ?)", getID(), "")
// 110-Fix-ClassFixture.cpp:51: passed: conn.executeSQL( "INSERT INTO employee (id, name) VALUES (?, ?)", getID(), "Joe Bloggs" ) for: true
// Passed both 2 test cases with 2 assertions.
