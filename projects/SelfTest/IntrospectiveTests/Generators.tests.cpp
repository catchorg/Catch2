
#include "catch.hpp"

#include <iostream> // !DBG
#include <numeric>
#include <map>

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

    class GeneratorBase {
    protected:
        size_t const m_size;

    public:
        GeneratorBase( size_t size ) : m_size( size ) {}
        virtual ~GeneratorBase();
        auto size() const -> size_t { return m_size; }
    };

    GeneratorBase::~GeneratorBase() {}

    template<typename T>
    class Generator : public GeneratorBase {
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
        using type = T;

        Generator() : GeneratorBase( 0 ) {}

        Generator( size_t size, std::unique_ptr<IGenerator<T>> generator )
        :   GeneratorBase( size ),
            m_generator( std::move( generator ) )
        {}
        Generator( Generator&& g1, Generator&& g2 )
        : GeneratorBase( g1.size() + g2.size() ) {
            m_generators.reserve( m_size );
            add( std::move( g1 ) );
            add( std::move( g2 ) );
        }

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

    using GeneratorBasePtr = std::unique_ptr<GeneratorBase>;

    class GeneratorCache {
        std::map<std::string, GeneratorBasePtr> m_generators;

    public:
        template<typename T>
        auto add( std::string const& id, Generator<T>&& generator ) -> Generator<T> const& {
            auto generatorPtr = make_unique<Generator<T>>( std::move( generator ) );
            auto const& storedGenerator = *generatorPtr;
            m_generators.insert( { id, std::move( generatorPtr ) } );
            return storedGenerator;
        }

        template<typename T, typename L>
        auto getGenerator( std::string const& id, L const& generatorExpression ) -> Generator<T> const& {

            auto it = m_generators.find( id );
            return ( it != m_generators.end() )
                ? static_cast<Generator<T> const&>( *it->second )
                : add( id, generatorExpression() );
        }

    };
//    auto getGeneratorCache() -> GeneratorCache& {
//        static GeneratorCache s_cache;
//        return s_cache;
//    }

    template<typename L>
    auto generate( GeneratorCache& cache, std::string const& id, size_t index, L const& generatorExpression ) -> typename decltype(generatorExpression())::type {

        using UnderlyingType = typename decltype(generatorExpression())::type;

        auto const& generator = cache.getGenerator<UnderlyingType, L>( id, generatorExpression );

        return generator[index];
    }


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

    SECTION( "type erasure" ) {
        auto gen = range( 7, 9 ) << 11;

        // Make type erased version
        auto dynCopy = make_unique<Generator<int>>( std::move( gen ) );
        std::unique_ptr<GeneratorBase const> base = std::move( dynCopy );

        // Only thing we can do is ask for the size
        CHECK( base->size() == 4 );

        // Restore typed version
        auto typed = dynamic_cast<Generator<int> const*>( base.get() );
        REQUIRE( typed );
        CHECK( typed->size() == 4 );
        CHECK( (*typed)[0] == 7 );
        CHECK( (*typed)[3] == 11 );
    }

    SECTION( "memoized" ) {
        GeneratorCache cache;

        std::string id = "test";

        SECTION( "type is deducible" ) {
            using GeneratorType = decltype( range( 11, 14 ) << 27 );
            REQUIRE( std::is_same<GeneratorType, Generator<int>>::value );
            REQUIRE( std::is_same<GeneratorType::type, int>::value );
        }

        SECTION( "macro2" ) {
            int created = 0;
            auto fun = [&]{
                created++;
                return values({42, 7});
            };

            CHECK( generate( cache, id, 0, fun ) == 42 );
            CHECK( created == 1 );
            CHECK( generate( cache, id, 0, fun ) == 42 );
            CHECK( created == 1 );
            CHECK( generate( cache, id, 1, fun ) == 7 );
            CHECK( created == 1 );
        }
    }

    //range( 1, 2 ) << values( { 3.1, 7.9 } ); // should error
//    int i = GENERATE( range(1,3) );

}
