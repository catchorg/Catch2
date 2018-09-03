/*
 *  Created by Phil Nash on 15/6/2018.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_GENERATORS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_GENERATORS_HPP_INCLUDED

#include "catch_interfaces_generatortracker.h"
#include "catch_common.h"
#include "catch_enforce.h"

#include <memory>
#include <vector>
#include <cassert>

#include <utility>

namespace Catch {
namespace Generators {

    // !TBD move this into its own location?
    namespace pf{
        template<typename T, typename... Args>
        std::unique_ptr<T> make_unique( Args&&... args ) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
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

        auto get( size_t ) const -> T override {
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
            // ToDo:: introduce a safe cast to catch potential overflows
            return static_cast<T>(m_first+index);
        }
    };


    template<typename T>
    struct NullGenerator : IGenerator<T> {
        auto get( size_t ) const -> T override {
            CATCH_INTERNAL_ERROR("A Null Generator is always empty");
        }
    };

    template<typename T>
    class Generator {
        std::unique_ptr<IGenerator<T>> m_generator;
        size_t m_size;

    public:
        Generator( size_t size, std::unique_ptr<IGenerator<T>> generator )
        :   m_generator( std::move( generator ) ),
            m_size( size )
        {}

        auto size() const -> size_t { return m_size; }
        auto operator[]( size_t index ) const -> T {
            assert( index < m_size );
            return m_generator->get( index );
        }
    };

    std::vector<size_t> randomiseIndices( size_t selectionSize, size_t sourceSize );

    template<typename T>
    class GeneratorRandomiser : public IGenerator<T> {
        Generator<T> m_baseGenerator;

        std::vector<size_t> m_indices;
    public:
        GeneratorRandomiser( Generator<T>&& baseGenerator, size_t numberOfItems )
        :   m_baseGenerator( std::move( baseGenerator ) ),
            m_indices( randomiseIndices( numberOfItems, m_baseGenerator.size() ) )
        {}

        auto get( size_t index ) const -> T override {
            return m_baseGenerator[m_indices[index]];
        }
    };

    template<typename T>
    struct RequiresASpecialisationFor;

    template<typename T>
    auto all() -> Generator<T> { return RequiresASpecialisationFor<T>(); }

    template<>
    auto all<int>() -> Generator<int>;


    template<typename T>
    auto range( T const& first, T const& last ) -> Generator<T> {
        return Generator<T>( (last-first), pf::make_unique<RangeGenerator<T>>( first, last ) );
    }

    template<typename T>
    auto random( T const& first, T const& last ) -> Generator<T> {
        auto gen = range( first, last );
        auto size = gen.size();

        return Generator<T>( size, pf::make_unique<GeneratorRandomiser<T>>( std::move( gen ), size ) );
    }
    template<typename T>
    auto random( size_t size ) -> Generator<T> {
        return Generator<T>( size, pf::make_unique<GeneratorRandomiser<T>>( all<T>(), size ) );
    }

    template<typename T>
    auto values( std::initializer_list<T> values ) -> Generator<T> {
        return Generator<T>( values.size(), pf::make_unique<FixedValuesGenerator<T>>( values ) );
    }
    template<typename T>
    auto value( T const& val ) -> Generator<T> {
        return Generator<T>( 1, pf::make_unique<SingleValueGenerator<T>>( val ) );
    }

    template<typename T>
    auto as() -> Generator<T> {
        return Generator<T>( 0, pf::make_unique<NullGenerator<T>>() );
    }

    template<typename... Ts>
    auto table( std::initializer_list<std::tuple<Ts...>>&& tuples ) -> Generator<std::tuple<Ts...>> {
        return values<std::tuple<Ts...>>( std::forward<std::initializer_list<std::tuple<Ts...>>>( tuples ) );
    }


    template<typename T>
    struct Generators : GeneratorBase {
        std::vector<Generator<T>> m_generators;

        using type = T;

        Generators() : GeneratorBase( 0 ) {}

        void populate( T&& val ) {
            m_size += 1;
            m_generators.emplace_back( value( std::move( val ) ) );
        }
        template<typename U>
        void populate( U&& val ) {
            populate( T( std::move( val ) ) );
        }
        void populate( Generator<T>&& generator ) {
            m_size += generator.size();
            m_generators.emplace_back( std::move( generator ) );
        }

        template<typename U, typename... Gs>
        void populate( U&& valueOrGenerator, Gs... moreGenerators ) {
            populate( std::forward<U>( valueOrGenerator ) );
            populate( std::forward<Gs>( moreGenerators )... );
        }

        auto operator[]( size_t index ) const -> T {
            size_t sizes = 0;
            for( auto const& gen : m_generators ) {
                auto localIndex = index-sizes;
                sizes += gen.size();
                if( index < sizes )
                    return gen[localIndex];
            }
            CATCH_INTERNAL_ERROR("Index '" << index << "' is out of range (" << sizes << ')');
        }
    };

    template<typename T, typename... Gs>
    auto makeGenerators( Generator<T>&& generator, Gs... moreGenerators ) -> Generators<T> {
        Generators<T> generators;
        generators.m_generators.reserve( 1+sizeof...(Gs) );
        generators.populate( std::move( generator ), std::forward<Gs>( moreGenerators )... );
        return generators;
    }
    template<typename T>
    auto makeGenerators( Generator<T>&& generator ) -> Generators<T> {
        Generators<T> generators;
        generators.populate( std::move( generator ) );
        return generators;
    }
    template<typename T, typename... Gs>
    auto makeGenerators( T&& val, Gs... moreGenerators ) -> Generators<T> {
        return makeGenerators( value( std::forward<T>( val ) ), std::forward<Gs>( moreGenerators )... );
    }
    template<typename T, typename U, typename... Gs>
    auto makeGenerators( U&& val, Gs... moreGenerators ) -> Generators<T> {
        return makeGenerators( value( T( std::forward<U>( val ) ) ), std::forward<Gs>( moreGenerators )... );
    }


    auto acquireGeneratorTracker( SourceLineInfo const& lineInfo ) -> IGeneratorTracker&;

    template<typename L>
    // Note: The type after -> is weird, because VS2015 cannot parse
    //       the expression used in the typedef inside, when it is in
    //       return type. Yeah, ¯\_(ツ)_/¯
    auto generate( SourceLineInfo const& lineInfo, L const& generatorExpression ) -> decltype(std::declval<decltype(generatorExpression())>()[0]) {
        using UnderlyingType = typename decltype(generatorExpression())::type;

        IGeneratorTracker& tracker = acquireGeneratorTracker( lineInfo );
        if( !tracker.hasGenerator() )
            tracker.setGenerator( pf::make_unique<Generators<UnderlyingType>>( generatorExpression() ) );

        auto const& generator = static_cast<Generators<UnderlyingType> const&>( *tracker.getGenerator() );
        return generator[tracker.getIndex()];
    }

} // namespace Generators
} // namespace Catch

#define GENERATE( ... ) \
    Catch::Generators::generate( CATCH_INTERNAL_LINEINFO, []{ using namespace Catch::Generators; return makeGenerators( __VA_ARGS__ ); } )


#endif // TWOBLUECUBES_CATCH_GENERATORS_HPP_INCLUDED
