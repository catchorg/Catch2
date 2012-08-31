/*
 *  Created by Phil on 02/05/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_PTR_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_PTR_HPP_INCLUDED

#include "catch_common.h"

namespace Catch {

    // An intrusive reference counting smart pointer.
    // T must implement addRef() and release() methods
    // typically implementing the IShared interface
    template<typename T>
    class Ptr {
    public:
        Ptr() : m_p( NULL ){}
        Ptr( T* p ) : m_p( p ){
            if( m_p )
                m_p->addRef();
        }
        Ptr( const Ptr& other ) : m_p( other.m_p ){
            if( m_p )
                m_p->addRef();
        }
        ~Ptr(){
            if( m_p )
                m_p->release();
        }
        Ptr& operator = ( T* p ){
            Ptr temp( p );
            swap( temp );
            return *this;
        }
        Ptr& operator = ( const Ptr& other ){
            Ptr temp( other );
            swap( temp );
            return *this;
        }
        void swap( Ptr& other ){
            std::swap( m_p, other.m_p );
        }
        
        T* get(){
            return m_p;
        }
        const T* get() const{
            return m_p;
        }
        
        T& operator*() const {
            return *m_p;
        }

        T* operator->() const {
            return m_p;
        }
        
        bool operator !() const {
            return m_p == NULL;
        }
        
    private:
        T* m_p;
    };
    
    struct IShared : NonCopyable {
        virtual ~IShared();
        virtual void addRef() = 0;
        virtual void release() = 0;
    };
    
    template<typename T>
    struct SharedImpl : T {
        
        SharedImpl() : m_rc( 0 ){}
        
        virtual void addRef(){
            ++m_rc;
        }
        virtual void release(){
            if( --m_rc == 0 )
                delete this;
        }
        
        int m_rc;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_PTR_HPP_INCLUDED
