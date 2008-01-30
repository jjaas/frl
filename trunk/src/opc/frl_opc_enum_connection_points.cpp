#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_enum_connection_points.h"
#include "opc/frl_opc_connection_point.h"

namespace frl
{
	namespace opc
	{
		EnumConnectionPoints::EnumConnectionPoints()
			:	refCount( 0 ), currentIndex( 0 )
		{
		}

		EnumConnectionPoints::EnumConnectionPoints( const std::vector< ConnectionPoint* > &pointsList )
			:	refCount( 0 ), currentIndex( 0 )
		{
			for( std::vector< ConnectionPoint* >::const_iterator it = pointsList.begin(); it != pointsList.end(); ++it )
			{
				ConnectionPoint *point = new ConnectionPoint();
				*point = *( const_cast< ConnectionPoint* >( *it ));
				point->AddRef();
				points.push_back( point );
			}
		}

		EnumConnectionPoints::~EnumConnectionPoints()
		{
			for( std::vector< ConnectionPoint* >::iterator it = points.begin(); it != points.end(); ++it )
			{
				delete (*it);
			}
			points.clear();
		}

		STDMETHODIMP EnumConnectionPoints::QueryInterface( REFIID iid, LPVOID* ppInterface )
		{
			if( ppInterface == NULL )
				return E_INVALIDARG;

			if ( iid == IID_IUnknown )
			{
				*ppInterface = (IUnknown*) this;
			}
			else if ( iid == IID_IEnumConnectionPoints )
			{
				*ppInterface = this;
			}
			else
			{
				*ppInterface = NULL;
				return E_NOINTERFACE;
			}

			AddRef();
			return S_OK;
		}

		ULONG EnumConnectionPoints::AddRef( void )
		{
			return ::InterlockedIncrement( &refCount );
		}

		ULONG EnumConnectionPoints::Release( void )
		{
			LONG tmp = ::InterlockedDecrement( &refCount );
			if( tmp == 0 )
				delete this;
			return tmp;
		}

		HRESULT STDMETHODCALLTYPE EnumConnectionPoints::Next( /* [in] */ ULONG cConnections, /* [length_is][size_is][out] */ LPCONNECTIONPOINT *ppCP, /* [out] */ ULONG *pcFetched )
		{
			if (pcFetched == NULL)
				return E_INVALIDARG;

			*pcFetched = 0;

			if (cConnections == 0)
				return S_OK;

			if( currentIndex >= points.size() )
				return S_FALSE;

			std::vector< ConnectionPoint* >::iterator it = points.begin();
			ULONG i = (ULONG)currentIndex;
			for( ; i < cConnections; i++, ++it )
			{
				if( it == points.end() )
				{
					*pcFetched = i;
					currentIndex = points.size();
					return S_FALSE;
				}
				ppCP[i] = (*it);
				ppCP[i]->AddRef();

			}
			*pcFetched = i;
			currentIndex = i;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE EnumConnectionPoints::Skip( /* [in] */ ULONG cConnections )
		{
			if ( currentIndex + cConnections > points.size())
			{
				currentIndex = points.size();
				return S_FALSE;
			}

			currentIndex += cConnections;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE EnumConnectionPoints::Reset( void )
		{
			currentIndex = 0;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE EnumConnectionPoints::Clone( /* [out] */ IEnumConnectionPoints **ppEnum )
		{
			if( ppEnum == NULL )
				return E_INVALIDARG;

			EnumConnectionPoints *pNewEnum = new EnumConnectionPoints();
			if (pNewEnum == NULL)
			{
				*ppEnum=NULL;
				return E_OUTOFMEMORY;
			}

			std::vector< ConnectionPoint* >::iterator it;
			for( it = points.begin(); it != points.end();  it++ )
			{
				pNewEnum->points.push_back( *it );
			}
			pNewEnum->currentIndex = currentIndex;
			HRESULT hResult = pNewEnum->QueryInterface( IID_IEnumConnectionPoints, (void**) ppEnum );
			return S_OK;
		}

		
	} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
