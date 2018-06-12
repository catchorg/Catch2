
#include "catch.hpp"

#include <iostream> // !DBG
#include <numeric>

namespace Catch {
namespace generators {

    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique( Args&&... args ) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<typename T>
    struct IGenerator {
        virtual ~IGenerator() {}
        virtual auto get( size_t index ) const -> T = 0;
    };

    template<typename T>
    class SingleValueGenerator : public IGenerator<T> {
        T m_value;
    public:
        SingleValueGenerator( T const& value ) : m_value( value ) {}

        auto get( size_t index ) const -> T override {
            assert( index == 0 );
            return m_value;
        }
    };

    template<typename T>
    class FixedValuesGenerator : public IGenerator<T> {
        std::vector<T> m_values;

    public:
        FixedValuesGenerator( std::initializer_list<T> values ) : m_values( values ) {}

        auto get( size_t index ) const -> T override {
            return m_values[index];
        }
    };

    template<typename T>
    class RangeGenerator : public IGenerator<T> {
        T const m_first;
        T const m_last;

    public:
        RangeGenerator( T const& first, T const& last ) : m_first( first ), m_last( last ) {
            assert( m_last > m_first );
        }

        auto get( size_t index ) const -> T override {
            return m_first+index;
        }
    };

    template<typename T>
    class Generator;

    template<typename T>
    auto value( T const& val ) -> Generator<T>;


    template<typename Left, typename Right>
    struct ComposedGeneratorsMustHaveTheSameUnderlyingType;

    template<typename T>
    class Generator {
        size_t m_size;
        std::unique_ptr<IGenerator<T>> m_generator;
        std::vector<Generator<T>> m_generators;

        void add( Generator&& other ) {
            if( other.m_generator )
                m_generators.push_back( std::move( other ) );
            else {
                std::move( other.m_generators.begin(), other.m_generators.end(), std::back_inserter( m_generators ) );
            }
        }
    public:
        Generator() : m_size( 0 ) {}

        Generator( size_t size, std::unique_ptr<IGenerator<T>> generator )
        :   m_size( size ),
            m_generator( std::move( generator ) )
        {}
        Generator( Generator&& g1, Generator&& g2 )
        : m_size( g1.size() + g2.size() ) {
            m_generators.reserve( m_size );
            add( std::move( g1 ) );
            add( std::move( g2 ) );
        }

        auto size() const -> size_t { return m_size; }

        auto operator[]( size_t index ) const -> T {
            assert( index < m_size );
            if( m_generator )
                return m_generator->get( index );

            size_t sizes = 0;
            for( auto const& gen : m_generators ) {
                auto localIndex = index-sizes;
                sizes += gen.size();
                if( index < sizes )
                    return gen[localIndex];
            }
            assert(false); // should never happen
        }

        friend auto operator << ( Generator&& g1, Generator&& g2 ) -> Generator {
            return Generator( std::move(g1), std::move(g2));
        }
        friend auto operator << ( Generator&& g1, T const& val ) -> Generator {
            return Generator( std::move(g1), value( val ) );
        }
        friend auto operator << ( T const& val, Generator&& g2 ) -> Generator {
            return Generator( value( val ), std::move(g2) );
        }
        template<typename U>
        friend auto operator << ( Generator&&, Generator<U>&& ) -> Generator {
            // If you get an error here it's probably because you are trying to compose two generators that
            // are templated on different types.
            // Your error message will probably also show you the line calling this, which should give more context.
            return ComposedGeneratorsMustHaveTheSameUnderlyingType<T, U>();
        }
    };


    template<typename T>
    auto range( T const& first, T const& last ) -> Generator<T> {
        return Generator<T>( 1+last-first, make_unique<RangeGenerator<T>>( first, last ) );
    }
    template<typename T>
    auto values( std::initializer_list<T> values ) -> Generator<T> {
        return Generator<T>( values.size(), make_unique<FixedValuesGenerator<T>>( values ) );
    }
    template<typename T>
    auto value( T const& val ) -> Generator<T> {
        return Generator<T>( 1, make_unique<SingleValueGenerator<T>>( val ) );
    }
}
}

TEST_CASE("Generators") {
    using namespace Catch::generators;

    SECTION( "range" ) {
        auto gen = range(1,3);

        CHECK( gen.size() == 3 );

        CHECK( gen[0] == 1 );
        CHECK( gen[1] == 2 );
        CHECK( gen[2] == 3 );
    }
    SECTION( "fixed values" ) {
        auto gen = values( { 3, 1, 4, 1 } );

        CHECK( gen.size() == 4 );
        CHECK( gen[0] == 3 );
        CHECK( gen[1] == 1 );
        CHECK( gen[2] == 4 );
        CHECK( gen[3] == 1 );
    }
    SECTION( "combined" ) {
        auto gen = range( 1, 2 ) << values( { 9, 7 } );

        CHECK( gen.size() == 4 );
        CHECK( gen[0] == 1 );
        CHECK( gen[1] == 2 );
        CHECK( gen[2] == 9 );
        CHECK( gen[3] == 7 );
    }

    SECTION( "values" ) {
        auto gen = Generator<int>() << 3 << 1;

        CHECK( gen.size() == 2 );
        CHECK( gen[0] == 3 );
        CHECK( gen[1] == 1 );
    }

    SECTION( "values first" ) {
        auto gen = 7 << Generator<int>();

        CHECK( gen.size() == 1 );
        CHECK( gen[0] == 7 );
    }

    //range( 1, 2 ) << values( { 3.1, 7.9 } ); // should error
//    int i = GENERATE( range(1,3) );

}